#ifndef _diffusion_topic_selector_h_
#define _diffusion_topic_selector_h_ 1

/*
 * Copyright © 2021 - 2022 Push Technology Ltd., All Rights Reserved.
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
 * @file topic-selector.h
 *
 * A topic selector identifies one or more topics and are created from a pattern expression.
 *
 * @since 6.7
 */

/**
 * Opaque Topic Selector struct.
*/
typedef struct DIFFUSION_TOPIC_SELECTOR_T DIFFUSION_TOPIC_SELECTOR_T;


/**
 * @brief Return a DIFFUSION_TOPIC_SELECTOR_T struct for the given expression.
 *
 * @param expression    The expression to be used for evaluating topic paths.
 * @param error         Populated on API error. Can be NULL.
 *
 * @return              The topic selector struct for the given expression if the operation
 *                      was successful, NULL otherwise. In this case, if a non-NULL `error`
 *                      pointer has been provided, this will be populated with the error
 *                      information and should be freed with `diffusion_api_error_free`.
 *
 * @since 6.7
 */
DIFFUSION_TOPIC_SELECTOR_T *diffusion_topic_selector_create(
        char *expression,
        DIFFUSION_API_ERROR *error);


/**
 * @brief Evaluate a topic selector against a topic path.
 *
 * @param topic_selector    The topic selector.
 * @param topic_path        The topic path to evaluate against.
 *
 * @return                  true is the topic selector selects the topic path,
 *                          false otherwise.
 *
 * @since 6.7
 */
bool diffusion_topic_selector_selects(
        DIFFUSION_TOPIC_SELECTOR_T *topic_selector,
        char *topic_path);


/**
 * @brief Free a topic selector
 *
 * @param topic_selector    The topic selector.
 *
 * @since 6.7
 */
void diffusion_topic_selector_free(DIFFUSION_TOPIC_SELECTOR_T *topic_selector);


#endif
