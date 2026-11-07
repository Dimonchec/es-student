#include "platform/platform.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pico/time.h"

#define LED_PIN 25
#define BUTTON_PIN 15

#define ADC_PIN 26
#define ADC_CHANNEL 0
#define ADC_VREF_V 3.3f
#define ADC_LEVELS 4096

#define ADC_TEMP_CHANNEL 4 

#define CHARGE_PIN 22

static volatile adc_user_t adc_user = ADC_FREE; // <--- ДОБАВИТЬ

static repeating_timer_t capture_timer;
static uint16_t capture_samples[PLATFORM_CAPTURE_MAX];
static uint32_t capture_times[PLATFORM_CAPTURE_MAX];
static uint32_t capture_count;
static uint32_t capture_period_us;
static volatile uint32_t capture_index;
static volatile uint32_t capture_handler_us;

static bool on_capture(repeating_timer_t *timer)
{
    (void)timer;
    uint32_t start_us = time_us_32();
    capture_times[capture_index] = start_us;
    capture_samples[capture_index] = adc_read();
    capture_index++;
    capture_handler_us += time_us_32() - start_us;

    if (capture_index < capture_count)
    {
        return true;
    }

    // Захват окончен: АЦП свободен, будильник снимается
    adc_user = ADC_FREE;
    return false;
}

void platform_init(void)
{
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_init(CHARGE_PIN);
    gpio_set_dir(CHARGE_PIN, GPIO_OUT);
    gpio_put(CHARGE_PIN, 0);

    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_set_temp_sensor_enabled(true);
}

void platform_led_set(bool on)
{
    gpio_put(LED_PIN, on);
}

bool platform_button_read(void)
{
    return !gpio_get(BUTTON_PIN); // Инвертируем, так как кнопка замыкает на землю
}

float platform_voltage_read(void)
{
    adc_select_input(ADC_CHANNEL);
    return adc_read() * ADC_VREF_V / ADC_LEVELS;
}

float platform_temp_read(void)
{
    adc_select_input(ADC_TEMP_CHANNEL);
    float voltage = adc_read() * ADC_VREF_V / ADC_LEVELS;
    // Формула из даташита: T = 27 - (U - 0.706) / 0.001721
    return 27.0f - (voltage - 0.706f) / 0.001721f;
}

void platform_charge_set(bool on)
{
    gpio_put(CHARGE_PIN, on);
}

// --- Будильник индикатора ---
static repeating_timer_t blink_timer;
static bool blink_running = false;
static volatile bool blink_on = false;
static volatile uint32_t blink_toggles = 0;

// Обработчик будильника
static bool on_blink(repeating_timer_t *timer)
{
    (void)timer;
    blink_on = !blink_on;
    gpio_put(LED_PIN, blink_on);
    blink_toggles++;
    return true; // true — вызвать снова через тот же период
}

bool platform_blink_start(uint32_t period_ms)
{
    if (period_ms < 10 || period_ms > 60000)
    {
        return false;
    }

    platform_blink_stop();
    blink_toggles = 0;

    // Минус перед периодом — отсчитывать от начала прошлого вызова
    blink_running = add_repeating_timer_ms(-(int32_t)period_ms, on_blink, NULL, &blink_timer);
    return blink_running;
}

uint32_t platform_blink_stop(void)
{
    if (blink_running)
    {
        cancel_repeating_timer(&blink_timer);
        blink_running = false;
    }

    blink_on = false;
    gpio_put(LED_PIN, 0);
    return blink_toggles;
}

bool platform_capture_start(uint32_t count, uint32_t period_us)
{
    if (adc_user != ADC_FREE || count == 0 || count > PLATFORM_CAPTURE_MAX || period_us == 0)
    {
        return false;
    }

    adc_select_input(ADC_CHANNEL);
    capture_count = count;
    capture_period_us = period_us;
    capture_index = 0;
    capture_handler_us = 0;
    adc_user = ADC_CAPTURE;

    if (!add_repeating_timer_us(-(int64_t)period_us, on_capture, NULL, &capture_timer))
    {
        adc_user = ADC_FREE;
        return false;
    }
    return true;
}

bool platform_adc_busy(void)
{
    return adc_user != ADC_FREE;
}

bool platform_capture_done(void)
{
    return capture_index >= capture_count && capture_count > 0;
}

void platform_capture_get(platform_capture_t *capture)
{
    if (capture_count == 0) {
        capture->samples = capture_samples;
        capture->count = 0;
        capture->period_us = 0;
        capture->min_interval_us = 0;
        capture->max_interval_us = 0;
        capture->handler_percent = 0.0f;
        return;
    }   

    // Разброс интервалов: min и max
    uint32_t min_int = UINT32_MAX;
    uint32_t max_int = 0;
    for (uint32_t i = 1; i < capture_count; i++)
    {
        uint32_t diff = capture_times[i] - capture_times[i - 1];
        if (diff < min_int) min_int = diff;
        if (diff > max_int) max_int = diff;
    }
    capture->min_interval_us = (capture_count > 1) ? min_int : 0;
    capture->max_interval_us = (capture_count > 1) ? max_int : 0;

    // Доля времени ядра внутри обработчика
    uint32_t total_time = capture_times[capture_count - 1] - capture_times[0];
    capture->handler_percent = (total_time > 0) ? (100.0f * capture_handler_us / total_time) : 0.0f;
}
