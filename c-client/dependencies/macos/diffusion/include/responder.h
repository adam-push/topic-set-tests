/**
 * @file responder.h
 *
 * Functions for working with `DIFFUSION_RESPONDER_HANDLE_T` to
 * either respond or reject requests.
 *
 * Copyright © 2018, 2021 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */

#ifndef _diffusion_responder_h_
#define _diffusion_responder_h_ 1

#include <stdbool.h>

#include "session.h"
#include "datatypes.h"
#include "diffusion-api-error.h"

/**
 * @brief A handle dispatched on receiving a request. Each request received has
 * its own unique handle. This handle is required to respond to a request
 * with the function `diffusion_respond_to_request` or reject a request with
 * `diffusion_reject_request`.
 */
typedef struct DIFFUSION_RESPONDER_HANDLE_T DIFFUSION_RESPONDER_HANDLE_T;

/**
 * @brief Retrieve the responder handle's response datatype.
 * 
 * @param handle            The response handle used when dispatching a response.
 * @param response_datatype A pointer to a `DIFFUSION_DATATYPE` which will have its
 *                          value set to the responder's response datatype.
 * @param error             Populated if an error occurs. Can be NULL.
 * 
 * @return true if the response datatype was successfully retrieved. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_responder_get_response_datatype(const DIFFUSION_RESPONDER_HANDLE_T *handle, DIFFUSION_DATATYPE *response_datatype, DIFFUSION_API_ERROR *error);

/**
 * @brief Respond to a request.
 * 
 * @param session           The session handle. If NULL, this function returns immediately.
 * @param handle            The response handle used when dispatching a response.
 * @param response          The response to be sent.
 * @param error             Populated if an error occurs. Can be NULL.
 * 
 * @return true if the response was successfully dispatched. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_respond_to_request(SESSION_T *session, const DIFFUSION_RESPONDER_HANDLE_T *handle, BUF_T *response, DIFFUSION_API_ERROR *error);

/**
 * @brief Reject a request
 * 
 * @param session           The session handle. If NULL, this function returns immediately.
 * @param handle            The response handle used when dispatching a response.
 * @param message           Context message to be contained in the rejection. Can be NULL.
 * @param error             Populated if an error occurs. Can be NULL.
 * 
 * @return true if the response was successfully rejected. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_reject_request(SESSION_T *session, const DIFFUSION_RESPONDER_HANDLE_T *handle, const char *message, DIFFUSION_API_ERROR *error);

/**
 * @brief Create a duplicate (copy) of an existing `DIFFUSION_RESPONDER_HANDLE_T`. `diffusion_responder_handle_free` should be
 *        called on the pointer when longer needed.
 * 
 * @param handle            The responder handle to be duplicated.
 * 
 * @return a copy of the provided responder handle. NULL, if the provided handle is NULL.
 */
DIFFUSION_RESPONDER_HANDLE_T *diffusion_responder_handle_dup(const DIFFUSION_RESPONDER_HANDLE_T *handle);

/**
 * @brief Free memory associated with a `DIFFUSION_RESPONDER_HANDLE_T`
 * 
 * @param handle             The responder handle to be freed.
 */
void diffusion_responder_handle_free(DIFFUSION_RESPONDER_HANDLE_T *handle);

#endif