#include "pi-task.h"
#include "pico/stdlib.h"

static double sum = 0.0;
static double sign = 1.0;
static uint32_t terms_total = 0;
static uint32_t current_term = 0;
static bool running = false;
static bool done = false;
static double result = 0.0;

#define CHUNK_SIZE 1000

void pi_task_init(void)
{
    running = false;
    done = false;
}

void pi_task_start(uint32_t terms)
{
    if (running) return; // Не начинаем новый расчет, если текущий не закончен
    terms_total = terms;
    current_term = 0;
    sum = 0.0;
    sign = 1.0;
    running = true;
    done = false;
    result = 0.0;
}

void pi_task_handle(void)
{
    if (!running) return;

    uint32_t end = current_term + CHUNK_SIZE;
    if (end > terms_total) end = terms_total;

    for (uint32_t k = current_term; k < end; k++)
    {
        sum += sign / (2 * k + 1);
        sign = -sign;
    }
    current_term = end;

    if (current_term >= terms_total)
    {
        result = sum * 4;
        running = false;
        done = true;
    }
}

bool pi_task_is_done(void) { return done; }
double pi_task_get_result(void) { return result; }
uint32_t pi_task_get_terms(void) { return terms_total; }