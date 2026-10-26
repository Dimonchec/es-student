#pragma once

#include <stdint.h>

#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.2.0"
#define DEVICE_PROJECT "231-firmware-template"
#define DEVICE_REPO "https://github.com/Dimonchec/es-student"

// Запасное значение на случай, если сборка не передала тип платы
#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

// Только одна функция — паспорт устройства
void device_info(void);