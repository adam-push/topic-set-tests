#ifndef _diffusion_topics_h_
#define _diffusion_topics_h_ 1

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
 * @file topics.h
 *
 * Subscription to topics and fetching topic data.
 */
#include "session.h"
#include "misc/deprecate.h"

#include "fetch-request.h"

/**
 * @brief Callback for on_subscribe().
 *
 * @param session The current active session.
 * @param context User-supplied context from the initial subscribe() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_subscribe_cb)(SESSION_T *session, void *context);

/**
 * @brief Callback handlers that may be registered to deal with
 * messages sent by Diffusion in response to the subscription request.
 */
#define SUBSCRIPTION_HANDLERS                                           \
        /** A callback for when a topic message has been received. */   \
        TOPIC_HANDLER_T on_topic_message;                               \
        /** A callback to indicate that the subscription request has been received. */ \
        on_subscribe_cb on_subscribe;                                   \
        /** Standard service error callback. */                         \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard callback. */                       \
        DISCARD_HANDLER_T on_discard;

typedef struct subscription_handlers_s {
        SUBSCRIPTION_HANDLERS
} SUBSCRIPTION_HANDLERS_T;

/**
 * @brief Structure supplied when subscribing to a topic.
 */
typedef struct subscription_params_s {
        /// Callback handlers.
        SUBSCRIPTION_HANDLERS
        /// Topic selected describing which topics to subscribe to.
        const char *topic_selector;
        /// User-supplied context return to callbacks.
        void *context;
} SUBSCRIPTION_PARAMS_T;

/**
 * @brief Structure describing an incoming notification that the
 * client has been subscribed to a topic.
 */
typedef struct svc_notify_subscription_request_s {
        /// Topic identification.
        struct {
                /// Internal identifier for the topic.
                uint32_t topic_id;
                /// Name of the topic which the client was subscribed
                /// to.
                char *topic_path;
        } topic_info;
        /// Description of the structure of the topic.
        TOPIC_SPECIFICATION_T *topic_specification;
} SVC_NOTIFY_SUBSCRIPTION_REQUEST_T;

/**
 * @brief Callback for notify_subscription_register().
 *
 * @param session The current active session.
 * @param request The incoming notification request.
 * @param context User-supplied context from the initial registration call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_notify_subscription_cb)(SESSION_T *session,
                                         const SVC_NOTIFY_SUBSCRIPTION_REQUEST_T *request,
                                         void *context);

/**
 * @brief Callback handlers for subscription notification messages.
 */
#define NOTIFY_SUBSCRIPTION                                             \
        /** Callback fired when a subscription notification has been received. */ \
        on_notify_subscription_cb on_notify_subscription;               \
        /** Standard service error callback. */                         \
        ERROR_HANDLER_T on_error;

typedef struct notify_subscription_handlers_s {
        NOTIFY_SUBSCRIPTION
} NOTIFY_SUBSCRIPTION_HANDLERS_T;

/**
 * @brief Structure supplied when registering to receive topic
 * subscription notifications.
 */
typedef struct notify_subscription_params_s {
        NOTIFY_SUBSCRIPTION
        /// User-supplied context returned to callbacks.
        void *context;
} NOTIFY_SUBSCRIPTION_PARAMS_T;

/**
 * @brief Callback for on_unsubscribe().
 *
 * @param session The current active session.
 * @param context User-supplied context from the initial unsubscribe() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_unsubscribe_cb)(SESSION_T *session, void *context);

/**
 * @brief Callback handlers that may be registered to deal with
 * messages sent by Diffusion in response to the unsubscription
 * request.
 */
#define UNSUBSCRIPTION_HANDLERS                                         \
        /** A callback to indicate that the unsubscription request has been received. */ \
        on_unsubscribe_cb on_unsubscribe;                               \
        /** Standard error handler callback. */                         \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard discard handler callback. */                       \
        DISCARD_HANDLER_T on_discard;

typedef struct unsubscription_handlers_s {
        UNSUBSCRIPTION_HANDLERS
} UNSUBSCRIPTION_HANDLERS_T;

/**
 * @brief Structure supplied when unsubscribing from a topic.
 */
typedef struct unsubscription_params_s {
        UNSUBSCRIPTION_HANDLERS
        /// Topic selector describing which topics to unsubscribe from.
        const char *topic_selector;
        /// User-supplied context returned to callbacks.
        void *context;
} UNSUBSCRIPTION_PARAMS_T;

