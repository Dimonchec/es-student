#include "stdio-text-protocol.h"
#include "pico/stdlib.h"
#include <stdio.h>

static char buffer[STDIO_TEXT_PROTOCOL_LINE_SIZE];
static uint32_t length = 0;
static command_t current_command;

void stdio_text_protocol_init(void)
{
    length = 0;
}

// Разбивает строку на слова на месте, возвращает количество слов
static uint32_t split_words(uint32_t len)
{
    uint32_t words = 0;
    bool in_word = false;

    for (uint32_t i = 0; i <= len; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\0')
        {
            if (in_word)
            {
                buffer[i] = '\0';
                in_word = false;
            }
        }
        else if (!in_word)
        {
            in_word = true;
            words++;
        }
    }
    return words;
}

const command_t *stdio_text_protocol_handle(void)
{
    int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return NULL;
    }

    // Backspace (0x08 или 0x7f): убираем последний символ и стираем его на экране
    if (symbol == 0x08 || symbol == 0x7f)
    {
        if (length > 0)
        {
            length--;
            printf("\b \b");
        }
        return NULL;
    }

    // Конец строки (CR или LF)
    if (symbol == '\r' || symbol == '\n')
    {
        printf("\n");
        buffer[length] = '\0';

        if (length == 0)
        {
            length = 0;
            return NULL; // Пустой кадр игнорируется
        }

        // Сбрасываем флаг для новой строки
        current_command.truncated = false;

        // Считаем слова
        uint32_t words = split_words(length);

        if (words == 0)
        {
            length = 0;
            return NULL;
        }

        // Заполняем структуру
        current_command.name = &buffer[0];
        current_command.argc = (words > 1) ? (words - 1) : 0;

        // Восстанавливаем указатели на аргументы
        uint32_t word_idx = 0;
        bool in_word = false;
        for (uint32_t i = 0; i <= length; i++)
        {
            if (buffer[i] != '\0')
            {
                if (!in_word)
                {
                    in_word = true;
                    if (word_idx > 0 && (word_idx - 1) < STDIO_TEXT_PROTOCOL_MAX_ARGS)
                    {
                        current_command.argv[word_idx - 1] = &buffer[i];
                    }
                    word_idx++;
                }
            }
            else
            {
                in_word = false;
            }
        }

        // Проверка на переполнение аргументов
        if (current_command.argc > STDIO_TEXT_PROTOCOL_MAX_ARGS)
        {
            current_command.truncated = true;
        }

        length = 0;
        return &current_command;
    }

    // Обычный символ
    if (length + 1 < STDIO_TEXT_PROTOCOL_LINE_SIZE)
    {
        buffer[length++] = (char)symbol;
        putchar(symbol); // Эхо
    }
    else
    {
        // Буфер переполнен — помечаем как truncated
        current_command.truncated = true;
    }

    return NULL;
}