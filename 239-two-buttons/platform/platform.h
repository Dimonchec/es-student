#pragma once
#include <stdbool.h>

// Перечисление кнопок
typedef enum {
    PLATFORM_BUTTON_1 = 0,
    PLATFORM_BUTTON_2 = 1,
    PLATFORM_BUTTON_COUNT
} platform_button_t;

void platform_init(void);
void platform_led_set(bool on);
bool platform_button_read(platform_button_t button);