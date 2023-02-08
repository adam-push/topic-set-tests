#ifndef _diffusion_double_value_h_
#define _diffusion_double_value_h_ 1

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
 * @file diffusion-double.h
 *
 * Double datatype associated functions.
 */
#include <stdbool.h>

#include "buf.h"
#include "diffusion-api-error.h"
#include "diffusion-value.h"

/**
 * @brief Read a double from the diffusion value.
 *
 * @param value        the diffusion value.
 * @param double_value pointer to set the double value to.
 * @param error        populated if an error occurs. Can be NULL.
 *
 * @return true if the value was successfully set in the provided
 *         `double_value` pointer. False otherwise.
 */
bool read_diffusion_double_value(const DIFFUSION_VALUE_T *const value, double *double_value, DIFFUSION_API_ERROR *error);

/**
 * @brief Write a double value into a `BUF_T` for an update.
 *
 * @param value value to be written into the `BUF_T`
 * @param buf   `BUF_T` to write the double value into
 *
 * @return true if the value was successfully written into the BUF_T.
 *         False will be returned if buf is NULL.
 */
bool write_diffusion_double_value(double value, const BUF_T *buf);

#endif
