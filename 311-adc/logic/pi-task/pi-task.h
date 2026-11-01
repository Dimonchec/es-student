#pragma once
#include <stdint.h>
#include <stdbool.h>

// Перечисляемый тип состояний (как требует проверка)
typedef enum {
    PI_STATE_IDLE,
    PI_STATE_CALCULATING,
    PI_STATE_DONE
} pi_state_t;

void pi_task_init(void);
void pi_task_handle(void);
void pi_task_start(uint32_t terms);
bool pi_task_is_done(void);
bool pi_task_is_busy(void); // <--- ДОБАВЛЕНО для проверки на занятость
double pi_task_get_result(void);
uint32_t pi_task_get_terms(void);
uint32_t pi_task_get_time_ms(void);