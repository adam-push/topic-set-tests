#ifndef _diffusion_messaging_h_
#define _diffusion_messaging_h_ 1

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
 * @file messaging.h
 *
 * Provides a client session with request-response messaging
 * capabilities that can be used to implement application services.
 *
 * <h3>Messaging feature.</h3>
 * Request-response messaging allows a session to send requests to other
 * sessions. Each receiving session provides a corresponding response, which is
 * returned to the sending session. Each request and response carries an
 * application provided value.
 *
 * The method used to send a request determines which sessions will receive it.
 * Each request is routed using the provided <em>message path</em> – an
 * application provided string. Two addressing schemes are provided:
 * <em>unaddressed requests</em> and <em>addressed requests</em>.
 *
 * <h4>Unaddressed requests</h4>
 *
 * A session can provide an application service by implementing a handler and
 * registering it with the server. This is somewhat similar to implementing a
 * REST service, except that interactions between the sender and receiver are
 * asynchronous.
 *
 * Unaddressed requests sent using `send_request` are routed by
 * the server to a handler that has been pre-registered by another session, and
 * matches the message path.
 *
 * Handlers are registered with `addRequestHandler`. Each session may
 * register at most one handler for a given message path. Optionally, one or
 * more session property names can be provided (see `session.h` for a full
 * description of session properties), in which case the values of the session
 * properties for each recipient session will be returned along with its
 * response. To add a request handler, the control client session must have
 * GLOBAL_PERMISSION_REGISTER_HANDLER permission. If
 * registering to receive session property values, the session must also have
 * GLOBAL_PERMISSION_VIEW_SESSION permission.
 *
 * Routing works as follows:
 *
 * <ol>
 * <li>The session sends the request, providing the message path,
 * the request value and data type, and the expected response type.
 * <li>The server uses the message path to apply access control. The sender must
 * have the PATH_PERMISSION_SEND_TO_MESSAGE_HANDLER path permission for
 * the message path, or the request will be rejected.
 * <li>The server uses the message path to select a pre-registered handler and
 * route the request to the appropriate recipient session. The server will
 * consider all registered handlers and select one registered for the most
 * specific path. If multiple sessions have registered a handler registered for
 * a path, one will be chosen arbitrarily. If there is no registered handler
 * matching the message path, the request will be rejected.
 * <li>Otherwise, the server forwards the request to one of the sessions
 * registered to handle the message path. The message path is also passed to the
 * recipient session, providing a hierarchical context.
 * <li>The recipient session processes the request and returns a response to the
 * server, which forwards the response to the sending session.
 * </ol>
 *
 * Registration works across a cluster of servers. If no matching handler is
 * registered on the server to which the sending session is connected, the
 * request will be routed to another server in the cluster that has one.
 *
 * <h4>Addressed requests</h4>
 *
 * Addressed requests provide a way to perform actions on a group of sessions,
 * or to notify sessions of one-off events (for repeating streams of events, use
 * a topic instead).
 *
 * An addressed request can be sent to a set of sessions using
 * `send_request_to_filter`. For the details of session
 * filters, see `session.h`. Sending a request to a filter will match zero
 * or more sessions. Each response received will be passed to the provided
 * callbacks in `SEND_REQUEST_TO_FILTER_PARAMS_T`. As a convenience, an
 * addressed request can be sent a specific session using `send_request_to_session`
 * that accepts a session id.
 *
 * Sending an addressed request requires PATH_PERMISSION_SEND_TO_SESSION permission.
 *
 * If the sending session is connected to a server belonging to a cluster, the
 * recipient sessions can be connected to other servers in the cluster. The
 * filter will be evaluated against all sessions hosted by the cluster.
 *
 * To receive addressed requests, a session must set up a local request stream
 * to handle the specific message path, using `set_request_stream`.
 * When a request is received for the message path, the
 * `DIFFUSION_REQUEST_STREAM_T.request_stream_on_request_cb` method
 * on the stream is triggered.
 * The session should respond using the provided `DIFFUSION_RESPONDER_HANDLE_T`.
 * Streams receive a `DIFFUSION_REQUEST_STREAM_T.request_stream_on_close_cb`
 * callback when unregistered and a `DIFFUSION_REQUEST_STREAM_T.request_stream_on_error_cb`
 * callback if the session is closed.
 *
 * If a request is sent to a session that does not have a matching stream for
 * the message path, an error will be returned to the sending session.
 *
 */

#include "session.h"
#include "misc/deprecate.h"

#include "datatypes.h"
#include "responder.h"
#include "registration.h"
#include "types/messaging_types.h"

