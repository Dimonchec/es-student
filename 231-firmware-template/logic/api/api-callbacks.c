#include "api/api-commands.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include "device/device.h"
#include "profiling/profiling.h"
#include "systime/systime.h"
#include "firmware.h"
#include "led-task/led-task.h"
#include "button-task/button-task.h"
#include "pi-task/pi-task.h"

static bool parse_u32(const char *text, uint32_t *value)
{
    if (!isdigit((unsigned char)text[0])) return false;
    char *end;
    errno = 0;
    unsigned long number = strtoul(text, &end, 10);
    if (*end != '\0' || errno == ERANGE || number > UINT32_MAX) return false;
    *value = (uint32_t)number;
    return true;
}

static void print_led_state(void)
{
    led_state_t state = led_task_get_state();
    uint32_t period = led_task_get_period_ms();
    const char *name = (state == LED_STATE_OFF) ? "off" :
                       (state == LED_STATE_ON) ? "on" : "blink";
    printf("led: %s, period %u ms\n", name, (unsigned)period);
}

static void cmd_help(const command_t *cmd) { (void)cmd; }

static void cmd_info(const command_t *cmd)
{
    (void)cmd;
    device_info_t info;
    device_get_info(&info, FIRMWARE_PROJECT, FIRMWARE_VERSION, FIRMWARE_REPO);
    
    // ПОРЯДОК СТРОГО ПО ПРИЛОЖЕНИЮ Б:
    printf("name: %s\n", FIRMWARE_NAME); // <--- ДОБАВЛЕНО ИМЯ ПРОШИВКИ
    printf("version: %s\n", info.version);
    printf("project: %s\n", info.project);
    printf("repo: %s\n", info.repo);
    printf("board: %s\n", info.board);
    printf("serial: %s\n", info.serial);
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", 
           (unsigned)info.manufacturer, (unsigned)info.part, (unsigned)info.revision);
    printf("pico-sdk: %s\n", info.sdk_version);
}

static void cmd_uptime(const command_t *cmd) { (void)cmd; printf("uptime: %llu ms\n", systime_ms()); }
static void cmd_led_enable(const command_t *cmd) { (void)cmd; led_task_set_state(LED_STATE_ON); print_led_state(); }
static void cmd_led_disable(const command_t *cmd) { (void)cmd; led_task_set_state(LED_STATE_OFF); print_led_state(); }
static void cmd_led_blink(const command_t *cmd) { (void)cmd; led_task_set_state(LED_STATE_BLINK); print_led_state(); }

static void cmd_led_period(const command_t *cmd)
{
    uint32_t period_ms;
    if (cmd->argc != 1 || !parse_u32(cmd->argv[0], &period_ms))
    {
        printf("error: usage led_period <period_ms>\n");
        return;
    }
    if (!led_task_set_period_ms(period_ms))
    {
        printf("error: period_ms must be greater than 0\n");
        return;
    }
    print_led_state();
}

static void cmd_button(const command_t *cmd)
{
    (void)cmd;
    bool pressed = button_task_is_pressed();
    uint32_t count = button_task_get_press_count();
    printf("button: %s, presses %u\n", pressed ? "pressed" : "released", (unsigned)count);
}

static void cmd_pi_start(const command_t *cmd)
{
    if (pi_task_is_busy()) { printf("error: pi is busy\n"); return; }

    uint32_t terms = 1000000;
    if (cmd->argc > 1) { printf("error: usage pi_start [terms]\n"); return; }

    if (cmd->argc == 1)
    {
        char *endptr;
        long val = strtol(cmd->argv[0], &endptr, 10);
        if (*endptr != '\0' || val <= 0) 
        { 
            if (*endptr == '\0' && val == 0) printf("error: terms must be greater than 0\n");
            else printf("error: usage pi_start [terms]\n"); 
            return; 
        }
        terms = (uint32_t)val;
    }
    pi_task_start(terms);
    printf("pi: started, %u terms\n", (unsigned)terms);
}

static void cmd_pi(const command_t *cmd)
{
    (void)cmd;
    if (pi_task_is_done()) 
        printf("pi: %.9f in %u ms\n", pi_task_get_result(), (unsigned)pi_task_get_time_ms());
    else if (pi_task_is_busy())
        printf("pi: running\n");
    else
        printf("pi: not started\n");
}

static void cmd_profiling(const command_t *cmd)
{
    (void)cmd;
#if PROFILING_ENABLED
    printf("profiling\n");
    for (uint32_t i = 0; i < PROFILING_MAX_STOPWATCHES; i++) {
        profiling_result_t res;
        if (profiling_get(i, &res)) {
            printf("%-8s count %-7u mean %u us max %u us\n", 
                   res.name, (unsigned)res.count, (unsigned)res.mean_us, (unsigned)res.max_us);
        }
    }
#else
    printf("profiling is disabled in this build\n");
#endif
}

static void cmd_profiling_reset(const command_t *cmd)
{
    (void)cmd;
    profiling_reset();
    printf("profiling reset\n");
}

// ТАБЛИЦА СТРОГО ПО ПРИЛОЖЕНИЮ Б (help первый)
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