#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include <apr.h>
#include <apr_atomic.h>
#include <apr_thread_cond.h>
#include <apr_thread_mutex.h>
#include <locale.h>

#include "test_utils.h"
#include "diffusion.h"


#ifdef WIN32
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #define CLOCK_REALTIME 0

    static int clock_gettime(int clock_id, struct timespec *spec) {
        __int64 wintime;
        GetSystemTimeAsFileTime((FILETIME*)&wintime);
        wintime -= 116444736000000000i64;               // 1 jan 1601 to 1 jan 1970
        spec->tv_sec = wintime / 10000000i64;           // seconds
        spec->tv_nsec = wintime % 10000000i64 * 100;    // nano-seconds
        return 0;
    }
#endif


apr_pool_t *g_pool = NULL;
apr_thread_mutex_t *g_mutex;
apr_thread_cond_t *g_cond;


void wait_for_counter_to_reach(uint32_t *counter, uint32_t value)
{
    while(true)
    {
        uint32_t current_count = apr_atomic_read32(counter);
        if (current_count == value) {
            break;
        }
        SLEEP_MS(1);
    }
}


static int on_clear_root_topic_path_removed(
    SESSION_T *session,
    const DIFFUSION_TOPIC_REMOVAL_RESULT_T *response,
    void *context)
{
    int removed_count = diffusion_topic_removal_result_removed_count(response);
    printf("Topic removal cleared %d topics.\n", removed_count);
    thread_signal();
    return HANDLER_SUCCESS;
}


static int on_clear_root_topic_path_error(
    SESSION_T *session,
    const DIFFUSION_ERROR_T *error)
{
    printf("Received error from Diffusion Server: %d:%s\n", error->code, error->message);
    return HANDLER_SUCCESS;
}


void clear_root_topic_path(
    SESSION_T *session,
    char *root_topic_path)
{
    thread_init();

    char *topic_selector = calloc(1 + strlen(root_topic_path) + 2, sizeof(char));
    sprintf(topic_selector, "*%s/", root_topic_path);
    printf("Clearing Root Topic Path: [%s]\n", topic_selector);

    TOPIC_REMOVAL_PARAMS_T remove_params = {
        .topic_selector = topic_selector,
        .on_removed = on_clear_root_topic_path_removed,
        .on_error = on_clear_root_topic_path_error
    };

    topic_removal(session, remove_params);
    if (thread_wait(5000) != HANDLER_SUCCESS) {
        printf("Topic removal of [%s] failed.\n", root_topic_path);
    }
    free(topic_selector);

    thread_terminate();
}


void thread_init()
{
    apr_initialize();
    apr_pool_create(&g_pool, NULL);

    apr_thread_mutex_create(&g_mutex, APR_THREAD_MUTEX_UNNESTED, g_pool);
    apr_thread_cond_create(&g_cond, g_pool);
}


void thread_signal()
{
    apr_thread_mutex_lock(g_mutex);
    apr_thread_cond_broadcast(g_cond);
    apr_thread_mutex_unlock(g_mutex);
}


int thread_wait(long timeout_ms)
{
    int rc = HANDLER_SUCCESS;
    apr_thread_mutex_lock(g_mutex);
    if (timeout_ms > 0) {
        rc = apr_thread_cond_timedwait(
            g_cond,
            g_mutex,
            (apr_interval_time_t) timeout_ms * 1000
        );
    }
    else {
        apr_thread_cond_wait(g_cond, g_mutex);
    }
    apr_thread_mutex_unlock(g_mutex);
    return rc;
}


void thread_terminate()
{
    apr_thread_cond_destroy(g_cond);
    apr_thread_mutex_destroy(g_mutex);
    apr_pool_destroy(g_pool);
    apr_terminate();
}


uint64_t get_epoch_time_ns()
{
        const int billion = 1 * 1000 * 1000 * 1000;
        struct timespec spec;

        clock_gettime(CLOCK_REALTIME, &spec);
        uint64_t epoch_ns = (uint64_t) spec.tv_sec * billion + (uint64_t) spec.tv_nsec;
        return (uint64_t) epoch_ns;
}
