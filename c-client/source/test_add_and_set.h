#ifndef test_add_and_set_h
#define test_add_and_set_h 1

#include "diffusion.h"

long test_add_and_set(
    SESSION_T *session,
    char *root_topic_path,
    long iterations,
    long total_topics,
    long total_unique_topic_values,
    LIST_T *topic_values
);

#endif