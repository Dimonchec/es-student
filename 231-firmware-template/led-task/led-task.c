#include "led-task.h"
#include "led.h"
#include "pico/stdlib.h"

// Статические переменные задачи (скрыты от внешнего мира)
static led_state_t led_state = LED_STATE_BLINK;
static uint32_t period_ms = 1000;
static uint64_t last_toggle_us = 0;

// Вспомогательная функция для проверки периода
static bool period_elapsed(uint64_t *last_us, uint64_t period_us)
{
    uint64_t now_us = time_us_64();
    if (now_us - *last_us >= period_us)
    {
        *last_us += period_us; // Сдвигаем метку на период
        return true;
    }
    return false;
}

void led_task_init(void)
{
    led_init();
    led_task_set_state(LED_STATE_BLINK); // Начальное состояние — мигает
}

void led_task_set_state(led_state_t state)
{
    led_state = state;

    // Действие при входе в состояние
    switch (led_state)
    {
        case LED_STATE_OFF:
            led_set(false);
            break;
        case LED_STATE_ON:
            led_set(true);
            break;
        case LED_STATE_BLINK:
            // Первое переключение должно случиться сразу (метка в прошлом)
            last_toggle_us = time_us_64() - (period_ms * 1000);
            break;
    }
}

void led_task_handle(void)
{
    switch (led_state)
    {
        case LED_STATE_OFF:
        case LED_STATE_ON:
            // Ничего не делаем, светодиод уже установлен при входе
            break;

        case LED_STATE_BLINK:
            if (period_elapsed(&last_toggle_us, period_ms * 1000))
            {
                led_toggle();
            }
            break;
    }
}

bool led_task_set_period_ms(uint32_t new_period_ms)
{
    if (new_period_ms == 0)
    {
        return false;
    }
    period_ms = new_period_ms;
    
    // Если мигаем, сбрасываем метку, чтобы применился новый период
    if (led_state == LED_STATE_BLINK)
    {
        last_toggle_us = time_us_64() - (period_ms * 1000);
    }
    return true;
}

uint32_t led_task_get_period_ms(void)
{
    return period_ms;
}
led_state_t led_task_get_state(void)
{
    return led_state;
}