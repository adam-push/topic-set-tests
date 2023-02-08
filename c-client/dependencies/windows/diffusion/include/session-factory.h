#ifndef _diffusion_session_factory_h_
#define _diffusion_session_factory_h_ 1

/*
 * Copyright © 2018 - 2022 Push Technology Ltd., All Rights Reserved.
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
 * @file session-factory.h
 *
 * Functions relating to a session and session management.
 */

#include <stdlib.h>
#include <stdbool.h>

#include "buf.h"
#include "hash.h"
#include "retry-strategy.h"
#include "types/session_types.h"


/**
 * An opaque diffusion session factory struct.
 */
typedef struct DIFFUSION_SESSION_FACTORY_T DIFFUSION_SESSION_FACTORY_T;


/**
 * @brief Create a memory allocated `DIFFUSION_SESSION_FACTORY_T`. `diffusion_session_factory_free`
 *        should be called on the pointer when no longer needed.
 *
 * @return  a diffusion session factory
 */
DIFFUSION_SESSION_FACTORY_T *diffusion_session_factory_init(void);


/**
 * @brief Nominates a listener for session events from a session.
 *
 * If not specified, there will be no session listener.
 *
 * @param session_factory the session factory to mutate. If NULL, this function is a no-op.
 * @param listener        specifies a listener to be used for subsequent sessions
 *                        that are created. NULL may be specified to remove any previously
 *                        specified listener
 */
void diffusion_session_factory_session_listener(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        SESSION_LISTENER_T *listener);


/**
 * @brief Sets the user-supplied context.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param user_context      User-supplied context for this session. It is the user's responsibility
 *                          to free any memory that this points to when the session is no longer required.
 */
void diffusion_session_factory_user_context(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        void *user_context);


/**
 * @brief Sets the security principal.
 *
 * By default this will be an anonymous principal.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param principal         the principal
 */
void diffusion_session_factory_principal(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        const char *principal);


/**
 * @brief Set credentials to `credentials_create_none()`
 *
 * This is the default.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 */
void diffusion_session_factory_no_credentials(
        const DIFFUSION_SESSION_FACTORY_T *session_factory);


/**
 * @brief Set credentials.
 *
 * The default is `credentials_create_none`©
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param credentials       the credentials. If NULL, this function is a no-op.
 */
void diffusion_session_factory_credentials(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        const CREDENTIALS_T *credentials);


/**
 * @brief Set credentials of type `PLAIN_PASSWORD`.
 *
 * The default is `NONE`.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param password          the password. If NULL, this function is a no-op.
 */
void diffusion_session_factory_password(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        const char *password);


/**
 * @brief Set credentials of type `CUSTOM`.
 *
 * The default is `NONE`.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param bytes             the credentials as bytes. If NULL, this function is a no-op.
 */
void diffusion_session_factory_custom_credentials(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        const BUF_T *bytes);


/**
 * @brief Sets the connection timeout.
 *
 * This constrains the time taken to establish an initial connection
 * to the server. The write timeout} constrains the time to send the
 * connection request. The server is responsible for limiting the overall
 * time taken to complete a connection once it has received the
 * request.
 *
 * The default is `DIFFUSION_DEFAULT_CONNECTION_TIMEOUT`
 *
 * @param session_factory       the session factory to mutate. If NULL, this function is a no-op.
 * @param connection_timeout    the default connection timeout in milliseconds. If this
 *                              exceeds one hour (3600000ms) a warning will be logged and the
 *                              time-out will be set to one hour. If this value is 0, this
 *                              function is a no-op.
 */
void diffusion_session_factory_connection_timeout(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        uint32_t connection_timeout);


/**
 * @brief Disable reconnection. Sessions will not attempt to reconnect in the event
 * of unexpected connection loss.
 *
 * This is equivalent to calling `diffusion_session_factory_reconnection_timeout` with a
 * timeout of 0
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 */
void diffusion_session_factory_no_reconnection(
        const DIFFUSION_SESSION_FACTORY_T *session_factory);


/**
 * @brief Sets the reconnection timeout. This determines the duration that sessions
 * will attempt to reconnect for, in the event of unexpected connection
 * loss.
 *
 * The default is `DIFFUSION_DEFAULT_RECONNECTION_TIMEOUT`.
 *
 * A timeout value of 0 or less is equivalent to to calling
 * `diffusion_session_factory_no_reconnection`.
 *
 * @param session_factory       the session factory to mutate. If NULL, this function is a no-op.
 * @param reconnection_timeout  the timeout duration to use when attempting to reconnect,
 *                              in milliseconds
 */