/**
 * @brief Reason for unsubscription.
 */
typedef enum {
        /// Unsubscribed by the subscribing client.
        ///
        /// @since 5.6
        UNSUBSCRIPTION_REASON_REQUESTED = 0,

        /// The unsubscription was requested either by another client
        /// or by the server.
        ///
        /// @since 5.6
        UNSUBSCRIPTION_REASON_CONTROL = 1,

        /// The unsubscription occurred because the topic was removed.
        ///
        /// @since 5.6
        UNSUBSCRIPTION_REASON_REMOVAL = 2,

        /// The unsubscription occurred because the session is no longer
        /// authorized to access the topic.
        ///
        /// @since 5.9
        UNSUBSCRIPTION_REASON_AUTHORIZATION = 3,

        /// A reason that is unsupported by the session.
        ///
        /// @since 6.1
        UNSUBSCRIPTION_REASON_UNKNOWN_UNSUBSCRIBE_REASON = 4,

        /// The server has a significant backlog of messages for the session,
        /// and the topic specification has the
        /// `DIFFUSION_CONFLATION` topic property set to "unsubscribe".
        /// The session can resubscribe to the topic. The unsubscription
        /// is not persisted to the cluster, if the session fails
        /// over to a different server it will be resubscribed to the topic.
        ///
        /// @since 6.1
        UNSUBSCRIPTION_REASON_BACK_PRESSURE = 5,

        /// The unsubscription occurred because branch mapping rules changed.
        ///
        /// @since 6.7
        UNSUBSCRIPTION_REASON_BRANCH_MAPPINGS = 6,

        /// The server has re-subscribed this session to the topic. Existing
        /// streams are unsubscribed because the topic type and other
        /// attributes may have changed.
        ///
        /// This can happen if a set of servers is configured to use session
        /// replication, and a session connected to one server reconnects
        /// ("fails over") to a different server.
        ///
        /// A stream that receives an unsubscription notification with this
        /// reason will also receive a subscription notification with the new
        /// {@link TOPIC_SPECIFICATION_T topic specification}.
        ///
        /// @since 5.9
        UNSUBSCRIPTION_REASON_SUBSCRIPTION_REFRESH = 0xfffffff0,

        /// A fallback stream has been unsubscribed due to the addition of a
        /// stream that selects the topic.
        ///
        /// @since 5.9
        UNSUBSCRIPTION_REASON_STREAM_CHANGE = 0xfffffff01
} NOTIFY_UNSUBSCRIPTION_REASON_T;

/**
 * @brief Structure describing an incoming unsubscription
 * notification.
 */
typedef struct svc_notify_unsubscription_request_s {
        /// The internal identifier of the topic.
        uint32_t topic_id;
        /// The topic path, or NULL if not available.
        char *topic_path;
        /// The reason why the unsubscription happened.
        NOTIFY_UNSUBSCRIPTION_REASON_T reason;
} SVC_NOTIFY_UNSUBSCRIPTION_REQUEST_T;

/**
 * @brief Callback for notify_unsubscription_register.
 *
 * @param session The current active session.
 * @param request The incoming notification message.
 * @param context User-supplied context from the initial registration call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_notify_unsubscription_cb)(SESSION_T *session,
                                           const SVC_NOTIFY_UNSUBSCRIPTION_REQUEST_T *request,
                                           void *context);

/**
 * @brief Callback handlers for unsubscription notification callbacks.
 */
#define NOTIFY_UNSUBSCRIPTION                                           \
        /** Callback fired an an unsubscription notification has been received. */ \
        on_notify_unsubscription_cb on_notify_unsubscription;             \
        /** Standard service error callback. */                         \
        ERROR_HANDLER_T on_error;

typedef struct notify_unsubscription_handlers_s {
        NOTIFY_UNSUBSCRIPTION
} NOTIFY_UNSUBSCRIPTION_HANDLERS_T;

/**
 * @brief Structure supplied when registering to receive topic
 * unsubscription notifications.
 */
typedef struct notify_unsubscription_params_s {
        NOTIFY_UNSUBSCRIPTION
        /// User-supplied context returned to callbacks.
        void *context;
} NOTIFY_UNSUBSCRIPTION_PARAMS_T;

