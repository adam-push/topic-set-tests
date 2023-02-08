#ifndef test_utils_h
#define test_utils_h 1

#include "diffusion.h"

uint64_t get_epoch_time_ns();

void wait_for_counter_to_reach(
    uint32_t *counter,
    uint32_t value
);

void clear_root_topic_path(
    SESSION_T *session,
    char *root_topic_path
);

void thread_init();
void thread_signal();
int thread_wait(long timeout_ms);
void thread_terminate();

#endif