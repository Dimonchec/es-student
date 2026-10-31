#include "button-task/button-task.h"
#include "systime/systime.h"
#include <stddef.h>

#define DEBOUNCE_TIME_US 20000

typedef enum { BUTTON_STATE_RELEASED, BUTTON_STATE_PRESSING, BUTTON_STATE_PRESSED, BUTTON_STATE_RELEASING } button_state_t;

typedef struct {
    button_state_t state;
    uint64_t last_change_us;
    uint32_t press_count;
    button_task_callback_t on_press;
} button_context_t;

static button_context_t buttons[PLATFORM_BUTTON_COUNT];

void button_task_init(platform_button_t button, button_task_callback_t on_press)
{
    if (button >= PLATFORM_BUTTON_COUNT) return;
    buttons[button].state = BUTTON_STATE_RELEASED;
    buttons[button].last_change_us = systime_us();
    buttons[button].press_count = 0;
    buttons[button].on_press = on_press;
}

void button_task_handle(void)
{
    uint64_t now = systime_us();

    for (int i = 0; i < PLATFORM_BUTTON_COUNT; i++) {
        bool pressed = platform_button_read((platform_button_t)i);
        button_context_t *ctx = &buttons[i];

        switch (ctx->state)
        {
            case BUTTON_STATE_RELEASED:
                if (pressed) { ctx->last_change_us = now; ctx->state = BUTTON_STATE_PRESSING; }
                break;
            case BUTTON_STATE_PRESSING:
                if (!pressed) ctx->state = BUTTON_STATE_RELEASED;
                else if (now - ctx->last_change_us >= DEBOUNCE_TIME_US)
                {
                    ctx->press_count++;
                    if (ctx->on_press != NULL) ctx->on_press();
                    ctx->state = BUTTON_STATE_PRESSED;
                }
                break;
            case BUTTON_STATE_PRESSED:
                if (!pressed) { ctx->last_change_us = now; ctx->state = BUTTON_STATE_RELEASING; }
                break;
            case BUTTON_STATE_RELEASING:
                if (pressed) ctx->state = BUTTON_STATE_PRESSED;
                else if (now - ctx->last_change_us >= DEBOUNCE_TIME_US) ctx->state = BUTTON_STATE_RELEASED;
                break;
        }
    }
}

bool button_task_is_pressed(platform_button_t button)
{
    if (button >= PLATFORM_BUTTON_COUNT) return false;
    button_state_t s = buttons[button].state;
    return (s == BUTTON_STATE_PRESSING || s == BUTTON_STATE_PRESSED || s == BUTTON_STATE_RELEASING);
}

uint32_t button_task_get_press_count(platform_button_t button)
{
    if (button >= PLATFORM_BUTTON_COUNT) return 0;
    return buttons[button].press_count;
}