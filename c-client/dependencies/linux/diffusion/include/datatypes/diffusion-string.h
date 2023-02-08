#ifndef _diffusion_string_value_h_
#define _diffusion_string_value_h_ 1

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
 * @file diffusion-string.h
 *
 * String datatype associated functions.
 */

#include "buf.h"
#include "diffusion-api-error.h"
#include "diffusion-value.h"

/**
 * @brief Get the string data from a String diffusion value.
 *
 * @param value  value to read from.
 * @param string pointer to a pointer where the string data will be stored
 * @param error  populated if an error occurs. Can be NULL.
 *
 * @return true if the value is successfully read and the `string` pointer value is
 *         set to a pointer containing the string data. False otherwise.
 */
bool read_diffusion_string_value(const DIFFUSION_VALUE_T *value, char **string, DIFFUSION_API_ERROR *error);

/**
 * @brief Write a string value into a `BUF_T` for an update.
 *
 * @param value string to be written into the BUF_T
 * @param buf   `BUF_T` to write the string value into
 *
 * @return true if the value was successfully written into the `BUF_T`.
 *         False will be returned if the provided string value or buf is NULL.
 */
bool write_diffusion_string_value(const char *value, const BUF_T *buf);

#endif
