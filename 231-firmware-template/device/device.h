#pragma once
#include <stdint.h>

#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.2.0" // Укажите вашу версию
#define DEVICE_PROJECT "231-firmware-template"
#define DEVICE_REPO "https://github.com/Dimonchec/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

// Структура для паспорта (как требует задание)
typedef struct {
    const char *project;
    const char *version;
    const char *repo;
    const char *board;
    char serial[17]; // 16 hex-цифр + '\0'
    uint32_t manufacturer;
    uint32_t part;
    uint32_t revision;
    const char *sdk_version;
} device_info_t;

// Функция заполнения структуры
void device_get_info(device_info_t *info);