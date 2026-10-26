#include <stdio.h>
#include "pico/stdlib.h"
#include "led.h"
#include "api.h"
#include "stdio-text-protocol.h"

// Прототип функции для api.c
double calc_pi(uint terms);

const uint BLINK_HALF_PERIOD_MS = 500;
uint64_t last_toggle_us = 0;

void blink(void)
{
    uint64_t now_us = time_us_64();
    if (now_us - last_toggle_us >= BLINK_HALF_PERIOD_MS * 1000)
    {
        last_toggle_us = now_us;
        led_toggle();
    }
}

double calc_pi(uint terms)
{
    double sum = 0.0;
    double sign = 1.0;
    for (uint k = 0; k < terms; k++)
    {
        sum += sign / (2 * k + 1);
        sign = -sign;
    }
    return sum * 4;
}

int main()
{
    stdio_init_all();
    led_init();
    stdio_text_protocol_init();

    while (1)
    {
        blink();

        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL)
        {
            api_handle(command);
        }
    }
}