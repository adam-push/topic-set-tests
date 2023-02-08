#ifndef _diffusion_subscription_control_h_
#define _diffusion_subscription_control_h_ 1

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
 * @file subscription-control.h
 *
 * Allows the subscription on behalf of other sessions to the topic tree.
 */

#include "session.h"
#include "diffusion-api-error.h"

/**
 * @brief Callback for subscribe_client() and unsubscribe_client().
 *
 * @param session       The current active session.
 * @param context       User-supplied context from the originating call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_subscription_control_cb)(SESSION_T *session, void *context);

/**
 * @brief Callback for `diffusion_subscribe_by_filter` and `diffusion_unsubscribe_by_filter`.
 *
 * @param number_selected       Number of sessions matching the filter.
 * @param context               User-supplied context from the originating call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_subscription_control_by_filter_cb)(const int number_selected, void *context);

/**
 * @brief Callback handlers that may be registered so the caller may
 * receive notifcation that a subscription or unsubscription request
 * has completed.
 */
#define SUBSCRIPTION_CONTROL_HANDLERS                                   \
        /** A callback to indicate that the subscription or  unsubscription request has completed. */ \
        on_subscription_control_cb on_complete;                         \
        /** Standard service error callback */                          \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard callback */                        \
        DISCARD_HANDLER_T on_discard;

typedef struct subscription_control_params_s {
        /// Callback handlers.
        SUBSCRIPTION_CONTROL_HANDLERS
        /// ID of the session to be subscribed to or unsubscribed from
        /// the topic selector.
        SESSION_ID_T session_id;
        /// Topic selector.
        char *topic_selector;
        /// User-supplied context returned to callbacks.
        void *context;
} SUBSCRIPTION_CONTROL_PARAMS_T;

/**
 * Structure supplied when issuing a `diffusion_subscribe_by_filter` call.
 */
typedef struct diffusion_subscribe_by_filter_s {
        /// The session filter expression.
        const char *filter;
        /// The topics to subscribe to.
        const char *topic_selector;
        /// Callback
        on_subscription_control_by_filter_cb on_subscribe_by_filter;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_SUBSCRIBE_BY_FILTER_PARAMS_T;

/**
 * Structure supplied when issuing a `diffusion_unsubscribe_by_filter` call.
 */
typedef struct diffusion_unsubscribe_by_filter_s {
        /// The session filter expression.
        const char *filter;
        /// The topics to unsubscribe from.
        const char *topic_selector;
        /// Callback
        on_subscription_control_by_filter_cb on_unsubscribe_by_filter;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_UNSUBSCRIBE_BY_FILTER_PARAMS_T;

/**
 * @ingroup PublicAPI_SubscriptionControl
 *
 * @brief Subscribe a session to topics.
 *
 * This function requests that a client is subscribed to the topics
 * specified by a topic selector.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters describing the subscription control
 *                      request and callback handlers which may be invoked
 *                      in response.
 */
void subscribe_client(SESSION_T *session, const SUBSCRIPTION_CONTROL_PARAMS_T params);

/**
 * @ingroup PublicAPI_SubscriptionControl
 *
 * @brief Unsubscribe a session from topics.
 *
 * This function requests that a client is unsubscribed from the
 * topics specified by a topic selector.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters describing the subscription control
 *                      request and callback handlers which may be invoked
 *                      in response.
 */
void unsubscribe_client(SESSION_T *session, const SUBSCRIPTION_CONTROL_PARAMS_T params);

/**
 * @ingroup PublicAPI_SubscriptionControl
 *
 * @brief Subscribe sessions that satisfy a given session filter to topics.
 *
 * For each session that matches the filter, new subscriptions will be
 * established for existing topics that match the provided topic selector
 * and for which the sessions has `READ_TOPIC` permission. The topic
 * selector will be added to the topic selections of the subscribed session,
 * and re-evaluated when new topics are added or the session's security
 * roles change.
 *
 * A session that does not have `SELECT_TOPIC` permission for a topic
 * cannot subscribe directly, but can be subscribed indirectly using this
 * method.
 *
 * This function requests that a client is subscribed to the topics
 * specified by a topic selector.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters describing the `diffusion_subscribe_by_filter`
 *                      request and callback handlers which may be invoked
 *                      in response.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_subscribe_by_filter(SESSION_T *session, const DIFFUSION_SUBSCRIBE_BY_FILTER_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_SubscriptionControl
 *
 * @brief Unsubscribe sessions that satisfy a given session filter from topics.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters describing the `diffusion_unsubscribe_by_filter`
 *                      request and callback handlers which may be invoked
 *                      in response.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_unsubscribe_by_filter(SESSION_T *session, const DIFFUSION_UNSUBSCRIBE_BY_FILTER_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

#endif
