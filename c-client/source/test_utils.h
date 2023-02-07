#ifndef test_utils_h
#define test_utils_h 1

uint64_t get_epoch_time_ns();

void wait_for_counter_to_reach(
    uint32_t *counter,
    uint32_t value
);

void thread_init();
void thread_signal();
void thread_wait();
void thread_terminate();

#endif