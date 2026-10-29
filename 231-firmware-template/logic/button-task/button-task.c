#include "button-task/button-task.h"
#include "platform/platform.h"
#include "systime/systime.h"
#include <stddef.h>

#define DEBOUNCE_TIME_US 20000

typedef enum { BUTTON_STATE_RELEASED, BUTTON_STATE_PRESSING, BUTTON_STATE_PRESSED, BUTTON_STATE_RELEASING } button_state_t;

static button_state_t button_state = BUTTON_STATE_RELEASED;
static uint64_t last_change_us = 0;
static uint32_t press_count = 0;
static button_task_callback_t on_press_callback = NULL;

void button_task_init(button_task_callback_t on_press)
{
    on_press_callback = on_press;
    button_state = BUTTON_STATE_RELEASED;
    last_change_us = systime_us();
    press_count = 0;
}

void button_task_handle(void)
{
    bool pressed = platform_button_read();
    uint64_t now = systime_us();

    switch (button_state)
    {
        case BUTTON_STATE_RELEASED:
            if (pressed) { last_change_us = now; button_state = BUTTON_STATE_PRESSING; }
            break;
        case BUTTON_STATE_PRESSING:
            if (!pressed) button_state = BUTTON_STATE_RELEASED;
            else if (now - last_change_us >= DEBOUNCE_TIME_US)
            {
                press_count++;
                if (on_press_callback != NULL) on_press_callback(); // <--- Вызов callback
                button_state = BUTTON_STATE_PRESSED;
            }
            break;
        case BUTTON_STATE_PRESSED:
            if (!pressed) { last_change_us = now; button_state = BUTTON_STATE_RELEASING; }
            break;
        case BUTTON_STATE_RELEASING:
            if (pressed) button_state = BUTTON_STATE_PRESSED;
            else if (now - last_change_us >= DEBOUNCE_TIME_US) button_state = BUTTON_STATE_RELEASED;
            break;
    }
}

bool button_task_is_pressed(void) { return (button_state == BUTTON_STATE_PRESSING || button_state == BUTTON_STATE_PRESSED || button_state == BUTTON_STATE_RELEASING); }
uint32_t button_task_get_press_count(void) { return press_count; }