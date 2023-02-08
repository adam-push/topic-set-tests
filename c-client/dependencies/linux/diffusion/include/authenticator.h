/*
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

/**
 * @file authenticator.h
 *
 * An authentication handler that processes authentication requests from the
 * server.
 *
 * Instances can be registered with the server using the `diffusion_set_authentication_handler`
 * feature.
 *
 * The server calls an authentication handler when a client application creates
 * a session, or changes the principal associated with a session, allowing the
 * handler to veto individual requests.
 *
 * Authentication handlers are configured in precedence order. Authentication
 * will succeed if a handler responds by calling `diffusion_authenticator_allow` or
 * `diffusion_authenticator_allow_with_properties` and handlers with higher precedence
 * respond by calling `diffusion_authenticator_abstain`
 *
 * Authentication will fail if a handler responds by calling
 * `diffusion_authenticator_deny` and all higher precedence handlers respond by
 * calling `diffusion_authenticator_abstain`
 *
 * If all authentication handlers respond by calling `diffusion_authenticator_abstain`, the request
 * will be denied. Once the outcome is known, the server may choose not to call any remaining
 * authentication handlers.
 */

#ifndef _diffusion_authenticator_h_
#define _diffusion_authenticator_h_ 1

#include <stdlib.h>
#include <stdbool.h>

#include "session.h"
#include "diffusion-api-error.h"

/**
 * @brief Opaque diffusion authenticator struct.
 *
 * Each authentication request receives its own unique, single use authenticator.
 * An authenticator can only be used once to either allow, abstain or deny an
 * authentication request.
 */
typedef struct DIFFUSION_AUTHENTICATOR_T DIFFUSION_AUTHENTICATOR_T;

/**
 * @brief Authentication passed - allow the authentication request with fixed
 * properties as supplied but no user-defined properties.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param authenticator The session authenticator
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the authentication response was successfully dispatched. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated with the error
 *         information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_authenticator_allow(SESSION_T *session, const DIFFUSION_AUTHENTICATOR_T *authenticator, DIFFUSION_API_ERROR *error);

/**
 * @brief Authentication passed - allow the authentication request with
 * modifications to the session properties.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param authenticator The session authenticator
 * @param properties    This can include all allowed user-defined session
 *                      properties, as well as a subset of fixed session properties
 *                      see
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the authentication response was successfully dispatched. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated with the error
 *         information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_authenticator_allow_with_properties(SESSION_T *session, const DIFFUSION_AUTHENTICATOR_T *authenticator, const HASH_T *properties, DIFFUSION_API_ERROR *error);

/**
 * @brief The authentication has neither passed nor failed.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param authenticator The session authenticator
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the authentication response was successfully dispatched. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated with the error
 *         information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_authenticator_abstain(SESSION_T *session, const DIFFUSION_AUTHENTICATOR_T *authenticator, DIFFUSION_API_ERROR *error);

/**
 * @brief Authentication failed - deny the authentication request.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param authenticator The session authenticator
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the authentication response was successfully dispatched. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated with the error
 *         information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_authenticator_deny(SESSION_T *session, const DIFFUSION_AUTHENTICATOR_T *authenticator, DIFFUSION_API_ERROR *error);

/**
 * @brief Create a duplicate (copy) of an existing `DIFFUSION_AUTHENTICATOR_T`. `diffusion_authenticator_free` should be called
 *        on this pointer when no longer needed.
 *
 * @param authenticator The authenticator to be duplicated.
 *
 * @return a copy of the provided authenticator. NULL, if the provided authenticator is NULL.
 */
DIFFUSION_AUTHENTICATOR_T *diffusion_authenticator_dup(const DIFFUSION_AUTHENTICATOR_T *authenticator);

/**
 * @brief Free a memory allocated `DIFFUSION_AUTHENTICATOR_T`
 *
 * @param authenticator the authenticator to be freed.
 */
void diffusion_authenticator_free(DIFFUSION_AUTHENTICATOR_T *authenticator);

#endif