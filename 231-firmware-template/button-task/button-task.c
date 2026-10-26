#include "button-task.h"
#include "led-task.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define BUTTON_PIN 15
#define DEBOUNCE_TIME_US 20000 // 20 мс

typedef enum {
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_PRESSING,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASING,
} button_state_t;

static button_state_t button_state = BUTTON_STATE_RELEASED;
static uint64_t last_change_us = 0;
static uint32_t press_count = 0;

void button_task_init(void)
{
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    
    button_state = BUTTON_STATE_RELEASED;
    last_change_us = time_us_64();
    press_count = 0;
}

void button_task_handle(void)
{
    // Кнопка подтянута к питанию, поэтому нажатие замыкает на землю (0)
    bool level = gpio_get(BUTTON_PIN); 
    uint64_t now_us = time_us_64();

    switch (button_state)
    {
        case BUTTON_STATE_RELEASED:
            if (!level) // Нажата
            {
                last_change_us = now_us;
                button_state = BUTTON_STATE_PRESSING;
            }
            break;

        case BUTTON_STATE_PRESSING:
            if (level) // Отпущена
            {
                button_state = BUTTON_STATE_RELEASED;
            }
            else if (now_us - last_change_us >= DEBOUNCE_TIME_US)
            {
                press_count++;
                led_task_next_state(); // Переключаем светодиод
                button_state = BUTTON_STATE_PRESSED;
            }
            break;

        case BUTTON_STATE_PRESSED:
            if (level) // Отпущена
            {
                last_change_us = now_us;
                button_state = BUTTON_STATE_RELEASING;
            }
            break;

        case BUTTON_STATE_RELEASING:
            if (!level) // Снова нажата (дребезг)
            {
                button_state = BUTTON_STATE_PRESSED;
            }
            else if (now_us - last_change_us >= DEBOUNCE_TIME_US)
            {
                button_state = BUTTON_STATE_RELEASED;
            }
            break;
    }
}

bool button_task_is_pressed(void)
{
    // Кнопка считается нажатой, если мы в состоянии PRESSING, PRESSED или RELEASING
    return (button_state == BUTTON_STATE_PRESSING || 
            button_state == BUTTON_STATE_PRESSED || 
            button_state == BUTTON_STATE_RELEASING);
}

uint32_t button_task_get_press_count(void)
{
    return press_count;
}