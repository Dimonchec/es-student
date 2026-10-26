#include "api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "device.h"
#include "profiling.h"
#include "pi-task.h" // <--- ДОБАВЛЕНО: для работы с задачей расчета пи

// Прототипы обработчиков
void cmd_info(const command_t *cmd);
void cmd_uptime(const command_t *cmd);
void cmd_main_time_exec(const command_t *cmd);
void cmd_main_time_reset(const command_t *cmd);
void cmd_pi_start(const command_t *cmd); // <--- ДОБАВЛЕНО
void cmd_pi(const command_t *cmd);       // <--- ДОБАВЛЕНО

typedef struct {
    const char *name;
    void (*callback)(const command_t *cmd);
    const char *help;
} api_command_t;

// Таблица команд (calc_pi заменена на pi_start и pi)
const api_command_t api_commands[] = {
    { "info", cmd_info, "device passport" },
    { "uptime", cmd_uptime, "time since reset" },
    { "main_time_exec", cmd_main_time_exec, "superloop iteration: average and maximum" },
    { "main_time_reset", cmd_main_time_reset, "reset maximum iteration" },
    { "pi_start", cmd_pi_start, "start pi calculation: pi_start [terms]" },
    { "pi", cmd_pi, "show pi result or state" },
};

void api_handle(const command_t *command)
{
    // 1. Проверка на переполнение строки/аргументов
    if (command->truncated)
    {
        printf("error: command longer than 63 characters or 4 arguments\n");
        return;
    }

    // 2. Обработка help
    if (strcmp(command->name, "help") == 0)
    {
        printf("help\t\tlist of commands\n");
        for (size_t i = 0; i < sizeof(api_commands) / sizeof(api_commands[0]); i++)
        {
            printf("%-15s\t%s\n", api_commands[i].name, api_commands[i].help);
        }
        return;
    }

    // 3. Поиск команды в таблице
    for (size_t i = 0; i < sizeof(api_commands) / sizeof(api_commands[0]); i++)
    {
        if (strcmp(command->name, api_commands[i].name) == 0)
        {
            // ВАЖНО: используем поле callback
            if (api_commands[i].callback != NULL)
            {
                api_commands[i].callback(command);
            }
            return;
        }
    }

    // 4. Неизвестная команда
    printf("error: unknown command '%s', try help\n", command->name);
}

// --- РЕАЛИЗАЦИЯ ОБРАБОТЧИКОВ ---

void cmd_info(const command_t *cmd)
{
    (void)cmd;
    device_info();
}

void cmd_uptime(const command_t *cmd)
{
    (void)cmd;
    printf("uptime: %llu ms\n", time_us_64() / 1000);
}

void cmd_main_time_exec(const command_t *cmd)
{
    (void)cmd;
    printf("iteration avg %.2f us, max %u us\n", profiling_avg_us(), (unsigned)profiling_max_us());
}

void cmd_main_time_reset(const command_t *cmd)
{
    (void)cmd;
    profiling_reset_max();
    printf("max reset\n");
}

// --- НОВЫЕ ОБРАБОТЧИКИ ДЛЯ РАСЧЕТА ПИ ---

void cmd_pi_start(const command_t *cmd)
{
    // Если расчет уже идет, сообщаем об ошибке
    if (pi_task_get_state() == PI_TASK_RUNNING)
    {
        printf("error: pi is busy\n");
        return;
    }

    uint32_t terms = 1000000; // Значение по умолчанию
    if (cmd->argc > 0)
    {
        char *endptr;
        long val = strtol(cmd->argv[0], &endptr, 10);
        if (*endptr != '\0' || val <= 0)
        {
            printf("error: usage pi_start [terms]\n");
            return;
        }
        terms = (uint32_t)val;
    }

    pi_task_start(terms);
    printf("pi: started, %u terms\n", (unsigned)terms);
}

void cmd_pi(const command_t *cmd)
{
    (void)cmd;
    pi_task_state_t state = pi_task_get_state();

    if (state == PI_TASK_NOT_STARTED)
    {
        printf("pi: not started\n");
    }
    else if (state == PI_TASK_RUNNING)
    {
        printf("pi: running\n");
    }
    else
    {
        printf("pi: %.9f\n", pi_task_get_result());
    }
}