/**
 * @brief Context information of the request received.
 * Request path, session ID and session properties
 */
typedef struct DIFFUSION_REQUEST_CONTEXT_T DIFFUSION_REQUEST_CONTEXT_T;

/**
 * @brief Structure containing the information received and passed to
 * a handler registered on a message path, including the message content.
 */
typedef struct svc_send_receiver_client_request_s {
        /// Path from which this message was received.
        char *topic_path;
        /// Content of message.
        CONTENT_T *content;
        /// Session ID of sending client.
        SESSION_ID_T session_id;
        /// Additional options relating to message delivery.
        SEND_OPTIONS_T send_options;
        /// Properties associated with the message handler.
        HASH_T *session_properties;
        /// Conversation ID of sender, if response required.
        CONVERSATION_ID_T conversation_id;
} SVC_SEND_RECEIVER_CLIENT_REQUEST_T;

/**
 * @brief Callback when a handler is registered with the server.
 *
 * @param session The currently active session.
 * @param context User-supplied context from the initial
 *                register_msg_handler() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_msg_receiver_registration_cb)(SESSION_T *session, void *context);

/**
 * @brief Callback when a message is received by a registered handler
 * on a message path.
 *
 * @param session The currently active session.
 * @param request Details about the incoming message, and the message
 *                itself.
 * @param context User-supplied context from the initial
 *                register_msg_handler() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_send_receiver_client_msg_cb)(SESSION_T *session,
                                              const SVC_SEND_RECEIVER_CLIENT_REQUEST_T *request,
                                              void *context);

/**
 * @brief Handlers for notification of registration of handler
 *    and message receipt.
 */
#define MSG_RECEIVER_REGISTRATION_HANDLERS       \
    on_msg_receiver_registration_cb on_registered; \
    on_send_receiver_client_msg_cb on_message;   \
    ERROR_HANDLER_T on_error;           \
    DISCARD_HANDLER_T on_discard;

/**
 * @brief Structure supplied when issuing a register_msg_handler() call.
 */
typedef struct msg_receiver_registration_params_s {
        MSG_RECEIVER_REGISTRATION_HANDLERS
        /// Path used for registration.
        const char *topic_path;
        /// Control group. Leave as NULL to use a default value.
        const char *control_group;
        /// Properties associated with this message handler.
        LIST_T *session_properties;
        /// User-supplied context.
        void *context;
} MSG_RECEIVER_REGISTRATION_PARAMS_T;

/**
 * @brief Callback when a response to a request is received.
 *
 * @param response_datatype Datatype of the response.
 * @param response          Response as a `DIFFUSION_VALUE_T`.
 * @param context           User supplied context passed in from `SEND_REQUEST_TO_SESSION_PARAMS_T`.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*request_on_response_cb)(DIFFUSION_DATATYPE response_datatype,
                                      const DIFFUSION_VALUE_T *response,
                                      void *context);

/**
 * @brief Structure supplied when issuing a send_request_to_session() call
 */
typedef struct send_request_to_session_params_s {
        /// The session id of the session to receive the request.
        SESSION_ID_T *recipient_session;
        /// The request path used by the recipient to select an appropriate handler.
        const char *path;
        /// The request to send
        BUF_T *request;
        /// The datatype of the request
        DIFFUSION_DATATYPE request_datatype;
        /// The datatype of the response to be received
        DIFFUSION_DATATYPE response_datatype;
        /// Callback to handle the response
        request_on_response_cb on_response;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User-supplied context. Can be NULL.
        void *context;
} SEND_REQUEST_TO_SESSION_PARAMS_T;

/**
 * @brief Callback when a request to a request handler has been received.
 *
 * @param session           The currently active session.
 * @param request_datatype  The request datatype.
 * @param request           The request.
 * @param request_context   The request context.
 * @param handle            The responder handle. Used when a response is dispatched with
 *                          `respond_to_request`.
 * @param context           User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*request_handler_on_request)(SESSION_T *session,
                                          DIFFUSION_DATATYPE request_datatype,
                                          const DIFFUSION_VALUE_T *request,
                                          const DIFFUSION_REQUEST_CONTEXT_T *request_context,
                                          const DIFFUSION_RESPONDER_HANDLE_T *handle, void *context);

/**
 * @brief Callback when a request handler is active.
 *
 * @param session             The currently active session.
 * @param path                The path request handler is bound to.
 * @param registered_handler  The registered handler
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*request_handler_on_active)(SESSION_T *session,
                                         const char *path,
                                         const DIFFUSION_REGISTRATION_T *registered_handler);

/**
 * @brief Callback when a request handler is closed.
 */
typedef void (*request_handler_on_close)(void);

