#ifndef _diffusion_int64_value_h_
#define _diffusion_int64_value_h_ 1

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
 * @file diffusion-int64.h
 *
 * Int64 datatype associated functions.
 */
#include <stdbool.h>

#include "buf.h"
#include "diffusion-api-error.h"
#include "diffusion-value.h"


/**
 * @brief Read an int64 from the diffusion value.
 *
 * @param value       the diffusion value.
 * @param int64_value pointer to set the int64 value to.
 * @param error       populated if an error occurs. Can be NULL.
 *
 * @return true if the value was successfully set in the provided
 *         "int64_value" pointer. False otherwise.
 */
bool read_diffusion_int64_value(const DIFFUSION_VALUE_T *value, int64_t *int64_value, DIFFUSION_API_ERROR *error);

/**
 * @brief Write a int64 value into a `BUF_T` for an update.
 *
 * @param value int64 to be written into the `BUF_T`
 * @param buf   `BUF_T` to write the int64 value into
 *
 * @return true if the value was successfully written into the `BUF_T`.
 */
bool write_diffusion_int64_value(int64_t value, const BUF_T *buf);

#endif
