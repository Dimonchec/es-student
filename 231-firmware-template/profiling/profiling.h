#pragma once
#include <stdint.h>
#include <stdbool.h>

// Максимальное количество секундомеров
#define PROFILING_MAX_STOPWATCHES 5

// Структура результата
typedef struct {
    const char *name;
    uint32_t count;
    uint32_t mean_us;
    uint32_t max_us;
} profiling_result_t;

// Инициализация
void profiling_init(void);

// Настройка и запуск
void profiling_stopwatch_init(uint32_t id, const char *name);
void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);

// Получение результата (false, если id не задан)
bool profiling_get(uint32_t id, profiling_result_t *result);

// Сброс всех секундомеров
void profiling_reset(void);