#include "api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "device.h"
#include "profiling.h"

// Прототип функции из main.c
double calc_pi(uint terms);

typedef struct {
    const char *name;
    void (*callback)(const command_t *cmd);
    const char *help;
} api_command_t;

// Прототипы обработчиков
void cmd_info(const command_t *cmd);
void cmd_uptime(const command_t *cmd);
void cmd_calc_pi(const command_t *cmd);
void cmd_main_time_exec(const command_t *cmd);
void cmd_main_time_reset(const command_t *cmd);

const api_command_t api_commands[] = {
    { "info", cmd_info, "device passport" },
    { "uptime", cmd_uptime, "time since reset" },
    { "calc_pi", cmd_calc_pi, "calculate pi: calc_pi [terms]" },
    { "main_time_exec", cmd_main_time_exec, "superloop iteration: average and maximum" },
    { "main_time_reset", cmd_main_time_reset, "reset maximum iteration" },
};

void api_handle(const command_t *command)
{
    if (command->truncated)
    {
        printf("error: command longer than 63 characters or 4 arguments\n");
        return;
    }

    if (strcmp(command->name, "help") == 0)
    {
        printf("help\t\tlist of commands\n");
        for (size_t i = 0; i < sizeof(api_commands) / sizeof(api_commands[0]); i++)
        {
            printf("%-15s\t%s\n", api_commands[i].name, api_commands[i].help);
        }
        return;
    }

    for (size_t i = 0; i < sizeof(api_commands) / sizeof(api_commands[0]); i++)
    {
        if (strcmp(command->name, api_commands[i].name) == 0)
        {
            if (api_commands[i].callback != NULL)
            {
                api_commands[i].callback(command);
            }
            return;
        }
    }

    printf("error: unknown command '%s', try help\n", command->name);
}

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

void cmd_calc_pi(const command_t *cmd)
{
    uint32_t terms = 1000000;
    if (cmd->argc > 0)
    {
        char *endptr;
        long val = strtol(cmd->argv[0], &endptr, 10);
        if (*endptr != '\0' || val <= 0)
        {
            printf("error: usage calc_pi [terms]\n");
            return;
        }
        terms = (uint32_t)val;
    }

    uint64_t start = time_us_64();
    volatile double pi = calc_pi(terms);
    uint64_t spent = time_us_64() - start;

    printf("pi: %.9f\n", pi);
    printf("time: %llu ms\n", spent / 1000);
}