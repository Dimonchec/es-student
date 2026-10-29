#include "pi-task.h"
#include "systime/systime.h"

// Константа за один вызов (как требует проверка)
#define PI_TERMS_PER_CALL 1000

static pi_state_t state = PI_STATE_IDLE;
static double sum = 0.0;
static double sign = 1.0;
static uint32_t terms_total = 0;
static uint32_t current_term = 0;
static double result = 0.0;

void pi_task_init(void)
{
    state = PI_STATE_IDLE;
}

void pi_task_start(uint32_t terms)
{
    if (state == PI_STATE_CALCULATING) return; // Защита от повторного запуска
    
    terms_total = terms;
    current_term = 0;
    sum = 0.0;
    sign = 1.0;
    result = 0.0;
    state = PI_STATE_CALCULATING;
}

void pi_task_handle(void)
{
    switch (state)
    {
        case PI_STATE_IDLE:
        case PI_STATE_DONE:
            // Ничего не делаем
            break;

        case PI_STATE_CALCULATING:
        {
            uint32_t end = current_term + PI_TERMS_PER_CALL;
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
                state = PI_STATE_DONE;
            }
            break;
        }
    }
}

bool pi_task_is_done(void) { return state == PI_STATE_DONE; }
bool pi_task_is_busy(void) { return state == PI_STATE_CALCULATING; }
double pi_task_get_result(void) { return result; }
uint32_t pi_task_get_terms(void) { return terms_total; }