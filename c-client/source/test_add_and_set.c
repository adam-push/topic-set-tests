#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "test_add_and_set.h"
#include "test_utils.h"

uint32_t g_ready_count;

static int on_topic_update_add_and_set_increment_counter(
    DIFFUSION_TOPIC_CREATION_RESULT_T result,
    void *context)
{
        apr_atomic_inc32(&g_ready_count);
        return HANDLER_SUCCESS;
}


long test_add_and_set(
    SESSION_T *session,
    char *root_topic_path,
    long iterations,
    long total_topics,
    long total_unique_topic_values,
    LIST_T *topic_values)
{
    TOPIC_SPECIFICATION_T *topic_specification = topic_specification_init(TOPIC_TYPE_BINARY);
    uint64_t start_time = get_epoch_time_ns();
    apr_atomic_set32(&g_ready_count, 0);

    for (long i = 0; i < iterations; i++)
    {
        BUF_T *topic_value_buf = list_get_data_indexed(topic_values, i % total_unique_topic_values);

        long topic_index = i % total_topics;
        int chars_for_number = floor(log10(topic_index + 1)) + 1;
        char *topic_path = calloc(strlen(root_topic_path) + 1 + chars_for_number + 1, sizeof(char));
        sprintf(topic_path, "%s/%ld", root_topic_path, topic_index);

        DIFFUSION_TOPIC_UPDATE_ADD_AND_SET_PARAMS_T topic_update_params = {
                .topic_path = topic_path,
                .specification = topic_specification,
                .datatype = DATATYPE_BINARY,
                .update = topic_value_buf,
                .on_topic_update_add_and_set = on_topic_update_add_and_set_increment_counter
        };

        diffusion_topic_update_add_and_set(session, topic_update_params);
        free(topic_path);
    }

    wait_for_counter_to_reach(&g_ready_count, (uint32_t) iterations);

    uint64_t elapsed_time_ns = get_epoch_time_ns() - start_time;
    long elapsed_time_ms = elapsed_time_ns / 1000000.0;

    // cleanup
    clear_root_topic_path(session, root_topic_path);
    topic_specification_free(topic_specification);

    return elapsed_time_ms;
}
