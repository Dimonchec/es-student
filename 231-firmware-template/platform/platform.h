#pragma once
#include <stdbool.h>

// Инициализация платформы (GPIO, stdio)
void platform_init(void);

// Управление светодиодом
void platform_led_set(bool on);
bool platform_led_get(void);

// Чтение кнопки (без подавления дребезга)
bool platform_button_read(void);