void diffusion_session_factory_reconnection_timeout(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        int reconnection_timeout);


/**
 * @brief Sets the reconnection strategy. If reconnection is enabled, this strategy
 * will be called in the event of an unexpected connection loss.
 *
 * If the reconnection timeout has been set, but not a strategy, sessions
 * will use a default strategy that attempts to reconnect on a regular basis
 * of 5 seconds until the reconnection timeout is exceeded.
 *
 * @param session_factory       the session factory to mutate. If NULL, this function is a no-op.
 * @param reconnection_strategy the reconnection strategy to use
 */
void diffusion_session_factory_reconnection_strategy(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        RECONNECTION_STRATEGY_T *reconnection_strategy);


/**
 * @brief Sets the initial retry strategy.
 *
 * The strategy will determine whether a failure to open a session due to a transient exception
 * should be retried and if so, at what interval and for how many attempts.
 *
 * If no initial retry strategy is set, there will be no attempt to retry after such failure.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param strategy          the retry strategy to use.
 */
void diffusion_session_factory_initial_retry_strategy(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        DIFFUSION_RETRY_STRATEGY_T *strategy);


/**
 * @brief Sets the input buffer size for socket connection buffers and message
 * receiving buffers.
 *
 * This controls both how much pending data the socket can accommodate and
 * how much available data will be read by the input tasks. In general
 * setting both this and `diffusion_session_factory_output_buffer_size` to the same
 * reasonable size will result in higher network throughput. The cost of the
 * receiving buffer is only incurred when actually reading data for
 * processing. The cost of the socket buffer is always incurred and may be
 * modified by the operating system.
 *
 * The default is `DIFFUSION_DEFAULT_INPUT_BUFFER_SIZE`.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param input_buffer_size input buffer size in bytes. This may not be less than `DIFFUSION_MAXIMUM_MESSAGE_SIZE_MINIMUM`,
 *                          if so, this function is a no-op.
 */
void diffusion_session_factory_input_buffer_size(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        uint32_t input_buffer_size);


/**
 * @brief Sets the output buffer size for socket connection buffers and message
 * sending buffers.
 *
 * This controls both how much pending data the socket can accommodate and
 * how much data will be batched by sending messages. In general setting
 * both this and `diffusion_session_factory_input_buffer_size` to the same reasonable size
 * will result in higher network throughput. The cost of the sending buffer
 * is only incurred when actually queuing data for output. The cost of the
 * socket buffer is always incurred and may be modified by the operating
 * system.
 *
 * The default is `DIFFUSION_DEFAULT_OUTPUT_BUFFER_SIZE`.
 *
 * @param session_factory       the session factory to mutate. If NULL, this function is a no-op.
 * @param output_buffer_size    output buffer size in bytes. This may not be less than `DIFFUSION_MAXIMUM_MESSAGE_SIZE_MINIMUM`,
 *                              if so, this function is a no-op.
 */
void diffusion_session_factory_output_buffer_size(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        uint32_t output_buffer_size);


/**
 * @brief Sets the write timeout value for blocking writes.
 *
 * Blocking writes are only used for the initial connection request. If it
 * is not possible to complete the write within this time, the connection is
 * closed.
 *
 * The default is `DIFFUSION_DEFAULT_WRITE_TIMEOUT`.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param write_timeout     the write timeout in milliseconds. If this exceeds one
 *                          hour (3600000ms) a warning will be logged and the time-out will be
 *                          set to one hour. If this value is 0, this function is a no-op.
 */
void diffusion_session_factory_write_timeout(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        uint32_t write_timeout);


/**
 * @brief Set the maximum message size.
 *
 * This constrains the size of messages that will be accepted from the
 * server and thus the size of any content that can be received. The limit
 * is simply to protect against unexpectedly large messages.
 *
 * By default, the size of received messages is unconstrained – see
 * `DIFFUSION_DEFAULT_MAXIMUM_MESSAGE_SIZE`.
 *
 * @param session_factory       the session factory to mutate. If NULL, this function is a no-op.
 * @param maximum_message_size  the maximum message size in bytes. This must not be less than
 *                              `DIFFUSION_MAXIMUM_MESSAGE_SIZE_MINIMUM`.
 */
