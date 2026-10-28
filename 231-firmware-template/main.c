#include <stdio.h>
#include "pico/stdlib.h"

// Библиотеки из libs/
#include "profiling/profiling.h"
#include "stdio-text-protocol/stdio-text-protocol.h"
#include "systime/systime.h"
#include "logging/log.h"
#include "device/device.h"

// Файлы проекта
#include "firmware.h"
#include "api.h"             // Если api.h лежит в корне проекта
#include "led-task.h"        // Если лежат в корне
#include "button-task.h"
#include "pi-task.h"

// Перечисление для номеров секундомеров
typedef enum {
    STOPWATCH_LOOP = 0,
    STOPWATCH_COMMAND = 1,
    STOPWATCH_LED = 2,
    STOPWATCH_BUTTON = 3,
    STOPWATCH_PI = 4,
} stopwatch_id_t;

int main()
{
    stdio_init_all();
    led_task_init();
    button_task_init();
    pi_task_init();
    stdio_text_protocol_init();
    profiling_init();
    
    // Настройка секундомеров
    profiling_stopwatch_init(STOPWATCH_LOOP, "loop");
    profiling_stopwatch_init(STOPWATCH_COMMAND, "command");
    profiling_stopwatch_init(STOPWATCH_LED, "led");
    profiling_stopwatch_init(STOPWATCH_BUTTON, "button");
    profiling_stopwatch_init(STOPWATCH_PI, "pi");

    // ИСПРАВЛЕНО: используем FIRMWARE_NAME из firmware.h
    LOG_INF("%s %s\n", FIRMWARE_NAME, FIRMWARE_VERSION);

    while (1)
    {
        profiling_start(STOPWATCH_LOOP);

        profiling_start(STOPWATCH_COMMAND);
        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL) api_handle(command);
        profiling_stop(STOPWATCH_COMMAND);

        profiling_start(STOPWATCH_LED);
        led_task_handle();
        profiling_stop(STOPWATCH_LED);

        profiling_start(STOPWATCH_BUTTON);
        button_task_handle();
        profiling_stop(STOPWATCH_BUTTON);

        profiling_start(STOPWATCH_PI);
        pi_task_handle();
        profiling_stop(STOPWATCH_PI);

        profiling_stop(STOPWATCH_LOOP);
    }
}