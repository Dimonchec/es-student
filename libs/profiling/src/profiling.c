#include "profiling/profiling.h"
#include "systime/systime.h" // <--- Заменили на systime

typedef struct {
    const char *name;
    bool active;
    uint64_t start_us;
    uint32_t count;
    uint32_t sum_us;
    uint32_t max_us;
} stopwatch_t;

static stopwatch_t stopwatches[PROFILING_MAX_STOPWATCHES];

void profiling_init(void) { for (int i = 0; i < PROFILING_MAX_STOPWATCHES; i++) { stopwatches[i].name = NULL; stopwatches[i].active = false; stopwatches[i].count = 0; stopwatches[i].sum_us = 0; stopwatches[i].max_us = 0; } }
void profiling_stopwatch_init(uint32_t id, const char *name) { if (id < PROFILING_MAX_STOPWATCHES) { stopwatches[id].name = name; stopwatches[id].active = false; stopwatches[id].count = 0; stopwatches[id].sum_us = 0; stopwatches[id].max_us = 0; } }
bool profiling_get(uint32_t id, profiling_result_t *result) { if (id >= PROFILING_MAX_STOPWATCHES || stopwatches[id].name == NULL) return false; result->name = stopwatches[id].name; result->count = stopwatches[id].count; result->max_us = stopwatches[id].max_us; result->mean_us = (stopwatches[id].count > 0) ? (stopwatches[id].sum_us / stopwatches[id].count) : 0; return true; }
void profiling_reset(void) { for (int i = 0; i < PROFILING_MAX_STOPWATCHES; i++) { stopwatches[i].count = 0; stopwatches[i].sum_us = 0; stopwatches[i].max_us = 0; stopwatches[i].active = false; } }

#if PROFILING_ENABLED
void profiling_start(uint32_t id) { if (id < PROFILING_MAX_STOPWATCHES) { stopwatches[id].start_us = systime_us(); stopwatches[id].active = true; } }
void profiling_stop(uint32_t id) { if (id < PROFILING_MAX_STOPWATCHES && stopwatches[id].active) { uint64_t duration = systime_us() - stopwatches[id].start_us; stopwatches[id].active = false; stopwatches[id].count++; stopwatches[id].sum_us += (uint32_t)duration; if (duration > stopwatches[id].max_us) stopwatches[id].max_us = (uint32_t)duration; } }
#endif