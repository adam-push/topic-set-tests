#ifndef _diffusion_retry_strategy_h_
#define _diffusion_retry_strategy_h_ 1

/*
 * Copyright © 2022 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */

/**
 * @file retry-strategy.h
 *
 * Functions relating to initial session establishment retry strategy.
 */

#include <stdlib.h>

#include "diffusion-api-error.h"
#include "utils.h"


/**
 * An opaque diffusion session establishment retry strategy struct.
 */
typedef struct DIFFUSION_RETRY_STRATEGY_T DIFFUSION_RETRY_STRATEGY_T;


/**
 * @brief Creates a new retry strategy.
 *
 * @param interval  the number of milliseconds before the first retry and between subsequent retries.
 * @param attempts  the number of retry attempts.
 * @param api_error populated on API error. Can be NULL.
 *
 * @return  The new retry strategy if the operation was successful, NULL otherwise.
 *          In this case, if a non-NULL <code>api_error</code> pointer has been provided,
 *          this will be populated with the error information and should be freed with
 *          <code>diffusion_api_error_free</code>.
 */
DIFFUSION_RETRY_STRATEGY_T *diffusion_retry_strategy_create(
        uint32_t interval,
        uint32_t attempts,
        DIFFUSION_API_ERROR *api_error);


/**
 * @brief Creates a new retry strategy that indicates that no retry is to be attempted.
 *
 * @return The new retry strategy.
 */
DIFFUSION_RETRY_STRATEGY_T *diffusion_retry_strategy_no_retry();

/**
 * @brief Creates a new retry strategy that will retry indefinitely at the specified interval.
 *
 * @param interval  the number of milliseconds before the first retry and between subsequent retries.
 * @param api_error populated on API error. Can be NULL.
 *
 * @return  The new retry strategy if the operation was successful, NULL otherwise.
 *          In this case, if a non-NULL <code>api_error</code> pointer has been provided,
 *          this will be populated with the error information and should be freed with
 *          <code>diffusion_api_error_free</code>.
 */
DIFFUSION_RETRY_STRATEGY_T *diffusion_retry_strategy_create_interval_only(
        uint32_t interval,
        DIFFUSION_API_ERROR *api_error);


/**
 * @brief Free a retry strategy.
 *
 * @param retry_strategy    the retry strategy to be freed.
 */
void diffusion_retry_strategy_free(
        DIFFUSION_RETRY_STRATEGY_T *retry_strategy);


/**
 * @brief Returns the number of milliseconds between retries.
 *
 * @param retry_strategy    the retry strategy.
 *
 * @return The number of milliseconds between retries.
 */
uint32_t diffusion_retry_strategy_get_interval(
        DIFFUSION_RETRY_STRATEGY_T *retry_strategy);


/**
 * @brief Returns the maximum number of retries to attempt.
 *
 * @param retry_strategy    the retry strategy.
 *
 * @return The maximum number of retries to attempt.
 */
uint32_t diffusion_retry_strategy_get_attempts(
        DIFFUSION_RETRY_STRATEGY_T *retry_strategy);



#endif
