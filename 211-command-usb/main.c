#include <stdio.h>
#include <string.h> // <--- ДОБАВЛЕНО: для функции strcmp
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "led.h"
#include "log.h"
#include "device.h"
#include "pico/unique_id.h"

#define LINE_SIZE 32

char line[LINE_SIZE];
uint line_length = 0;   

const uint BUTTON_PIN = 15;
const uint DEBOUNCE_MS = 20;

// --- ДОБАВЛЕНО: Прототип функции, чтобы read_line знала о её существовании ---
void handle_command(const char *command);
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
void handle_command(const char *command)
{
    if (strcmp(command, "enable") == 0)
    {
        led_set(true);
        LOG_INF("led %s\n", led_is_on() ? "on" : "off");
    }
    else if (strcmp(command, "disable") == 0)
    {
        led_set(false);
        LOG_INF("led %s\n", led_is_on() ? "on" : "off");
    }
    else if (strcmp(command, "info") == 0)
    {
        device_info();
    }
    else if (strcmp(command, "version") == 0)
    {
        log_version();
    }
    else
    {
        LOG_ERR("unknown command: %s\n", command);
    }
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