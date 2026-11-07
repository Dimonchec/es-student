#include "stream-task/stream-task.h"
#include "platform/platform.h"
#include <stddef.h>

static stream_task_callback_t callback = NULL;
static stream_state_t state = STREAM_STATE_IDLE;
static uint8_t stream_bits = 16;
static uint32_t frame_number = 0;
static uint32_t total_frames = 0;
static uint32_t total_samples = 0;

void stream_task_init(stream_task_callback_t on_frame)
{
    callback = on_frame;
    state = STREAM_STATE_IDLE;
}

bool stream_task_start(uint8_t bits)
{
    if (state == STREAM_STATE_RUN) return false;
    if (bits != 16 && bits != 8) return false;
    if (!platform_stream_start(bits)) return false;

    stream_bits = bits;
    frame_number = 0;
    total_frames = 0;
    total_samples = 0;
    state = STREAM_STATE_RUN;
    return true;
}

void stream_task_stop(void)
{
    if (state != STREAM_STATE_RUN) return;
    platform_stream_stop();
    state = STREAM_STATE_IDLE;
}

void stream_task_handle(void)
{
    if (state != STREAM_STATE_RUN || callback == NULL) return;

    uint32_t samples;
    const uint8_t *data = platform_stream_next(&samples);
    if (data == NULL) return;

    uint32_t bytes = samples * 2; // 16 бит

    stream_header_t header;
    header.sync_word = STREAM_SYNC_WORD;
    header.number = frame_number;
    header.count = samples;

    callback(&header, data, bytes);
    
    frame_number++;
    total_frames++;
    total_samples += samples;
    
    platform_stream_release();
}

stream_state_t stream_task_get_state(void) { return state; }
uint8_t stream_task_get_bits(void) { return stream_bits; }
uint32_t stream_task_get_frames(void) { return total_frames; }
uint32_t stream_task_get_samples(void) { return total_samples; }
uint32_t stream_task_get_overruns(void) { return platform_stream_overruns(); }