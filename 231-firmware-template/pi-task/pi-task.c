#include "pi-task.h"
#include "pico/stdlib.h"

// Прикидка: при 125 МГц 0.5 мс = 62500 тактов. Цена члена ряда ~585 тактов.
// 62500 / 585 ≈ 106. Округляем до 100 для запаса.
#define PI_TERMS_PER_CALL 100

static pi_task_state_t state = PI_TASK_NOT_STARTED;
static uint32_t k = 0;           // Номер текущего члена
static uint32_t total_terms = 0; // Сколько всего надо посчитать
static double sum = 0.0;         // Текущая сумма
static double sign = 1.0;        // Знак текущего члена
static double result = 0.0;      // Итоговый результат
static uint64_t start_time = 0;  // Время начала
static uint64_t end_time = 0;    // Время окончания

void pi_task_start(uint32_t terms)
{
    if (terms == 0) terms = 1;
    state = PI_TASK_RUNNING;
    k = 0;
    total_terms = terms;
    sum = 0.0;
    sign = 1.0;
    start_time = time_us_64();
}

bool pi_task_handle(void)
{
    if (state != PI_TASK_RUNNING) return false;

    // Выполняем порцию
    uint32_t end_k = k + PI_TERMS_PER_CALL;
    // Защита от переполнения (как в задании)
    if (end_k < k) end_k = total_terms; 
    if (end_k > total_terms) end_k = total_terms;

    for (; k < end_k; k++)
    {
        sum += sign / (2.0 * k + 1.0);
        sign = -sign;
    }

    // Если достигли конца
    if (k == total_terms)
    {
        result = sum * 4.0;
        end_time = time_us_64();
        state = PI_TASK_DONE;
        return true; // Сообщаем, что задача завершилась
    }

    return false;
}

double pi_task_get_result(void)
{
    return result;
}

pi_task_state_t pi_task_get_state(void)
{
    return state;
}