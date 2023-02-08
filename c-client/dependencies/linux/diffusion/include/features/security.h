#ifndef _diffusion_security_h_
#define _diffusion_security_h_ 1

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
 * @file security.h
 *
 * Security feature.
 */
#include "session.h"
#include "diffusion-api-error.h"

/**
 * @brief Callback indicating that a response has been received to a
 * change principal request.
 *
 * @param session The current active session.
 * @param context User-supplied context from the original change_principal() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_change_principal_cb)(SESSION_T *session, void *context);

/**
 * @brief Callback for `diffusion_get_global_permissions`.
 *
 * @param global_permissions a `SET_T` containing `GLOBAL_PERMISSIONS_T`s
 * @param context            User-supplied context from the originating call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_get_global_permissions_cb)(const SET_T *global_permissions, void *context);

/**
 * @deprecated This function is deprecated since version 6.5 and onwards, and may
 * be removed
 *
 * @brief Callback for `diffusion_get_path_permissions`.
 *
 * @param topic_permissions  a `SET_T` containing `TOPIC_PERMISSIONS_T`s
 * @param context            User-supplied context from the originating call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
TYPEDEF_DEPRECATED(int, (*on_get_topic_permissions_cb)(const SET_T *topic_permissions, void *context))

/**
 * @since 6.5
 *
 * @brief Callback for `diffusion_get_path_permissions`.
 *
 * @param path_permissions  a `SET_T` containing `PATH_PERMISSIONS_T`s
 * @param context            User-supplied context from the originating call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_get_path_permissions_cb)(const SET_T *path_permissions, void *context);

/**
 * @brief Structure holding callbacks to be invoked after a request
 * to change the principal has been sent to and processed by
 * Diffusion.
 */
#define CHANGE_PRINCIPAL_HANDLERS                                       \
        /** Called when the change principal request is successful. */  \
        on_change_principal_cb on_change_principal;                     \
        /** Called when the change principal request results in failure. */ \
        on_change_principal_cb on_change_principal_failure;             \
        /** Standard service error handler callback. */                 \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard handler callback. */               \
        DISCARD_HANDLER_T on_discard;

typedef struct change_principal_handlers_s {
        CHANGE_PRINCIPAL_HANDLERS
} CHANGE_PRINCIPAL_HANDLERS_T;

/**
 * @brief Structure supplied when issuing a change_principal() request.
 */
typedef struct change_principal_params_s {
        CHANGE_PRINCIPAL_HANDLERS
        /// The new principal (username).
        const char *principal;
        /// The credentials associated with the principal.
        const CREDENTIALS_T *credentials;
        /// User-supplied context to return to callbacks.
        void *context;
} CHANGE_PRINCIPAL_PARAMS_T;

/**
 * @brief Structure supplied when issuing a `diffusion_get_global_permissions` request.
 */
typedef struct diffusion_get_global_permissions_params_s {
        /// Callback
        on_get_global_permissions_cb on_global_permissions;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_GET_GLOBAL_PERMISSIONS_PARAMS_T;

/**
 * @brief Structure supplied when issuing a `diffusion_get_path_permissions` request.
 */
typedef struct diffusion_get_path_permissions_params_s {
        /// The topic path to retrieve permissions for.
        const char *path;
        /// Callback
        on_get_path_permissions_cb on_path_permissions;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_GET_PATH_PERMISSIONS_PARAMS_T;

/**
 * @ingroup PublicAPI_Security
 *
 * @brief Issue a request to change the principal for the current (active)
 * session.
 *
 * @param session       The session handle. If NULL, this function has no effect.
 * @param params        Parameters defining the change_prinicpal() request
 *                      and notification callbacks.
 */
void change_principal(SESSION_T *session, const CHANGE_PRINCIPAL_PARAMS_T params);

/**
 * @ingroup PublicAPI_Security
 *
 * @brief Query the global permissions assigned to the calling session.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters describing the `diffusion_get_global_permissions`
 *                      request and callback handlers which may be invoked
 *                      in response.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_get_global_permissions(SESSION_T *session, const DIFFUSION_GET_GLOBAL_PERMISSIONS_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_Security
 *
 * @brief Query the path permissions assigned to the calling session on a given path.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters describing the `diffusion_get_path_permissions`
 *                      request and callback handlers which may be invoked
 *                      in response.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_get_path_permissions(SESSION_T *session, const DIFFUSION_GET_PATH_PERMISSIONS_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

#endif
