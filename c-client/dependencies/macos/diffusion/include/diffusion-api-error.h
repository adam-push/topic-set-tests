#ifndef _diffusion_api_error_h_
#define _diffusion_api_error_h_ 1

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
 * @file diffusion-api-error.h
 *
 * Error handling functions
 */

#include <stdlib.h>

/**
 * Opaque diffusion API error data type.
 */
typedef struct DIFFUSION_API_ERROR_T* DIFFUSION_API_ERROR;

/**
 * @brief Return the error's code.
 *
 * @param error the API error
 *
 * @return the error code. If 0 is returned, the provided error value
 *         does not contain an error.
 */
int get_diffusion_api_error_code(DIFFUSION_API_ERROR error);

/**
 * @brief Returns the API error description.
 *
 * @param error the API error
 *
 * @return the description. Will return NULL if the provided error value
 *         does not contain an error.
 */
const char *get_diffusion_api_error_description(DIFFUSION_API_ERROR error);

/**
 * @brief Free a diffusion API error.
 *
 * @param error error to be freed.
 */
void diffusion_api_error_free(DIFFUSION_API_ERROR error);

#endif
