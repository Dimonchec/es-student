#include "telemetry-task/telemetry-task.h"
#include "platform/platform.h"
#include "systime/systime.h"
#include <stddef.h>

static telemetry_state_t state = TELEMETRY_STATE_IDLE;
static uint32_t period_ms = 100;
static uint64_t last_record_us = 0;
static telemetry_task_callback_t on_record_callback = NULL;

void telemetry_task_init(telemetry_task_callback_t on_record)
{
    on_record_callback = on_record;
    state = TELEMETRY_STATE_IDLE;
}

void telemetry_task_handle(void)
{
    if (state != TELEMETRY_STATE_RUN) return;
    if (on_record_callback == NULL) return;

    if (systime_period_elapsed(&last_record_us, period_ms * 1000))
    {
        telemetry_record_t record;
        record.time_us = systime_us();
        record.voltage_v = platform_voltage_read();
        record.temp_c = platform_temp_read();
        
        on_record_callback(&record);
    }
}

bool telemetry_task_start(uint32_t new_period_ms)
{
    if (new_period_ms < 10) return false; // Период меньше 10 мс не принимаем

    period_ms = new_period_ms;
    last_record_us = systime_us(); // Первая запись — на ближайшей итерации
    state = TELEMETRY_STATE_RUN;
    return true;
}

void telemetry_task_stop(void)
{
    state = TELEMETRY_STATE_IDLE;
}

telemetry_state_t telemetry_task_get_state(void) { return state; }
uint32_t telemetry_task_get_period_ms(void) { return period_ms; }