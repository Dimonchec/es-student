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

const command_t *stdio_text_protocol_handle(void)
{
    int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return NULL;
    }

    if (symbol == 0x08 || symbol == 0x7f)
    {
        if (length > 0)
        {
            length--;
            printf("\b \b");
        }
        return NULL;
    }

    if (symbol == '\r' || symbol == '\n')
    {
        printf("\n");
        buffer[length] = '\0';

        if (length == 0)
        {
            length = 0;
            return NULL;
        }

        current_command.truncated = false;
        current_command.argc = 0;
        int arg_idx = 0;
        bool in_word = false;

        for (uint32_t i = 0; i <= length; i++)
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
                if (arg_idx == 0)
                {
                    current_command.name = &buffer[i];
                }
                else
                {
                    if (arg_idx - 1 < STDIO_TEXT_PROTOCOL_MAX_ARGS)
                    {
                        current_command.argv[arg_idx - 1] = &buffer[i];
                    }
                    else
                    {
                        current_command.truncated = true;
                    }
                }
                arg_idx++;
            }
        }
        current_command.argc = (arg_idx > 0) ? (arg_idx - 1) : 0;

        if (current_command.argc > STDIO_TEXT_PROTOCOL_MAX_ARGS)
        {
            current_command.truncated = true;
        }

        length = 0;
        return &current_command;
    }

    if (length + 1 < STDIO_TEXT_PROTOCOL_LINE_SIZE)
    {
        buffer[length++] = (char)symbol;
        putchar(symbol);
    }
    else
    {
        current_command.truncated = true;
    }

    return NULL;
}