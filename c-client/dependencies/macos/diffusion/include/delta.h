/**
 * @file delta.h
 *
 * Functions for generating deltas describing the changes to data, and
 * applying deltas to data.
 *
 * Copyright © 2016, 2021 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */
#ifndef _diffusion_delta_h_
#define _diffusion_delta_h_ 1

#include "buf.h"
#include "activity-history.h"

/**
 * When generating a delta, a chunk of memory is set aside for working
 * storage. This is the default value if not explicity specified.
 */
#define DIFF_DEFAULT_MAX_STORAGE INT_MAX

/**
 * In order to prevent complex delta generation from using up too much CPU
 * (especially with larger messages), a bailout factor is referenced to abort
 * the process early. If not explicitly specified, this is the value used. If
 * the process does bail out, then a delta may still be generated but may not
 * be of optimal length.
 */
#define DIFF_DEFAULT_BAILOUT_FACTOR 10000


/**
 * @brief Apply a delta to a value, producing a new value. `buf_free` should be called
 *        on the pointer when no longer needed.
 *
 * @param old_value     The original value
 * @param diff          The delta (diff) to apply to the original value.
 * @retval "BUF_T *"    A pointer to a BUF_T containing the result of applying
 *                      the delta to the original value.
 */
BUF_T *diff_apply_binary(
        BUF_T *old_value,
        BUF_T *diff);


/**
 * @brief Generate a delta between two binary values. `buf_free` should be called
 *        on the pointer when no longer needed.
 *
 * @param old_value     The source value
 * @param new_value     The target value
 * @retval "BUF_T *"    A pointer to a BUF_T containing a delta describing how to
 *                      transform the source value into the target value. If there are no
 *                      differences or an error occurs, then NULL is returned.
 */
BUF_T *diff_generate_binary(
        BUF_T *old_value,
        BUF_T *new_value);


/**
 * @brief Generate a delta between two binary values.
 *        The quality of the delta is based on the activity history passed as a parameter.
 *        `buf_free` should be called on the pointer when no longer needed.
 *
 * @param activity_history  The activity history of a session
 * @param old_value	        The source value
 * @param new_value         The target value
 * @retval "BUF_T *"        A pointer to a BUF_T containing a delta describing how to
 *                          transform the source value into the target value. If there are no
 *                          differences or an error occurs, then NULL is returned.
 */
BUF_T *diff_generate_binary_with_activity_history(
        DIFFUSION_ACTIVITY_HISTORY_T *activity_history,
        BUF_T *old_value,
        BUF_T *new_value);


/**
 * @brief Generate a delta between two binary values, with additional control over
 *        variables that influence the process.
 *        `buf_free` should be called on the pointer when no longer needed.
 *
 * @param old_value         The source value
 * @param new_value         The target value
 * @param max_storage       The maximum amount of internal storage to use.
 * @param bailout_factor    Influences when to abort delta generation; smaller
 *                          values will abort sooner. This is useful with larger messages where the
 *                          process may use a significant amount of CPU time.
 * @retval "BUF_T *"        A pointer to a BUF_T containing a delta describing how to
 *                          transform the source value into the target value. If there are no
 *                          differences or an error occurs, then NULL is returned.
 */
BUF_T *diff_generate_binary_ex(
        BUF_T *old_value,
        BUF_T *new_value,
        int max_storage,
        int bailout_factor);


/**
 * @brief Generate a delta between two binary values, with additional control over
 *        variables that influence the process.
 *        The quality of the delta is based on the activity history passed as a parameter.
 *        `buf_free` should be called on the pointer when no longer needed.
 *
 * @param activity_history  The activity history of a session
 * @param old_value         The source value
 * @param new_value         The target value
 * @param max_storage       The maximum amount of internal storage to use.
 * @param bailout_factor    Influences when to abort delta generation; smaller
 *                          values will abort sooner. This is useful with larger messages where the
 *                          process may use a significant amount of CPU time.
 * @retval "BUF_T *"        A pointer to a BUF_T containing a delta describing how to
 *                          transform the source value into the target value. If there are no
 *                          differences or an error occurs, then NULL is returned.
 */
BUF_T *diff_generate_binary_with_activity_history_ex(
        DIFFUSION_ACTIVITY_HISTORY_T *activity_history,
        BUF_T *old_value,
        BUF_T *new_value,
        int max_storage,
        int bailout_factor);

#endif
