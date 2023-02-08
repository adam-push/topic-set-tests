#ifndef _diffusion_topic_control_h_
#define _diffusion_topic_control_h_ 1

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
 * @file topic-control.h
 *
 * Allows the creation and removal of topics and notification of missing topics.
 */

#include "session.h"

#include "misc/deprecate.h"
#include "types/content_types.h"
#include "types/topic_types.h"
#include "topic-specification.h"
#include "hash.h"

typedef enum {
        /// No error while adding topic.
        ADD_TOPIC_FAILURE_REASON_SUCCESS = 0,
        /// Topic already existed with exactly the same details.
        ADD_TOPIC_FAILURE_REASON_EXISTS = 1,
        /// Topic already exists with the same name but different
        /// details.
        ADD_TOPIC_FAILURE_REASON_EXISTS_MISMATCH =  2,
        /// The name of the topic was found to be invalid.
        ADD_TOPIC_FAILURE_REASON_INVALID_NAME =  3,
        /// Some aspect of the details failed validation.
        ADD_TOPIC_FAILURE_REASON_INVALID_DETAILS =  4,
        /// @deprecated This failure reason is deprecated from version 6.2
        /// onwards and will be removed in a future release.
        ///
        /// A user supplied class could either not be found or could
        /// not be instantiated at the server.
        ADD_TOPIC_FAILURE_REASON_USER_CODE_ERROR =  5,
        /// The client was denied permission to add a topic of the
        /// specified type a the given point in the topic hierarchy.
        ADD_TOPIC_FAILURE_REASON_PERMISSIONS_FAILURE =  7,
        /// An unexpected error occurred whilst attempting to create
        /// the topic.
        ADD_TOPIC_FAILURE_REASON_UNEXPECTED_ERROR =  9
} SVC_ADD_TOPIC_FAILURE_REASON_T;

typedef enum {
        // Result code denoting the topic was
        // sucessfully added.
        TOPIC_ADD_RESULT_CREATED = 0,
        // Result code denoting the topic
        // already exists.
        TOPIC_ADD_RESULT_EXISTS = 1
} TOPIC_ADD_RESULT_CODE;

typedef enum {
        /// A topic already exists with the same path but a different specification.
        TOPIC_ADD_FAIL_EXISTS_MISMATCH = 1,
        /// The supplied topic path is invalid.
        TOPIC_ADD_FAIL_INVALID_NAME = 2,
        /// Some aspect of the topic specification failed validation.
        TOPIC_ADD_FAIL_INVALID_DETAILS = 3,
        /// The client was denied permission to add a topic of the specified type at
        /// the given point in the topic hierarchy.
        TOPIC_ADD_FAIL_PERMISSIONS_FAILURE = 4,
        /// An unexpected error occurred whilst attempting to create the topic. See
        /// the server log for more details.
        TOPIC_ADD_FAIL_UNEXPECTED_ERROR = 5,
        /// Adding the topic failed because the cluster was repartitioning due to a
        /// server starting, stopping, or failing. The operation can be retried.
        TOPIC_ADD_FAIL_CLUSTER_REPARTITION = 6,
        /// Adding the topic failed because of a license limit.
        TOPIC_ADD_FAIL_EXCEEDED_LICENSE_LIMIT = 7,
        /// @deprecated This failure reason is deprecated and unused from version 6.5.
        /// The value will be removed in a future release.
        TOPIC_ADD_FAIL_INCOMPATIBLE_PARENT = 8,
        /// Adding the topic failed because a topic is already bound to the specified
        /// path but the caller does not have the rights to manage it.
        TOPIC_ADD_FAIL_EXISTS_INCOMPATIBLE = 10
} TOPIC_ADD_FAIL_RESULT_CODE;

