#ifndef _diffusion_data_value_h_
#define _diffusion_data_value_h_ 1

/*
 * Copyright © 2018 - 2022 Push Technology Ltd., All Rights Reserved.
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
 * @file diffusion-value.h
 *
 * Generic datatype value associated functions.
 */
#include <stdlib.h>
#include <stdbool.h>

/**
 * Opaque diffusion value data type.
 */
typedef struct DIFFUSION_VALUE_T DIFFUSION_VALUE_T;

/**
 * @brief Retrieve the value's raw bytes. If the raw bytes
 * are successfully retrieved from the diffusion value, the
 * "len" parameter will have its value set to the length of
 * the returned bytes.
 *
 * @param value     the diffusion value
 * @param raw_bytes pointer to a pointer to be set
 * @param len       `size_t` pointer to have its value set to the
 *                  length of the bytes data.
 *
 * @return true if the value's raw bytes were successfully retrieved.
 *         False otherwise.
 */
bool diffusion_value_get_raw_bytes(const DIFFUSION_VALUE_T *value, void **raw_bytes, size_t *len);

/**
 * @brief Create a copy of a DIFFUSION_VALUE_T. `diffusion_value_free` should be
 * called on this pointer when no longer needed.
 *
 * @param value value to be copied.
 *
 * @return copy of the provided DIFFUSION_VALUE_T. NULL if the value
 *         cannot be copied.
 */
DIFFUSION_VALUE_T *diffusion_value_dup(const DIFFUSION_VALUE_T *value);

/**
 * @brief Free a diffusion value.
 *
 * @param value value to be freed.
 */
void diffusion_value_free(DIFFUSION_VALUE_T *value);

#endif
