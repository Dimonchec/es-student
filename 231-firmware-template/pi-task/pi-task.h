#pragma once
#include <stdint.h>
#include <stdbool.h>

// Состояния задачи
typedef enum {
    PI_TASK_NOT_STARTED,
    PI_TASK_RUNNING,
    PI_TASK_DONE
} pi_task_state_t;

// Функции задачи
void pi_task_start(uint32_t terms);
bool pi_task_handle(void); // true, если задача завершилась на этом вызове
double pi_task_get_result(void);
pi_task_state_t pi_task_get_state(void);