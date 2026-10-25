#pragma once

#include "pico/types.h"

typedef void (*command_handler_t)(void);

struct command_t
{
    const char *name;
    command_handler_t handler;
};

// extern говорит, что переменные определены в другом файле (в main.c)
extern const struct command_t commands[];
extern const uint command_count;