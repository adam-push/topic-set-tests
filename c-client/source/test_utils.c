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

#ifdef WIN32
        #define NOMINMAX
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
#endif

#include "test_utils.h"
#include "diffusion.h"

apr_pool_t *g_pool = NULL;
apr_thread_mutex_t *g_mutex;
apr_thread_cond_t *g_cond;


#ifdef WIN32
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


void thread_wait()
{
    apr_thread_mutex_lock(g_mutex);
    apr_thread_cond_wait(g_cond, g_mutex);
    apr_thread_mutex_unlock(g_mutex);
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
