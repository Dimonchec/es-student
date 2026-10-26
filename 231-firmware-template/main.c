#include <stdio.h>
#include "pico/stdlib.h"
#include "led.h"
#include "api.h"
#include "stdio-text-protocol.h"
#include "led-task.h"

// Прототип функции для api.c
double calc_pi(uint terms);


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
     led_task_init();
    stdio_text_protocol_init();

    while (1)
    {
        led_task_handle();
        
        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL)
        {
            api_handle(command);
        }
    }
}