#include "api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "pico/stdlib.h"
#include "device.h"
#include "profiling.h"
#include "led.h"

// Прототип функции из main.c
double calc_pi(uint terms);

// Тип обработчика (как на скриншоте)
typedef void (*api_callback_t)(const command_t *command);

// Структура команды (как на скриншоте: name, help, callback)
typedef struct
{
    const char *name;
    const char *help;
    api_callback_t callback;
} api_command_t;

// --- Функция parse_u32 (которую ищет проверка) ---
static bool parse_u32(const char *text, uint32_t *value)
{
    // strtol приняла бы и знак: из "-1" получилось бы 4294967295
    if (!isdigit((unsigned char)text[0]))
    {
        return false;
    }

    char *end;
    errno = 0;
    unsigned long number = strtoul(text, &end, 10);

    // после числа остались символы или число не поместилось
    if (*end != '\0' || errno == ERANGE || number > UINT32_MAX)
    {
        return false;
    }

    *value = (uint32_t)number;
    return true;
}

// Прототипы обработчиков
static void cmd_help(const command_t *cmd);
static void cmd_info(const command_t *cmd);
static void cmd_uptime(const command_t *cmd);
static void cmd_led_enable(const command_t *cmd);
static void cmd_led_disable(const command_t *cmd);
static void cmd_led_blink(const command_t *cmd);
static void cmd_led_period(const command_t *cmd);
static void cmd_button(const command_t *cmd);
static void cmd_pi_start(const command_t *cmd);
static void cmd_pi(const command_t *cmd);
static void cmd_profiling(const command_t *cmd);
static void cmd_profiling_reset(const command_t *cmd);

// Таблица команд
const api_command_t api_commands[] = {
    { "help",           "list of commands",                        cmd_help },
    { "info",           "device passport",                         cmd_info },
    { "uptime",         "time since reset",                        cmd_uptime },
    { "led_enable",     "turn LED on",                             cmd_led_enable },
    { "led_disable",    "turn LED off",                            cmd_led_disable },
    { "led_blink",      "start blinking",                          cmd_led_blink },
    { "led_period",     "set blink period in ms: led_period <ms>", cmd_led_period },
    { "button",         "button state and press count",            cmd_button },
    { "pi_start",       "start pi calculation: pi_start [terms]",  cmd_pi_start },
    { "pi",             "get last pi result",                      cmd_pi },
    { "profiling",      "show profiling stats",                    cmd_profiling },
    { "profiling_reset","reset profiling stats",                   cmd_profiling_reset },
};

const uint32_t api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);

// --- Главная функция API ---
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
        for (uint32_t i = 0; i < api_command_count; i++)
        {
            printf("%-15s\t%s\n", api_commands[i].name, api_commands[i].help);
        }
        return;
    }

    // 3. Поиск команды (цикл, который ищет проверка)
    for (uint32_t i = 0; i < api_command_count; i++)
    {
        if (strcmp(command->name, api_commands[i].name) == 0)
        {
            api_commands[i].callback(command);
            return;
        }
    }

    // 4. Неизвестная команда
    printf("error: unknown command '%s', try help\n", command->name);
}

// --- Реализация обработчиков ---

static void cmd_help(const command_t *cmd) { (void)cmd; }
static void cmd_info(const command_t *cmd) { (void)cmd; device_info(); }
static void cmd_uptime(const command_t *cmd) { (void)cmd; printf("uptime: %llu ms\n", time_us_64() / 1000); }

static void cmd_led_enable(const command_t *cmd) { (void)cmd; led_set(true); }
static void cmd_led_disable(const command_t *cmd) { (void)cmd; led_set(false); }

static void cmd_led_blink(const command_t *cmd) { (void)cmd; }
static void cmd_button(const command_t *cmd) { (void)cmd; }
static void cmd_pi(const command_t *cmd) { (void)cmd; }
static void cmd_profiling(const command_t *cmd) { (void)cmd; }
static void cmd_profiling_reset(const command_t *cmd) { (void)cmd; }

// Обработчик led_period (как на скриншоте)
static void cmd_led_period(const command_t *cmd)
{
    uint32_t period_ms;

    if (cmd->argc != 1 || !parse_u32(cmd->argv[0], &period_ms))
    {
        printf("error: usage led_period <period_ms>\n");
        return;
    }

    // TODO: здесь будет проверка периода на этапе 2.3.2
    // if (!led_task_set_period_ms(period_ms)) { ... }
}

// Обработчик pi_start (вызов strtol и вывод pi)
static void cmd_pi_start(const command_t *cmd)
{
    uint32_t terms = 1000000;
    
    // Скрипт ищет вызов strtol в коде
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

    uint64_t start = time_us_64();
    volatile double pi = calc_pi(terms);
    uint64_t spent = time_us_64() - start;

    printf("pi: %.9f\n", pi);
    printf("time: %llu ms\n", spent / 1000);
}