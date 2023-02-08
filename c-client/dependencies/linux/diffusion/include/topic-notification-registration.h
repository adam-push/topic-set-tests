#ifndef _diffusion_topic_notification_registration_h_
#define _diffusion_topic_notification_registration_h_ 1

/*
 * Copyright © 2019, 2021 Push Technology Ltd., All Rights Reserved.
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
 * @file topic-notification-registration.h
 *
 * The registration state of the associated listener on the server.
 *
 * The registration also provides operations to control which
 * topic paths the listener will receive notifications for.
 */

#include "session.h"
#include "registration.h"
#include "diffusion-api-error.h"

/**
 * @brief Callback when topic notifications for a topic
 * selector has successfully been selected.
 *
 * @param context User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_selected_cb)(void *context);

/**
 * @brief Callback when topic notifications for a topic
 * selector has successfully been deselected.
 *
 * @param context User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_deselected_cb)(void *context);

/**
 * @brief Structure supplied when issuing a `diffusion_topic_notification_registration_select` or
 * `diffusion_topic_notification_registration_deselect` request.
 */
typedef struct diffusion_topic_notification_registration_params_s {
        /// Callback when topic notifications for a topic
        /// selector has been selected.
        on_selected_cb on_selected;
        /// Callback when topic notifications for a topic
        /// selector has been deselected.
        on_deselected_cb on_deselected;
        /// The topic notification registration.
        DIFFUSION_REGISTRATION_T *registration;
        /// The topic selector string.
        const char *selector;
        /// Callback to handle errors from the server. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_TOPIC_NOTIFICATION_REGISTRATION_PARAMS_T;

/**
 * @ingroup PublicAPI_TopicNotifications
 *
 * @brief Request to receive notifications for all topics matched by the
 * provided selector.
 *
 * @param session      The session handle. If NULL, this function returns immediately.
 * @param params       Parameters defining the `diffusion_topic_notification_registration_select` request
 *                     and notification callbacks.
 * @param api_error    Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_topic_notification_registration_select(SESSION_T *session, const DIFFUSION_TOPIC_NOTIFICATION_REGISTRATION_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_TopicNotifications
 *
 * @brief Request to stop receiving notifications for all topics matched by the
 * given selector.
 *
 * @param session      The session handle. If NULL, this function returns immediately.
 * @param params       Parameters defining the `diffusion_topic_notification_registration_deselect` request
 *                     and notification callbacks.
 * @param api_error    Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_topic_notification_registration_deselect(SESSION_T *session, const DIFFUSION_TOPIC_NOTIFICATION_REGISTRATION_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

#endif
