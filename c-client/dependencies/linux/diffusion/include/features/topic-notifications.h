#ifndef _diffusion_topic_notifications_h_
#define _diffusion_topic_notifications_h_ 1

/*
 * Copyright © 2019 - 2022 Push Technology Ltd., All Rights Reserved.
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
 * @file topic-notifications.h
 *
 * Allows a client session to receive notifications about changes
 * to selected topics.
 *
 * Sessions receive notifications via `DIFFUSION_TOPIC_NOTIFICATION_LISTENER_T`s
 * The listener will be provided with the topic specifications for all topics
 * bound to paths that match registered selectors, and any subsequent notifications
 * for the selected topics on those paths, via
 * `DIFFUSION_TOPIC_NOTIFICATION_LISTENER_T.on_topic_notification`.
 * Notifications will only be emitted for paths where a topic is bound.
 *
 * For example, with a registered selector `"?a//"`, if a topic is added
 * at path `a/b/c/d` with no topics bound to paths higher in the hierarchy
 * `DIFFUSION_TOPIC_NOTIFICATION_LISTENER_T.on_topic_notification` will be called
 * once with a topic path of `"a/b/c/d"`, a notification type of `TOPIC_ADDED`, and
 * the topic's associated topic specification.
 *
 * The nature of the notification is provided by the`DIFFUSION_TOPIC_NOTIFICATION_TYPE_T`
 * enum. `TOPIC_ADDED` and `TOPIC_REMOVED` represent structural changes to the topic tree;
 * `TOPIC_SELECTED` indicates that a pre-existing topic has been selected by a new
 * registered selector, and similarly `TOPIC_DESELECTED` indicates that a topic is no
 * longer selected because of changes to the set of registered selectors for the listener.
 *
 * Registered `DIFFUSION_TOPIC_NOTIFICATION_LISTENER_T`s  will receive notifications
 * for all topics matching registered selections. Selection of topics using
 * topic selector expressions is provided via the
 * `DIFFUSION_REGISTRATION_T` associated for a specific listener.
 *
 * A session can request selections at any time, even if the topics do not exist
 * at the server. Selections are stored on the server and any subsequently added
 * topics that match registered selectors will generate notifications.
 *
 * Listeners will be informed about the presence or absence of unselected immediate
 * descendants via `DIFFUSION_TOPIC_NOTIFICATION_LISTENER_T.on_descendant_notification`.
 * This allows listeners to determine whether to select deeper topic paths
 * in order to walk the topic tree. An immediate descendant is defined as the first
 * bound topic on any branch below a given topic path.
 *
 * For example, for topics at `"a/b", "a/c", "a/c/d", "a/e/f/g"`, the
 * immediate descendants of `"a"` would be `"a/b", "a/c", "a/e/f/g"`.
 *
 * Immediate descendant notifications provide a `DIFFUSION_TOPIC_NOTIFICATION_TYPE_T` to
 * indicate the reason for the notification in the same manner as
 * `DIFFUSION_TOPIC_NOTIFICATION_LISTENER_T.on_topic_notification`
 *
 * For example, with a registered selector `">a"`, if a topic is added at
 * path `a/b` then `DIFFUSION_TOPIC_NOTIFICATION_LISTENER_T.on_descendant_notification`
 * will be called with a topic path of `"a/b"` and a notification type of
 * `TOPIC_ADDED`. If a topic was subsequently added at path `a/b/c`, no further
 * notifications will be received until `diffusion_topic_notification_registration_select`
 * was used to select the deeper topic path `">a/b"`.
 *
 * A listener will only be notified about topics for which the session
 * has `PATH_PERMISSION_SELECT_TOPIC` and `PATH_PERMISSION_READ_TOPIC` permissions.
 * `PATH_PERMISSION_SELECT_TOPIC` determines which selectors a listener may register;
 * `PATH_PERMISSION_READ_TOPIC` determines which selected topics the client may receive
 * notifications for.
 */

#include "session.h"
#include "topic-specification.h"
#include "topic-notification-registration.h"

typedef enum {
        /**
         * The topic has been added.
         */
        TOPIC_ADDED,

        /**
         * The topic existed at the time of the selector registration.
         */
        TOPIC_SELECTED,

        /**
         * The topic has been removed.
         */
        TOPIC_REMOVED,

        /**
         * The topic is no longer selected due to the removal of a selector.
         */
        TOPIC_DESELECTED
} DIFFUSION_TOPIC_NOTIFICATION_TYPE_T;

/**
 * @brief Callback when the listener has successfully been registered.
 *
 * @param registration The registration for topic notifications.
 * @param context      User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_listener_registered_cb)(const DIFFUSION_REGISTRATION_T *registration, void *context);

/**
 * @brief Notification for an immediate descendant of a selected topic path.
 * This notifies the presence or absence of a descendant topic that may
 * subsequently be explicitly selected.
 *
 * @param topic_path The path of the immediate descendant that is not selected.
 * @param type       The type of notification.
 * @param context    User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_descendant_notification_cb)(const char *topic_path, const DIFFUSION_TOPIC_NOTIFICATION_TYPE_T type, void *context);

/**
 * @brief A notification for a selected topic.
 *
 * @param topic_path    The path of the topic that this notification is for.
 * @param specification The specification of the topic that this notification is for
 * @param type          The type of notification.
 * @param context       User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_notification_cb)(const char *topic_path, const TOPIC_SPECIFICATION_T *specification, const DIFFUSION_TOPIC_NOTIFICATION_TYPE_T type, void *context);

/**
 * @brief Callback for when a topic notification listener has been closed.
 */
typedef void (*on_topic_notification_listener_close_cb)(void);

/**
 * Structure for adding a topic notification listener.
 */
typedef struct diffusion_topic_notification_listener_s {
        /// Callback when a topic notification listener
        /// has been registered.
        on_listener_registered_cb on_registered;
        /// Callback topic notification for an immediate
        /// descendant of a selected topic path.
        on_descendant_notification_cb on_descendant_notification;
        /// Callback topic notification for a selected topic.
        on_topic_notification_cb on_topic_notification;
        /// Callback when a topic notification listener
        /// has been closed.
        on_topic_notification_listener_close_cb on_close;
        /// Callback to handle errors from the server. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_TOPIC_NOTIFICATION_LISTENER_T;

/**
 * @ingroup PublicAPI_TopicNotifications
 *
 * @brief Register a listener to receive topic notifications.
 *
 * @param session   The current session. If NULL, this function returns immediately.
 * @param listener  The listener to receive topic specification notifications
 * @param api_error Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_topic_notification_add_listener(SESSION_T *session, const DIFFUSION_TOPIC_NOTIFICATION_LISTENER_T listener, DIFFUSION_API_ERROR *api_error);

#endif
