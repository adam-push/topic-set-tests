#ifndef _diffusion_binary_value_h_
#define _diffusion_binary_value_h_ 1

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
 * @file diffusion-binary.h
 *
 * Binary datatype associated functions.
 */
#include <stdbool.h>

#include "buf.h"
#include "diffusion-api-error.h"
#include "diffusion-value.h"

/**
 * @brief Get the binary value
 *
 * @param value  value to retrieve the binary data from.
 * @param binary pointer to a pointer where the binary data will be
 *               stored. Must be freed after use.
 * @param error  populated if an error occurs. Can be NULL.
 *
 * @return true if the value is successfully read and binary pointer value set to the
 *         binary data. False otherwise.
 */
bool read_diffusion_binary_value(const DIFFUSION_VALUE_T *value, void **binary, DIFFUSION_API_ERROR *error);

/**
 * @brief Write binary data into a `BUF_T` for an update.
 *
 * @param binary    value to be written into the `BUF_T`
 * @param buf       `BUF_T` to write the binary value into
 * @param len       the length of the binary value to be written into the `BUF_T`
 *
 * @return True if the value was successfully written into the BUF_T.
 *         False will be returned if buf is NULL.
 */
bool write_diffusion_binary_value(const void *binary, const BUF_T *buf, size_t len);

#endif
