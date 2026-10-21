#pragma once

#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.0.0"

#define DEVICE_PROJECT "134-led-module"
#define DEVICE_REPO "https://github.com/Dimonchec/es-student"

// Запасное значение на случай, если сборка не передала тип платы
#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

// Прототип функции
void device_info(void);