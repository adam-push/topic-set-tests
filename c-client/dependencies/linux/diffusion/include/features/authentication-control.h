#ifndef _diffusion_authentication_control_h_
#define _diffusion_authentication_control_h_ 1

/*
 * Copyright © 2014 - 2022 Push Technology Ltd., All Rights Reserved.
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
 * @file authentication-control.h
 *
 * Allows registering and deregistering an authentication service in a control client.
 */

#include "authenticator.h"
#include "registration.h"
#include "session.h"
#include "hash.h"
#include "types/client_details_types.h"

/**
 * If not specified, the group name "default" is used when registering
 * authentication handlers.
 */
#define DEFAULT_AUTH_GROUP_NAME "default"

/**
 * @brief Callback when an authentication handler is active.
 *
 * @param session            The currently active session.
 * @param registered_handler The registered handler
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*authenticator_on_active)(SESSION_T *session, const DIFFUSION_REGISTRATION_T *registered_handler);

/**
 * @brief Callback when an authentication handler has received a connection request to be
 * authenticated.
 *
 * @param session                     The currently active session.
 * @param principal                   The principal of the connection request to be authenticated
 * @param credentials                 The credentials of the connection request to be authenticated
 * @param session_properties          The session properties of the connection request to be authenticated
 * @param proposed_session_properties The proposed properties from the connection request to be authenticated
 * @param authenticator               The authenticator. Used to authenticate connection requests (see authenticator.h)
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*authenticator_on_authenticate)(SESSION_T *session, const char *principal, const CREDENTIALS_T *credentials,
                                             const HASH_T *session_properties, const HASH_T *proposed_session_properties,
                                             const DIFFUSION_AUTHENTICATOR_T *authenticator);

/**
 * @brief Callback when an authenticator has encountered an error.
 *
 * @param error The received error.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*authenticator_on_error)(const DIFFUSION_ERROR_T *error);

/**
 * @brief Callback when an authentication handler is closed.
 */
typedef void (*authenticator_on_close)(void);

/**
 * @brief Structure supplied as part of a `diffusion_set_authentication_handler` request.
 */
typedef struct diffusion_authentication_handler_s {
        /// Name of the authentication handler.
        char *handler_name;
        /// Callback when the authentication handler is
        /// active.
        authenticator_on_active on_active;
        /// Callback when a connection request to be
        /// authenticated has been received.
        authenticator_on_authenticate on_authenticate;
        /// Callback when the authentication handler
        /// encounters an error.
        authenticator_on_error on_error;
        /// Callback when an authentication handler
        /// is closed.
        authenticator_on_close on_close;
} DIFFUSION_AUTHENTICATION_HANDLER_T;

/**
 * @brief Structure supplied when issuing an `diffusion_set_authentication_handler` request.
 */
typedef struct diffusion_authentication_handler_params_s {
        /// Handler
        DIFFUSION_AUTHENTICATION_HANDLER_T *handler;
        /// Standard service error handler callback.
        ERROR_HANDLER_T on_error;
        /// Standard conversation discard callback.
        DISCARD_HANDLER_T on_discard;
        /// User-supplied context to return to callbacks.
        void *context;
} DIFFUSION_AUTHENTICATION_HANDLER_PARAMS_T;

/**
 * @ingroup PublicAPI_AuthenticationControl
 *
 * @brief Register an authentication handler for client authentication events.
 *
 * @param session       The session handle. If NULL, this function returns immediately
 *                      with the value NULL.
 * @param params        Parameters defining the request to register an
 *                      authentication handler.
 */
void diffusion_set_authentication_handler(SESSION_T *session, const DIFFUSION_AUTHENTICATION_HANDLER_PARAMS_T params);

#endif
