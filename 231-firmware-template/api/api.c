#include "api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "device.h"
#include "profiling.h"
#include "led.h"

// Прототип функции из main.c
double calc_pi(uint terms);

// Структура таблицы API с полями name, help и callback
typedef struct {
    const char *name;
    void (*callback)(const command_t *cmd);
    const char *help;
} api_command_t;

// Прототипы обработчиков
void cmd_help(const command_t *cmd);
void cmd_info(const command_t *cmd);
void cmd_uptime(const command_t *cmd);
void cmd_led_enable(const command_t *cmd);
void cmd_led_disable(const command_t *cmd);
void cmd_led_blink(const command_t *cmd);
void cmd_led_period(const command_t *cmd);
void cmd_button(const command_t *cmd);
void cmd_pi_start(const command_t *cmd);
void cmd_pi(const command_t *cmd);
void cmd_profiling(const command_t *cmd);
void cmd_profiling_reset(const command_t *cmd);

// Таблица команд
const api_command_t api_commands[] = {
    { "help",           cmd_help,           "list of commands" },
    { "info",           cmd_info,           "device passport" },
    { "uptime",         cmd_uptime,         "time since reset" },
    { "led_enable",     cmd_led_enable,     "turn LED on" },
    { "led_disable",    cmd_led_disable,    "turn LED off" },
    { "led_blink",      cmd_led_blink,      "start blinking" },
    { "led_period",     cmd_led_period,     "set blink period in ms: led_period <ms>" },
    { "button",         cmd_button,         "button state and press count" },
    { "pi_start",       cmd_pi_start,       "start pi calculation: pi_start [terms]" },
    { "pi",             cmd_pi,             "get last pi result" },
    { "profiling",      cmd_profiling,      "show profiling stats" },
    { "profiling_reset",cmd_profiling_reset,"reset profiling stats" },
};

void api_handle(const command_t *command)
{
    // 1. Проверка на переполнение
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

    // 3. Поиск команды
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

    // 4. Неизвестная команда
    printf("error: unknown command '%s', try help\n", command->name);
}

// --- Реализация обработчиков ---

void cmd_help(const command_t *cmd)
{
    (void)cmd; // help обрабатывается в api_handle
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

void cmd_led_enable(const command_t *cmd) { (void)cmd; led_set(true); }
void cmd_led_disable(const command_t *cmd) { (void)cmd; led_set(false); }
void cmd_led_blink(const command_t *cmd) { (void)cmd; /* заглушка на этап 2.3.2 */ }
void cmd_led_period(const command_t *cmd) { (void)cmd; /* заглушка на этап 2.3.2 */ }
void cmd_button(const command_t *cmd) { (void)cmd; /* заглушка на этап 2.3.3 */ }
void cmd_pi_start(const command_t *cmd) { (void)cmd; /* заглушка на этап 2.3.4 */ }
void cmd_pi(const command_t *cmd) { (void)cmd; /* заглушка на этап 2.3.4 */ }
void cmd_profiling(const command_t *cmd) { (void)cmd; /* заглушка на этап 2.3.5 */ }
void cmd_profiling_reset(const command_t *cmd) { (void)cmd; /* заглушка на этап 2.3.5 */ }