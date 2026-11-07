#include "block-task/block-task.h"
#include <stddef.h>
#include "platform/platform.h"

static block_task_callback_t callback = NULL;
static uint32_t expected_count = 0;

bool block_task_start(uint32_t count, block_task_callback_t on_block)
{
    if (!platform_block_start(count)) return false;
    callback = on_block;
    expected_count = count;
    return true;
}

void block_task_handle(void)
{
    if (callback == NULL) return;
    if (!platform_block_done()) return;

    block_task_callback_t cb = callback;
    callback = NULL; // забываем до вызова, чтобы можно было запустить новый захват
    
    cb(platform_block_samples(), expected_count, platform_block_time_us());
}