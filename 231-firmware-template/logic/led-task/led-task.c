#include "led-task/led-task.h"
#include "platform/platform.h"
#include "systime/systime.h"

static led_state_t led_state = LED_STATE_BLINK;
static uint32_t period_ms = 1000;
static uint64_t last_toggle_us = 0;
static bool led_is_on = false; // Локальное состояние для мигания

// Вспомогательная функция для проверки периода
static bool period_elapsed(uint64_t *last_us, uint64_t period_us)
{
    uint64_t now_us = systime_us();
    if (now_us - *last_us >= period_us)
    {
        *last_us += period_us;
        return true;
    }
    return false;
}

void led_task_init(void)
{
    led_task_set_state(LED_STATE_BLINK);
}

void led_task_set_state(led_state_t state)
{
    led_state = state;
    switch (led_state)
    {
        case LED_STATE_OFF:
            led_is_on = false;
            platform_led_set(false);
            break;
        case LED_STATE_ON:
            led_is_on = true;
            platform_led_set(true);
            break;
        case LED_STATE_BLINK:
            // Действие при входе в состояние "мигает"
            last_toggle_us = systime_us() - (period_ms * 1000);
            led_is_on = true; // Начинаем с включенного
            platform_led_set(true);
            break;
    }
}

void led_task_handle(void)
{
    switch (led_state)
    {
        case LED_STATE_OFF:
        case LED_STATE_ON:
            break;

        case LED_STATE_BLINK:
            if (systime_us() - last_toggle_us >= period_ms * 1000) 
            {
                last_toggle_us += period_ms * 1000;
                led_is_on = !led_is_on;
                platform_led_set(led_is_on);
            }
            break;
    }
}

bool led_task_set_period_ms(uint32_t new_period_ms)
{
    if (new_period_ms == 0) return false;
    period_ms = new_period_ms;
    if (led_state == LED_STATE_BLINK) 
    {
        last_toggle_us = systime_us() - (period_ms * 1000);
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

void led_task_next_state(void)
{
    switch (led_state)
    {
        case LED_STATE_OFF: led_task_set_state(LED_STATE_ON); break;
        case LED_STATE_ON: led_task_set_state(LED_STATE_BLINK); break;
        case LED_STATE_BLINK: led_task_set_state(LED_STATE_OFF); break;
    }
}