/**
 * @brief Callback for add_topic_from_specification().
 *
 * @param session     The current active session.
 * @param result_code The result code from the topic add request.
 * @param context     User-supplied context from the initial
 *                    add_topic_from_specification() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_add_cb)(SESSION_T *session,
                               TOPIC_ADD_RESULT_CODE result_code,
                               void *context);

/**
 * @brief Callback for add_topic_from_specification().
 *
 * @param session     The current active session.
 * @param result_code The topic add failure result code
 * @param error       The error reason. This will only be set
 *                    if this function is called through the
 *                    callback
 * @param context     User-supplied context from the initial
 *                    add_topic_from_specification() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_add_failed_cb)(SESSION_T *session,
                                      TOPIC_ADD_FAIL_RESULT_CODE result_code,
                                      const DIFFUSION_ERROR_T *error,
                                      void *context);

/**
 * add_topic_from_specification callback
 */
typedef struct add_topic_callback_s {
        /**
         * Topic added successfully (or already exists).
         */
        on_topic_add_cb on_topic_added_with_specification;
        /**
         * Topic failed to be added.
         */
        on_topic_add_failed_cb on_topic_add_failed_with_specification;
        /**
         * Standard service error handler.
         */
        ERROR_HANDLER_T on_error;
        /**
         * Standard conversation discard callback.
         */
        DISCARD_HANDLER_T on_discard;
        /**
         * Context. Can be NULL.
         */
        void *context;
} ADD_TOPIC_CALLBACK_T;

/**
 * An opaque topic removal result struct.
 */
typedef struct DIFFUSION_TOPIC_REMOVAL_RESULT_T DIFFUSION_TOPIC_REMOVAL_RESULT_T;

/**
 * @brief The number of topics removed in the topic removal result.
 *
 * @param topic_removal_result Topic removal result to query
 *
 * @return the number of topics removed. -1 is returned if the topic removal result is `NULL`.
 */
int diffusion_topic_removal_result_removed_count(const DIFFUSION_TOPIC_REMOVAL_RESULT_T *topic_removal_result);


/**
 * @brief Callback for topic_removal() handlers.
 *
 * @param session The current active session.
 * @param result The response message from Diffusion.
 * @param context User-supplied context from the topic_removal() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_removal_cb)(SESSION_T *session,
                                   const DIFFUSION_TOPIC_REMOVAL_RESULT_T *result,
                                   void *context);

/**
 * @brief User-specified callbacks that may be invoked in response to
 * issuing a "topic removal" request to Diffusion.
 */
#define TOPIC_REMOVAL_HANDLERS \
        /** Topic removed. */                                           \
        on_topic_removal_cb on_removed;                                 \
        /** Standard service error handler. */                          \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard handler. */                        \
        DISCARD_HANDLER_T on_discard;

typedef struct topic_removal_handlers_s {
        TOPIC_REMOVAL_HANDLERS
} TOPIC_REMOVAL_HANDLERS_T;

/**
 * @brief Structure passed to topic_removal().
 */
typedef struct topic_removal_params_s {
        TOPIC_REMOVAL_HANDLERS
        /// Selector describing topics to remove.
        const char *topic_selector;
        /// User-supplied context returned to callbacks.
        void *context;
} TOPIC_REMOVAL_PARAMS_T;

/**
 * @brief Structure of a request to register for missing topic notifications.
 *
 * Notification that a session has made a subscription request using a
 * selector that does not match any topics.
 */
typedef struct svc_missing_topic_request_s {
        /// The session id of the client making the request for the topic.
        SESSION_ID_T *session_id;
        /// The session properties of the client making the request for the topic.
        HASH_T *session_properties;
        /// Register for notifications of missing topics that match
        /// this topic selector.
        char *topic_selector;
        /// List of the names of the server through which the
        /// notification has been routed.
        /// The first name in the list will be the name of the server to which
        /// the originating session was connected. If the notification was routed
        /// through remote server connections before reaching the recipient then
        /// those servers will also be listed in the order that the notification
        /// passed through them.
        LIST_T *server_names;
        /// The conversation ID to use in the response message.
        CONVERSATION_ID_T *conversation_id;
} SVC_MISSING_TOPIC_REQUEST_T;

