/**
 * @file activity-history.h
 *
 * Structures and functions to define and manage the activity history
 * in the context of delta calculations.
 *
 * Copyright © 2021 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */
#ifndef _diffusion_activity_history_h_
#define _diffusion_activity_history_h_ 1

#include <stdlib.h>
#include <stdint.h>

#define DIFFUSION_DELTA_BUCKET_COUNT 16
#define DIFFUSION_DELTA_BUCKET_MILLIS 1024L


typedef enum {
        DIFFUSION_DELTA_QUALITY_HIGHEST = 0,
        DIFFUSION_DELTA_QUALITY_HIGH = 1,
        DIFFUSION_DELTA_QUALITY_MODERATE = 2,
        DIFFUSION_DELTA_QUALITY_LOW = 3,
        DIFFUSION_DELTA_QUALITY_LOWEST = 4
} DIFFUSION_DELTA_QUALITY_T;

/**
 * Opaque structure for activity history
 */
typedef struct DIFFUSION_ACTIVITY_HISTORY_T DIFFUSION_ACTIVITY_HISTORY_T;

/**
 * @brief create a new activity history
 *
 * @param bucket_count
 * @param bucket_ms
 * @return a new activity history
 */
DIFFUSION_ACTIVITY_HISTORY_T *diffusion_activity_history_create(
        int bucket_count,
        uint64_t bucket_ms);


/**
 * @brief returns the utilisation of the activity history passed as a parameter
 *
 * @param activity_history the activity history
 * @return a value between 0 (not busy) and 256 (completely busy)
 */
int diffusion_activity_history_utilisation(
        DIFFUSION_ACTIVITY_HISTORY_T *activity_history);


/**
 * @brief start an activity period.
 *
 * @param activity_history the activity history
 */
void diffusion_activity_history_start(
        DIFFUSION_ACTIVITY_HISTORY_T *activity_history);


/**
 * @brief end the current activity period.
 *
 * @param activity_history the activity_history
 */
void diffusion_activity_history_stop(
        DIFFUSION_ACTIVITY_HISTORY_T *activity_history);


/**
 * @brief free a memory allocated DIFFUSION_ACTIVITY_HISTORY_T struct
 *
 * @param activity_history the activity history
 */
void diffusion_activity_history_free(
        DIFFUSION_ACTIVITY_HISTORY_T *activity_history);




#endif
