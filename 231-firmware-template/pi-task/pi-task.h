#pragma once
#include <stdint.h>
#include <stdbool.h>

void pi_task_init(void);
void pi_task_handle(void);
void pi_task_start(uint32_t terms);
bool pi_task_is_done(void);
double pi_task_get_result(void);
uint32_t pi_task_get_terms(void);