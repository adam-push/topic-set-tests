#ifndef _diffusion_json_value_h_
#define _diffusion_json_value_h_ 1

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
 * @file diffusion-json.h
 *
 * JSON datatype associated functions.
 */
#include <stdbool.h>

#include "buf.h"
#include "diffusion-api-error.h"
#include "diffusion-value.h"

/**
 * @brief Get a stringified representation of the JSON diffusion value.
 *
 * @param value       the diffusion value
 * @param json_string pointer to a pointer where the stringifed JSON
 *                    data will be stored
 * @param error error populated if an error occurs. Can be NULL.
 *
 * @return true if the value is successfully read and the `json_string` pointer value is
 *         set to a pointer containing the stringified representation of the JSON diffusion
 *         value. False otherwise.
 */
bool to_diffusion_json_string(const DIFFUSION_VALUE_T *value, char **json_string, DIFFUSION_API_ERROR *error);

/**
 * @brief Write a JSON value into a `BUF_T` for an update.
 *
 * @param json  value to be written into the `BUF_T`
 * @param buf   `BUF_T` to write the JSON value into
 *
 * @return true if the value was successfully written into the BUF_T.
 *         False will be returned if the provided JSON string is invalid
 *         JSON, or json or buf is NULL.
 */
bool write_diffusion_json_value(const char *json, const BUF_T *buf);

#endif
