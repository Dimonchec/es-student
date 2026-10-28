#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> // <--- ДОБАВИТЬ ВОТ ЭТУ СТРОКУ

#ifndef PROFILING_ENABLED
#define PROFILING_ENABLED 1
#endif


#define PROFILING_MAX_STOPWATCHES 5

typedef struct {
    const char *name;
    uint32_t count;
    uint32_t mean_us;
    uint32_t max_us;
} profiling_result_t;

void profiling_init(void);
void profiling_stopwatch_init(uint32_t id, const char *name);
bool profiling_get(uint32_t id, profiling_result_t *result);
void profiling_reset(void);

#if PROFILING_ENABLED
void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);
#else
#define profiling_start(id) ((void)0)
#define profiling_stop(id) ((void)0)
#endif