/**
 * @ingroup PublicAPI_Topics
 *
 * @brief Subscribe to topics.
 *
 * This function has the roles of subscribing to the topics specified by the
 * selector and installing handlers for received \ref TOPIC_MESSAGE_T messages
 * which match that selector and also any other service messages related to
 * subscription. These handlers are called before (and in addition to) any
 * handlers installed using add_message_handler() for \ref
 * MESSAGE_TYPE_TOPIC_LOAD or \ref MESSAGE_TYPE_DELTA messages.
 *
 * If a topic handler was already registered for the supplied selector, it is
 * replaced and the old handlers are returned.
 *
 * The session will become subscribed to each existing topic matching
 * the selector unless the session is already subscribed to the topic,
 * or the session does not have `READ_TOPIC` permission for the topic
 * path. For each topic to which the session becomes subscribed, a
 * subscription notification and initial value (if any) will be
 * delivered to registered value streams before the completion handler
 * is called.
 *
 * The subscription request is also retained at the server and the
 * session will be automatically subscribed to newly created topics
 * that match the selector (unless a subsequent unsubscription cancels
 * the request).
 *
 * @param session       The session handle. If NULL, the function returns immediately.
 * @param params        Parameters describing the subscription request and
 *                      callbacks handlers which may be invoked in response.
 * @return              Previous topic handler if replacing, else NULL.
 */
TOPIC_HANDLER_T subscribe(SESSION_T *session, const SUBSCRIPTION_PARAMS_T params);

/**
 * @ingroup PublicAPI_Topics
 *
 * @brief Register to receive subscription notifications.
 *
 * This function should be called by the application to register to
 * receive notifications that it has been subscribed to a topic.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Callbacks and other parameters. {@link NOTIFY_SUBSCRIPTION_PARAMS_T on_notify_subscription}
 *                      should be set to receive notifications.
 */
void notify_subscription_register(SESSION_T *session, const NOTIFY_SUBSCRIPTION_PARAMS_T params);

/**
 * @ingroup PublicAPI_Topics
 *
 * @brief Unsubscribe from one or more topics.
 *
 * This function has the roles of subscribing to the topics specified by the
 * selector and installing handlers for received \ref TOPIC_MESSAGE_T messages
 * which match that selector and also any other service messages related to
 * unsubscription.
 *
 * @param session     The session handle. If NULL, this function returns immediately.
 * @param params      Parameters describing the unsubscription request and
 *                    callback handlers which may be invoked in response.
 */
void unsubscribe(SESSION_T *session, const UNSUBSCRIPTION_PARAMS_T params);

/**
 * @ingroup PublicAPI_Topics
 *
 * @brief Register to receive unsubscription notifications.
 *
 * This function should be called by the application to receive
 * notifications that it has been unsubscribed to a topic.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Callbacks and other parameters. {@link NOTIFY_UNSUBSCRIPTION_PARAMS_T on_notify_unsubscription}
 *                      should be set to receive notifications.
 */
void notify_unsubscription_register(SESSION_T *session, const NOTIFY_UNSUBSCRIPTION_PARAMS_T params);

/**
 * @ingroup PublicAPI_Topics
 *
 * @brief Creates an unconfigured fetch request.
 *
 * @param session    The session handle. If NULL, the function returns NULL.
 *
 * @return a new unconfigured fetch request
 */
DIFFUSION_FETCH_REQUEST_T *diffusion_fetch_request_init(SESSION_T *session);

/**
 * @ingroup PublicAPI_Topics
 *
 * @brief Create a duplicate (copy) of an existing `DIFFUSION_FETCH_REQUEST_T`. `diffusion_fetch_request_free`
 *        should be called on this pointer when no longer needed.
 *
 * @param fetch_request fetch request to be duplicated
 *
 * @return a copy of the provided fetch request. NULL, if the fetch request is NULL.
 */
DIFFUSION_FETCH_REQUEST_T *diffusion_fetch_request_dup(const DIFFUSION_FETCH_REQUEST_T *fetch_request);

/**
 * @ingroup PublicAPI_Topics
 *
 * @brief Free a memory allocated `DIFFUSION_FETCH_REQUEST_T`
 *
 * @param fetch_request The fetch request to be freed
 */
void diffusion_fetch_request_free(DIFFUSION_FETCH_REQUEST_T *fetch_request);

#endif
