#pragma once

#include <stdint.h> // <--- ОБЯЗАТЕЛЬНО для uint8_t и uint32_t

#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.0.0"

#define DEVICE_PROJECT "211-command-usb"
#define DEVICE_REPO "https://github.com/Dimonchec/es-student"

// Запасное значение на случай, если сборка не передала тип платы
#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

struct info_t
{
    uint32_t version;
    char name[13];
    uint8_t revision;
};

// Внешняя переменная, чтобы её видел модуль memory
extern struct info_t device_card;

// Прототип функции
void device_info(void);
void dev_info(void); // <--- Прототип новой функции