/**
 * @brief Callback for missing_topic_register_handler().
 *
 * Called when a session subscribes using a topic selector that matches
 * no existing topics.
 *
 * @param session The current active session.
 * @param request The incoming notification message.
 * @param context User-supplied context from the initial registration call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_missing_topic_cb)(SESSION_T *session,
                                   const SVC_MISSING_TOPIC_REQUEST_T *request,
                                   void *context);

/**
 * @brief Callback handlers for missing topic notification messages.
 */
#define MISSING_TOPIC_HANDLERS                                          \
        /** Callback when a client requests a topic that doesn't exist. */ \
        on_missing_topic_cb on_missing_topic;                           \
        /** Standard service error handler. */                          \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard handler. */                        \
        DISCARD_HANDLER_T on_discard;

typedef struct missing_topic_handlers_s {
        MISSING_TOPIC_HANDLERS
} MISSING_TOPIC_HANDLERS_T;

/**
 * @brief Structure supplied when registering to receive missing topic
 * notifications.
 */
typedef struct missing_topic_params_s {
        MISSING_TOPIC_HANDLERS
        /// Receive notifications for missing topics under this topic.
        const char *topic_path;
        /// User-supplied context return to callbacks.
        void *context;
} MISSING_TOPIC_PARAMS_T;

/**
 * @ingroup PublicAPI_TopicControl
 *
 * @brief Add a topic to Diffusion.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param topic_path    The topic path to be added.
 * @param specification The topic specification
 * @param callback      Struct containing pointers to callback functions will be called when
 *                      a response to the request is received from the server.
 */
void add_topic_from_specification(SESSION_T *session,
                                  const char *topic_path,
                                  const TOPIC_SPECIFICATION_T *specification,
                                  const ADD_TOPIC_CALLBACK_T callback);

/**
 * @ingroup PublicAPI_TopicControl
 *
 * @brief Remove topics from Diffusion.
 *
 * Send a request to remove one or more topics at the server.  The topics to
 * remove will depend upon the nature of the topic selector specified. If the
 * selector does not have descendant pattern qualifiers (i.e. / or //), only
 * those topics that exist at paths indicated by the selector will be removed
 * and not their descendants. If a single / qualifier is specified, all
 * descendants of the matching topic paths will be removed. If // is
 * specified, all branches of the topic tree that match the selector (i.e
 * topics at the selected paths and all descendants of the selected paths)
 * will be removed.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing which topics to remove
 *                      and callbacks to handle success or failure
 *                      notifications.
 */
void topic_removal(SESSION_T *session, const TOPIC_REMOVAL_PARAMS_T params);

/**
 * @ingroup PublicAPI_TopicControl
 *
 * @brief Register to receive notifications about missing topics that
 * clients attempt to subscribe.
 *
 * Register a handler for receiving missing topic notifications when a client
 * attempts to subscribe to a topic underneath a given root topic, but
 * that requested topic doesn't exist.
 *
 * @param session       The current session. If NULL, this function returns immediately
 *                      with the value NULL.
 * @param params        Parameters defining the registration request.
 * @return              A conversation id that can be used to deregister the
 *                      handler at a later time, or NULL if the supplied session is NULL.
 */
CONVERSATION_ID_T *missing_topic_register_handler(SESSION_T *session, const MISSING_TOPIC_PARAMS_T params);

/**
 * @ingroup PublicAPI_TopicControl
 *
 * @brief Cease receiving missing topic notifications.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param conversation_id       The conversation id returned from a previous
 *                              call to missing_topic_register_handler().
 */
void missing_topic_deregister_handler(SESSION_T *session,
                                      const CONVERSATION_ID_T *conversation_id);

/**
 * @brief Proceed (retry) after a processing a missing topic notification.
 *
 * This method is a no-op. In previous releases this would cause the
 * selector to be added to the sessions's selections and the selection
 * to be re-evaluated.
 * Since 6.6, the selector is always added to the session's selections
 * before this notification is issued.
 *
 * @param session       The current session.
 * @param request       The request which gave rise to the missing topic
 *                      notification.
 * @deprecated since 6.6
 */
DEPRECATED(void missing_topic_proceed(SESSION_T *session,
                           SVC_MISSING_TOPIC_REQUEST_T *request));

#endif