void diffusion_session_factory_maximum_message_size(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        uint32_t maximum_message_size);


/**
 * @brief Set the host name of the server to connect the session to.
 *
 * This value is only used if a URL is not provided when opening a session.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param server_host       the host name of the server
 */
void diffusion_session_factory_server_host(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        const char *server_host);


/**
 * @brief Set the port of the server to connect the session to.
 *
 * This value is only used if a URL is not provided when opening a session.
 * If the port is not set using this method or a URL, the port will be
 * inferred based on the transport and security configuration.
 *
 * The provided value must be within the range used for port numbers.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param server_port       the port of the server
 */
void diffusion_session_factory_server_port(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        int server_port);


/**
 * @brief Set if a secure transport should be used.
 *
 * This value is only used if a URL is not provided when opening a session.
 * Enables or disables the use of transport layer security.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param secure_transport  if transport layer security is enabled
 */
void diffusion_session_factory_secure_transport(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        bool secure_transport);


/**
 * @brief Set the path used for HTTP requests.
 *
 * This value is only used if a URL is not provided when opening a session.
 * The path must start with a '/' and end with '/diffusion'.
 *
 * The default is `DIFFUSION_DEFAULT_REQUEST_PATH`.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param request_path      the path used for HTTP requests
 */
void diffusion_session_factory_request_path(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        const char *request_path);


/**
 * @brief Set the recovery buffer size.
 *
 * If the server is configured to support reconnection, a session
 * established with a non-zero reconnection time retains
 * a buffer of sent messages. If the session disconnects and
 * reconnects, this buffer is used to re-send messages that the
 * server has not received.
 *
 * The default is `DIFFUSION_DEFAULT_RECOVERY_BUFFER_SIZE`.
 * Higher values increase the chance of successful reconnection, but
 * increase the per-session memory footprint.
 *
 * @param session_factory       the session factory to mutate. If NULL, this function is a no-op.
 * @param recovery_buffer_size  the recovery buffer size in messages; can be 0
 */
void diffusion_session_factory_recovery_buffer_size(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        uint32_t recovery_buffer_size);


/**
 * @brief Sets the maximum size of the outbound message queue for the connection.
 *
 * The outbound message queue should be large enough to accommodate all the
 * messages sent to the server. This would include topic updates, messages
 * sent using messaging and service requests such as registering
 * a handler.
 *
 * It may be necessary to increase this value for applications that send
 * messages in bursts, or continue to send messages when a session is
 * disconnected and reconnecting. Larger values allow more messages to be
 * queued, and increase the memory footprint of the session.
 *
 * If the outbound message queue fills, sending a message will cause the
 * session to close with an error.
 *
 * The default is `DIFFUSION_DEFAULT_MAXIMUM_QUEUE_SIZE`.
 *
 * @param session_factory       the session factory to mutate. If NULL, this function is a no-op.
 * @param maximum_queue_size    a positive integer indicating the maximum number of messages
 *                              that may be queued
 */
void diffusion_session_factory_maximum_queue_size(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        uint32_t maximum_queue_size);


/**
 * @brief Sets a user-defined session property value.
 *
 * Supplied session properties will be provided to the server when a session
 * is created using this session factory. The supplied properties will be
 * validated during authentication and may be discarded or changed.
 *
 * The specified property will be added to any existing properties set for
 * this session factory.
 *
 * For details of how session properties are used see {@link SESSION_T}.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param key               the property key
 * @param value             the property value
 */
void diffusion_session_factory_property(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        const char *key, const
        char *value);


/**
 * @brief Sets user-defined session property values.
 *
 * Supplied session properties will be provided to the server when a session
 * is created using this session factory. The supplied properties will be
 * validated during authentication and may be discarded or changed.
 *
 * The specified properties will be added to any existing properties set for
 * this session factory. If any of the keys have been previously declared
 * then they will be overwritten with the new values.
 *
 * For details of how session properties are used see {@link SESSION_T}.
 *
 * @param session_factory   the session factory to mutate. If NULL, this function is a no-op.
 * @param properties        a map of user-defined session properties
 */
void diffusion_session_factory_properties(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        const HASH_T *properties);


/**
 * @brief Free a memory allocated `DIFFUSION_SESSION_FACTORY_T`
 *
 * @param session_factory the session factory to be freed
 */
void diffusion_session_factory_free(
        DIFFUSION_SESSION_FACTORY_T *session_factory);

#endif
