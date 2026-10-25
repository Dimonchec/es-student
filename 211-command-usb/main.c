#include <stdio.h>
#include <string.h> // <--- ДОБАВЛЕНО: для функции strcmp
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "led.h"
#include "log.h"
#include "device.h"
#include "pico/unique_id.h"
#include "memory.h"
#include "command.h"

#define LINE_SIZE 32

char line[LINE_SIZE];
uint line_length = 0;   

const uint BUTTON_PIN = 15;
const uint DEBOUNCE_MS = 20;

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

// 2. Выносим действия команд в отдельные функции
void cmd_enable(void)
{
    led_set(true);
    LOG_INF("led %s\n", led_is_on() ? "on" : "off");
}

void cmd_disable(void)
{
    led_set(false);
    LOG_INF("led %s\n", led_is_on() ? "on" : "off");
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

// 3. Собираем таблицу команд
const struct command_t commands[] = {
    { "enable",  cmd_enable  },
    { "disable", cmd_disable },
    { "info",    cmd_info    },
    { "version", cmd_version },
    { "ping",    cmd_ping    },
    { "mem_info", cmd_mem_info }, 
    { "fw_info", cmd_fw_info }, 
    { "dev_info", cmd_dev_info },// <-- Добавили сюда
};

const uint command_count = sizeof(commands) / sizeof(commands[0]);




// ------------------------------------------------------------------------------

bool get_button_debounce(uint pin)
{
    bool state = gpio_get(pin);
    sleep_ms(DEBOUNCE_MS);
    return state && gpio_get(pin);
}

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

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    bool previous = false;

    while (1)
    {
        // 1. Обработка физической кнопки
        bool current = get_button_debounce(BUTTON_PIN);
        if (previous == true && current == false)
        {
            led_toggle();
            LOG_INF("led %s\n", led_is_on() ? "on" : "off");
        }
        previous = current;

        // 2. Обработка ввода с терминала
        read_line();
    }
}