/**
 * @brief Callback when a request handler has encountered an error.
 *
 * @param error   The received error.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*request_handler_on_error)(const DIFFUSION_ERROR_T *error);

/**
 * @brief Request handler structure required when creating an `ADD_REQUEST_HANDLER_PARAMS_T`
 */
typedef struct diffusion_request_handler_s {
        /// The datatype of the request
        DIFFUSION_DATATYPE request_datatype;
        /// The datatype of the response to be received
        DIFFUSION_DATATYPE response_datatype;
        /// Callback when a request handler has been registered.
        request_handler_on_active on_active;
        /// Callback when a request has been received by the handler.
        request_handler_on_request on_request;
        /// Callback when an error occurs.
        request_handler_on_error on_error;
        /// Callback when a request handler is closed.
        request_handler_on_close on_close;
} DIFFUSION_REQUEST_HANDLER_T;

/**
 * @brief Structure supplied when issuing a add_request_handler() call.
 */
typedef struct add_request_handler_params_s {
        /// The request path to set a handler to.
        const char *path;
        /// Request handler
        DIFFUSION_REQUEST_HANDLER_T *request_handler;
        /// The session properties
        char **session_properties;
        // Callback if an error occurs during request handler
        // registration or deregistration. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} ADD_REQUEST_HANDLER_PARAMS_T;

/**
 * @brief Callback when a filtered request has been dispatched to a number
 * of sessions
 *
 * @param number_sent The number of requests sent to sessions.
 * @param context     User supplied context passed in from `SEND_REQUEST_TO_FILTER_PARAMS_T`.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*filtered_request_number_sent)(int number_sent, void *context);

/**
 * @brief Callback when a response to a filtered request has been received.
 *
 * @param response_datatype Datatype of the response.
 * @param response          Response as a `DIFFUSION_VALUE_T`.
 * @param context           User supplied context passed in from `SEND_REQUEST_TO_FILTER_PARAMS_T`.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*filtered_request_on_response)(DIFFUSION_DATATYPE response_datatype,
                                            const DIFFUSION_VALUE_T *response,
                                            void *context);

/**
 * @brief Callback when a response results in an error.
 *
 * @param session_id  The session id of the session which responded with
 *                    an error.
 * @param error       The response error. The context will be the user supplied context passed in from
 *                    `SEND_REQUEST_TO_FILTER_PARAMS_T`.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*filtered_request_on_response_error)(const SESSION_ID_T *session_id,
                                                  const DIFFUSION_ERROR_T *error);

/**
 * Structure supplied when issuing a send_request_to_filter() call.
 */
typedef struct send_request_to_filter_params_s {
        /// The request path to send the request to.
        const char *path;
        /// The session filter expression.
        const char *filter;
        /// The datatype of the request
        DIFFUSION_DATATYPE request_datatype;
        /// The datatype of the response to be received
        DIFFUSION_DATATYPE response_datatype;
        /// Request to send
        BUF_T *request;
        /// Callback when the request has been dispatched to
        /// all matching sessions.
        filtered_request_number_sent on_number_sent;
        /// Callback when a response is received
        filtered_request_on_response on_response;
        /// Callback when an error response is received
        filtered_request_on_response_error on_response_error;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} SEND_REQUEST_TO_FILTER_PARAMS_T;

/**
 * @ingroup PublicAPI_Messaging
 *
 * @brief Return a copy of the request context's session ID. `session_id_free` should be
 * called on this pointer when no longer needed.
 *
 * @param request_context the request context to retrieve the session ID from.
 *
 * @return a copy of the request context's session ID. NULL, if `request_context` is NULL.
 */
SESSION_ID_T *diffusion_request_context_get_session_id(const DIFFUSION_REQUEST_CONTEXT_T *request_context);

/**
 * @ingroup PublicAPI_Messaging
 *
 * @brief Return a copy of the request context's request path. `free` should be called on
 * this pointer when no longer needed.
 *
 * @param request_context the request context to retrieve the request path from.
 *
 * @return a copy of the request context's request path. NULL, if `request_context` is NULL.
 */
char *diffusion_request_context_get_path(const DIFFUSION_REQUEST_CONTEXT_T *request_context);

/**
 * @ingroup PublicAPI_Messaging
 *
 * @brief Return a copy of the request context's session properties. `hash_free` should be called
 * on this pointer when no longer needed.
 *
 * @param request_context the request context to retrieve session properties from.
 *
 * @return a copy of the request context's session properties. NULL, if `request_context` is NULL.
 */
HASH_T *diffusion_request_context_get_session_properties(const DIFFUSION_REQUEST_CONTEXT_T *request_context);

/**
 * @ingroup PublicAPI_Messaging
 *
 * @brief Send a request to a specific session.
 *
 * @param session   The session handle. If NULL, this function returns immediately.
 * @param params    Parameters defining the request to send a request to a specific
 *                  session.
 */
void send_request_to_session(SESSION_T *session, SEND_REQUEST_TO_SESSION_PARAMS_T params);

/**
 * @brief Send a request via a filter.
 *
 * @param session   The session handle. If NULL, this function returns immediately.
 * @param params    Parameters defining the request to send a request via a filter.
 */
void send_request_to_filter(SESSION_T *session, SEND_REQUEST_TO_FILTER_PARAMS_T params);

/**
 * @ingroup PublicAPI_Messaging
 *
 * @brief Add a server registered request handler
 *
 * @param session   The session handle. If NULL, this function returns immediately.
 * @param params    Parameters defining the request handler to be added.
 */
void add_request_handler(SESSION_T *session, ADD_REQUEST_HANDLER_PARAMS_T params);

/**
 * @brief Structure supplied to a send_request() call.
 */
typedef struct send_request_params_s {
        /// Path to send the request to
        const char *path;
        /// Request to be sent
        BUF_T *request;
        /// The datatype of the request
        DIFFUSION_DATATYPE request_datatype;
        /// The datatype of the response to be received
        DIFFUSION_DATATYPE response_datatype;
        /// Callback to handle the response
        request_on_response_cb on_response;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User-supplied context. Can be NULL.
        void *context;
} SEND_REQUEST_PARAMS_T;

/**
 * @brief Callback for a request stream when a request has been received.
 *
 * @param session          The session.
 * @param path             Path the request was received on.
 * @param request_datatype The request's datatype.
 * @param request          The request.
 * @param responder_handle The responder handle used when dispatching a response with
 *                         `respond_to_request`. If the responder handle is required
 *                         outside the scope of this function, use `responder_handle_dup`
 *                         to copy it and use where necessary. This can be freed with
 *                         `responder_handle_free`.
 * @param context          User supplied context passed in from the request stream.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*request_stream_on_request_cb)(SESSION_T *session, const char *path, DIFFUSION_DATATYPE request_datatype,
                                            const DIFFUSION_VALUE_T *request, const DIFFUSION_RESPONDER_HANDLE_T *responder_handle, void *context);

/**
 * @brief Callback when a request stream encounters an error.
 *
 * @param error The error received on the request stream.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*request_stream_on_error_cb)(const DIFFUSION_ERROR_T *error);

/**
 * @brief Callback for a request stream that has been closed.
 */
typedef void (*request_stream_on_close_cb)(void);

/**
 * @brief Structure supplied when adding a request stream.
 */
typedef struct diffusion_request_stream_s {
        /// Callback on receiving a request
        request_stream_on_request_cb on_request;
        /// Callback when an error is encountered
        request_stream_on_error_cb on_error;
        /// Callback when a request stream is closed
        request_stream_on_close_cb on_close;
        /// User supplied context.
        void *context;
} DIFFUSION_REQUEST_STREAM_T;

/**
 * @ingroup PublicAPI_Messaging
 *
 * @brief Send a request to a path.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters defining the request to be sent.
 */
void send_request(SESSION_T *session, SEND_REQUEST_PARAMS_T params);

/**
 * @ingroup PublicAPI_Messaging
 *
 * @brief Set a request stream to handle requests to a specified path.
 *
 * @param session           The session handle. If NULL, this function returns immediately.
 * @param path              Path to receive requests on.
 * @param request_datatype  The request datatype.
 * @param response_datatype The response datatype.
 * @param request_stream    Request stream to handle requests to this path
 *
 * @return the current REQUEST_STREAM_T at this path. NULL, if this is the
 *         first stream to be set to the path.
 */
DIFFUSION_REQUEST_STREAM_T *set_request_stream(SESSION_T *session, const char *path,
                        DIFFUSION_DATATYPE request_datatype, DIFFUSION_DATATYPE response_datatype, const DIFFUSION_REQUEST_STREAM_T *request_stream);

/**
 * @ingroup PublicAPI_Messaging
 *
 * @brief Remove a request stream bound to a specified path.
 *
 * @param session      The session handle. If NULL, this function returns immediately.
 * @param request_path The path at which to remove the request stream. If NULL,
 *                     this function returns immediately.
 *
 * @return the request stream that was removed. NULL, if there is no request stream
 *         bound to this path.
 */
DIFFUSION_REQUEST_STREAM_T *remove_request_stream(SESSION_T *session, const char *request_path);

#endif
