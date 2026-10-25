#include <stdio.h>
#include <string.h> // <--- ДОБАВЛЕНО: для функции strcmp
#include "pico/stdlib.h"
#include "led.h"
#include "log.h"
#include "device.h"
#include "pico/unique_id.h"
#include "memory.h"
#include "command.h"
#include "clock.h"
#include "profiling.h"

#define LINE_SIZE 32

const uint BLINK_HALF_PERIOD_MS = 500;
uint64_t last_toggle_us = 0;

// прикидка: за член ряда 4 операции с double, 175 + 110 + 190 + 110 = 585 тактов;
// 1 000 000 членов по 585 тактов при 125 МГц — около 4,7 с
const uint CALC_PI_TERMS = 1000000;

void blink(void)
{
    uint64_t now_us = time_us_64();

    // Если прошло больше 500 мс (500 * 1000 микросекунд)
    if (now_us - last_toggle_us >= BLINK_HALF_PERIOD_MS * 1000)
    {
        last_toggle_us = now_us; // Запоминаем время переключения
        led_toggle();            // Переключаем светодиод
    }
}

char line[LINE_SIZE];
uint line_length = 0;   



// 1. Тип для указателя на функцию-обработчик (принимает void, возвращает void)
/*typedef void (*command_handler_t)(void);

// 3. Структура для хранения пары "имя команды" - "функция"
struct command_t
{
    const char *name;
    command_handler_t handler;
};*/


// --- ДОБАВЛЕНО: Прототип функции, чтобы read_line знала о её существовании ---
void handle_command(const char *command);

double calc_pi(uint terms)
{
    double sum = 0.0;
    double sign = 1.0;

    for (uint k = 0; k < terms; k++)
    {
        // Добавляем к сумме sign / (2k + 1)
        sum += sign / (2 * k + 1);
        // Меняем знак
        sign = -sign;
    }

    return sum * 4;
}

volatile double pi_result;

void cmd_calc_pi(void)
{
    uint64_t start_us = time_us_64();
    pi_result = calc_pi(CALC_PI_TERMS);
    uint64_t spent_us = time_us_64() - start_us;

    printf("pi: %.8f\n", pi_result);
    printf("time: %llu ms\n", spent_us / 1000);
}

void cmd_info(void)
{
    device_info();
}

void cmd_version(void)
{
    log_version();
}

void cmd_mem_info(void)
{
    mem_info();
}

void cmd_dev_info(void)
{
    dev_info();
}

// 5. Добавляем новую команду ping
void cmd_ping(void)
{
    printf("pong\n");
}

void cmd_fw_info(void)
{
    fw_info(); // Вызов функции из memory.c
}

void cmd_boot_info(void)
{
    boot_info();
}
void cmd_clk_info(void)
{
    clk_info();
}
void cmd_uptime(void)
{
    uptime();
}
void cmd_main_time_exec(void)
{
    printf("iteration avg %.2f us, max %u us\n", profiling_avg_us(), (unsigned)profiling_max_us());
}

void cmd_main_time_reset(void)
{
    profiling_reset_max();
    printf("max reset\n");
}

// 3. Собираем таблицу команд
const struct command_t commands[] = {
    { "info",    cmd_info    },
    { "version", cmd_version },
    { "ping",    cmd_ping    },
    { "mem_info", cmd_mem_info }, 
    { "fw_info", cmd_fw_info }, 
    { "dev_info", cmd_dev_info },
    { "boot_info", cmd_boot_info },
    { "clk_info", cmd_clk_info },
    { "uptime", cmd_uptime },
    { "calc_pi", cmd_calc_pi },
    { "main_time_exec", cmd_main_time_exec },
    { "main_time_reset", cmd_main_time_reset },
};

const uint command_count = sizeof(commands) / sizeof(commands[0]);




// ------------------------------------------------------------------------------



void read_line(void)
{
    int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return;
    }

    // Если нажали Enter (\r или \n)
    if (symbol == '\r' || symbol == '\n')
    {
        putchar('\n'); // Переводим курсор в терминале на новую строку
        line[line_length] = '\0'; // Завершаем строку нулем

        if (line_length > 0)
        {
            LOG_DBG("got %s\n", line);
            handle_command(line); // Теперь компилятор знает, что это за функция
        }

        line_length = 0; // Сбрасываем длину для следующей команды
        return;
    }

    // Если символ обычный и буфер не переполнен
    if (line_length + 1 < LINE_SIZE)
    {
        line[line_length] = (char)symbol;
        line_length = line_length + 1;
        putchar(symbol); // Эхо-вывод: показываем символ, который ввели
    }
}

// --- Тело функции handle_command ---
// 4. Заменяем цепочку поиском по таблице
void handle_command(const char *command)
{
    for (uint i = 0; i < command_count; i++)
    {
        if (strcmp(command, commands[i].name) == 0)
        {
            // ВАЖНО: Проверка на NULL (как сказано в красной рамке)
            if (commands[i].handler != NULL)
            {
                commands[i].handler(); // Вызов функции по указателю
            }
            return; // Команда найдена и выполнена, выходим из функции
        }
    }

    // Сюда доходим, только если команда не найдена в цикле
    LOG_ERR("unknown command: %s\n", command);
}

int main()
{
    stdio_init_all();
    led_init();

    profiling_init(); // <--- Добавить перед циклом
    
    while (1)
    {
        profiling_iteration(); // <--- Добавить первой строкой в цикле
        blink();    // Проверяем, не пора ли переключить светодиод
        read_line(); // Читаем команды из терминала
    }

    
}