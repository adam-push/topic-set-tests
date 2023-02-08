#ifndef _diffusion_session_types_h_
#define _diffusion_session_types_h_ 1

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
 * @file session_types.h
 *
 * Types to describe a session, its parameters and related functions.
 */

#include <apr.h>
#include <apr_pools.h>

#include "error.h"
#include "hash.h"
#include "d_semaphore.h"
#include "delta.h"
#include "types/error_types.h"
#include "types/security_types.h"
#include "types/service_types.h"
#include "types/topic_types.h"
#include "retry-strategy.h"

/** The time, in milliseconds, that a connection attempt will wait to be established before it is terminated. Default value is 2 seconds. */
extern const uint32_t DIFFUSION_DEFAULT_CONNECTION_TIMEOUT;

/** The time, in milliseconds, that reconnection attempts will be retried. Default value is 60 seconds. */
extern const int DIFFUSION_DEFAULT_RECONNECT_TIMEOUT;

/** The time, in milliseconds, that a blocking write will timeout after. Default value is 2 seconds. */
extern const uint32_t DIFFUSION_DEFAULT_WRITE_TIMEOUT;

/** The default delay between reconnection attempts, in milliseconds. Default value is 5 seconds. */
extern const uint32_t DIFFUSION_DEFAULT_RECONNECT_DELAY;

/** The minimum max message size (in bytes). This value is 1024. */
extern const uint32_t DIFFUSION_MAXIMUM_MESSAGE_SIZE_MINIMUM;

/* The default maximum message size in bytes. The maximum message size
 * limits the size of received messages. This default value is
 * is `INT_MAX`, so the message size is effectively unlimited.
 */
extern const uint32_t DIFFUSION_DEFAULT_MAXIMUM_MESSAGE_SIZE;

/** The default maximum outbound queue size. Default value is 1000. */
extern const uint32_t DIFFUSION_DEFAULT_MAXIMUM_QUEUE_SIZE;

/** The default input buffer size in bytes. Default value is 131072. */
extern const uint32_t DIFFUSION_DEFAULT_INPUT_BUFFER_SIZE;

/** The default output buffer size in bytes. Default value is 131072. */
extern const uint32_t DIFFUSION_DEFAULT_OUTPUT_BUFFER_SIZE;

/** The default request path. Default path is "/diffusion". */
extern const char *const DIFFUSION_DEFAULT_REQUEST_PATH;

/** The default recovery buffer size. Default value is 128. */
extern const uint32_t DIFFUSION_DEFAULT_RECOVERY_BUFFER_SIZE;

/** The default number of reconnection attempts. This value is the result of
 * `DIFFUSION_DEFAULT_RECONNECT_TIMEOUT / DIFFUSION_DEFAULT_RECONNECT_DELAY`, thus
 * the default value is 12.
 */
extern const uint32_t DIFFUSION_DEFAULT_RECONNECT_RETRY_COUNT;

typedef struct DIFFUSION_SESSION_ATTRIBUTES_T DIFFUSION_SESSION_ATTRIBUTES_T;
typedef struct TOPIC_ROUTING_T TOPIC_ROUTING_T;
typedef struct FLOW_CONTROL_T FLOW_CONTROL_T;
typedef struct MESSAGE_QUEUE_T MESSAGE_QUEUE_T;
typedef struct RECOVERY_BUFFER_T RECOVERY_BUFFER_T;
typedef struct DIFFUSION_REQUEST_STREAM_REGISTRY_T DIFFUSION_REQUEST_STREAM_REGISTRY_T;
typedef struct DIFFUSION_CLIENT_SESSION_LOCKS_T DIFFUSION_CLIENT_SESSION_LOCKS_T;

/**
 * A session will move through different states during its lifecycle. This
 * enumeration lists those possible states.
 */
typedef enum {
        /// Unknown
        SESSION_STATE_UNKNOWN = -1,

        /// In the process of connecting to Diffusion.
        CONNECTING,

        /// Connected to Diffusion but not yet receiving data. (Deprecated).
        CONNECTED_INITIALISING,

        /// Connected to Diffusion and receiving data.
        CONNECTED_ACTIVE,

        /// Recovering from a connection error by attempting to reconnect to
        /// the same Diffusion server.
        RECOVERING_RECONNECT,

        /// @deprecated
        /// Recovering from a connection error by attempting to reconnect to
        /// a different Diffusion server.
        RECOVERING_FAILOVER,

        /// This session was closed by the client.
        CLOSED_BY_CLIENT,

        /// This session was closed by Diffusion.
        CLOSED_BY_SERVER,

        /// An error occurred which caused the connection to be lost.
        CLOSED_FAILED
} SESSION_STATE_T;

/**
 * Structure describing a session ID, which is a unique identifier used by
 * Diffusion for tracking this client; it is also used on reconnection to
 * attempt to restore existing session state.
 */
typedef struct session_id_s {
        /// Identifies a specific Diffusion server instance.
        uint64_t server_instance;
        /// Unique ID for this particular session on a server
        /// instance.
        uint64_t value;
} SESSION_ID_T;

// Forward declarations.
struct session_s;
struct transport_s;

/** Standard topic handler callback. */
typedef int (*TOPIC_HANDLER_T)(struct session_s *session, const TOPIC_MESSAGE_T *message);

/** Message stream callback. */
typedef int (*STREAM_MESSAGE_LISTENER_T)(struct session_s *session, const STREAM_MESSAGE_T *message, void *context);

/** Standard error handler callback. */
typedef int (*ERROR_HANDLER_T)(struct session_s *session, const DIFFUSION_ERROR_T *error);

/** Standard conversation discard handler callback. */
typedef int (*DISCARD_HANDLER_T)(struct session_s *session, void *context);

/**
 * When a session changes state, a listener may be called to inform the client
 * of the transition via the appropriate function pointer in this structure.
 */
typedef struct session_listener_s {
        /// The current session has transitioned between states.
        void (*on_state_changed)(struct session_s *session, const SESSION_STATE_T old_state, const SESSION_STATE_T new_state);
        /// Called when an error occurs, but there is not a more
        /// specific handler registered to deal with it.
        /// deprecated since 6.9. Will be removed in the future
        DEPRECATED(void (*on_handler_error)(const struct v5_message_s *msg))
} SESSION_LISTENER_T;

/**
 * A handler set is registered against a conversation id and contains function
 * pointers to callback handlers related to that conversation. Users should
 * avoid populating or accessing this structure.
 */
typedef struct handler_set_s {
        /// The handler's service type
        SERVICE_TYPE_T service_type;

        /// Handlers for any service that may be related to the
        /// conversation.
        void (*service_handler[SERVICE_TYPE_MAX_VALUE])();

        /// Error handler callbacks for services related to the
        /// conversation.
        int (*error_handler[SERVICE_TYPE_MAX_VALUE])();

        /// Callback invoked when a conversation is discarded,
        /// e.g. due to reconnect/failover.
        DISCARD_HANDLER_T discard_handler[SERVICE_TYPE_MAX_VALUE];

        /// If a topic message is received for this conversation, then this
        /// callback is used to process it.
        TOPIC_HANDLER_T topic_message_handler;

        /// This may contain service-specific data to be passed to context
        /// handlers.
        void *system_context;

        /// A structure containing the parameters passed in the initial API
        /// call. Specific to each API function, but normally includes user
        /// callbacks and context data.
        void *user_params;

        /// Callback for freeing the system context when handler_set_free()
        /// is called. If not provided, the memory is not freed.
        void (*system_context_free)(void *ctx);

        /// Callback for freeing the user params when handler_set_free()
        /// is called. If not provided, free() will be used.
        void (*user_params_free)(void *params);
} HANDLER_SET_T;

typedef enum {
        RECONNECTION_STRATEGY_TYPE_LEGACY = 0,
        RECONNECTION_STRATEGY_TYPE_CALLBACK = 1
} RECONNECTION_STRATEGY_TYPE_T;

typedef enum {
        /// Reconnect should be attempted.
        RECONNECTION_ATTEMPT_ACTION_START = 0,
        /// Reconnect should be aborted.
        RECONNECTION_ATTEMPT_ACTION_ABORT = 1
} RECONNECTION_ATTEMPT_ACTION_T;

struct reconnection_args_repeating {
        long retry_count;
        long retry_delay;
        long retries_done;
};

typedef RECONNECTION_ATTEMPT_ACTION_T (*PERFORM_RECONNECTION_CB)(struct session_s *, void *args);
typedef void (*PERFORM_RECONNECTION_AFTER_ACTION_CB)(struct session_s *, void *args);

/**
 * When connecting to Diffusion, a reconnection strategy can be employed which
 * describes what to do if the connection fails. It is recommended that a
 * reconnection strategy is created using one of the factory functions, e.g.
 * make_reconnection_strategy_repeating_attempt, as direct access to the
 * fields in this structure is likely to be withdrawn in the future.
 */
typedef struct reconnection_strategy_s {
        RECONNECTION_STRATEGY_TYPE_T type;

        /// \deprecated {Setting values directly for the reconnection
        /// strategy (formerly, failover strategy) is
        /// deprecated. Please use the functions provided to make a
        /// reconnetion strategy structure and to modify/access it.

        // For backwards compatibility when type ==
        // RECONNECTION_STRATEGY_TYPE_LEGACY
        long retry_count;
        long retry_delay;

        // Maximum length of time to reconnect for. Linked to the
        // reconnect timeout that is sent to the server.
        long reconnect_timeout;

        // User has specified their own reconnect_timeout with
        // reconnection_strategy_set_timeout().
        int _overridden_reconnect_timeout;

        char *current_url_ptr;

        // Function to be called to decide whether to reconnect or
        // abort.
        PERFORM_RECONNECTION_CB perform_reconnection;

        // Called after a successful connection.
        PERFORM_RECONNECTION_AFTER_ACTION_CB on_success;

        // Called after a failed connection.
        PERFORM_RECONNECTION_AFTER_ACTION_CB on_failure;

        // Args to be passed to callback function.
        void *perform_reconnection_args;

        // Function used to free the reconnection args during
        // reconnection_args_free(). May be NULL.
        void (*_free_args_fn)(void *);

} RECONNECTION_STRATEGY_T;

/**
 * When creating a session which performs a connection to Diffusion asynchronously,
 * these callbacks can be supplied and are invoked when the connection is made
 * successfully, or an error occurs.
 */
typedef struct session_create_callback_s {
        /// A connection has been made to Diffusion successfully.
        int (*on_connected)(struct session_s *session);

        /// An error has occured with the connection.
        int (*on_error)(struct session_s *session, DIFFUSION_ERROR_T *error);
} SESSION_CREATE_CALLBACK_T;

/**
 * This is the session structure which is created to represent a single
 * instance of connection (session) with Diffusion.
 */
typedef struct session_s {
        /// A unique session identifier allocated by Diffusion.
        SESSION_ID_T *id;

        /// The URL used to connect to Diffusion.
        const char *url_str;

        /// The current session connection state.
        volatile SESSION_STATE_T state;

        /// Callbacks to be notified when the session changes state
        /// (e.g. active, closed).
        SESSION_LISTENER_T *listener;

        /// If a handler has not otherwise been declared for a particular
        /// topic, then this handler is tried.
        TOPIC_HANDLER_T global_topic_handler;

        /// If defined, this handler is called in the absence of a
        /// service-specific handler.
        int (*global_service_error_handler)(struct session_s *session, const DIFFUSION_ERROR_T *error);

        /// User-supplied context for this session. It is the user's
        /// responsibility to free any memory that this points to when
        /// the session is no longer required.
        void *user_context;

        /*
         * Users should not modify or directly access fields below this point.
         */

        // The session's attributes
        DIFFUSION_SESSION_ATTRIBUTES_T *session_attributes;

        // The session's locks
        DIFFUSION_CLIENT_SESSION_LOCKS_T *session_locks;

        // Describes how the client should handle connection failures,
        // disconnections and reconnections.
        RECONNECTION_STRATEGY_T *reconnection_strategy;

        // Authentication principal (typically, username) used when
        // connecting to Diffusion.
        const char *principal;

        // Authentication credentials used when connecting to Diffusion.
        CREDENTIALS_T *credentials;

        // Private token identifying this session.
        void *token;

        int _connection_protocol;
        int _connection_version;
        CONNECTION_RESPONSE_CODE_T _connection_response_code;

        // Server-provided ping period, or -1 if not supplied.
        int64_t ping_period;

        // The underlying transport mechanism; do not modify this directly.
        struct transport_s *transport;

        // Thread used to coordinate connections.
        apr_thread_t *session_thread;

        // A map of topic selectors to topic handlers (TOPIC_HANDLER_T).
        HASH_T *topic_handlers;

        // Flow control
        FLOW_CONTROL_T *flow_control;

        //Request stream registry
        DIFFUSION_REQUEST_STREAM_REGISTRY_T *request_stream_registry;

        // Diff activity history
        DIFFUSION_ACTIVITY_HISTORY_T *activity_history;

        // Service definitions/handlers that are associated with this
        // session. This should not be modified directly by the user.
        SVC_DEFN_T *service_definitions[SERVICE_TYPE_MAX_VALUE];

        // Handlers for Diffusion messages which are not topic messages (ie,
        // not MESSAGE_TYPE_TOPIC_LOAD, MESSAGE_TYPE_DELTA or
        // MESSAGE_TYPE_FETCH_REPLY).
        void (*message_handlers[MESSAGE_TYPE_MAX_VALUE])(struct session_s *session, MESSAGE_T *msg);

        // A map of topic selectors to message handlers (STREAM_MESSAGE_LISTENER_T)
        HASH_T *stream_message_listeners;

        // A map of conversation IDs to handlers (HANDLER_SET_T).
        HASH_T *conversation_handlers;

        // Outbound message queue; not to be modified by the user.
        MESSAGE_QUEUE_T *message_queue;

        // APR pools for internal use.
        apr_pool_t *_root_pool;
        apr_pool_t *_session_pool;
        apr_pool_t *_connection_pool;

        // Semaphore used to coordinate the connection. For internal
        // use only.
        SEMAPHORE_T *_sem_connection;

        // Internal semaphore that prevents the transport getting invoked
        // during shutdown.
        SEMAPHORE_T *_sem_transport;

        // Internal session locks
        apr_thread_mutex_t *_session_mutex;
        apr_thread_mutex_t *_close_mutex;
        // Conversation handler mutex
        apr_thread_mutex_t *_handler_mutex;

        // Structure for callbacks used in asynchronous session
        // creation. For internal use only.
        SESSION_CREATE_CALLBACK_T _creation_callbacks;

        // Atomic sequence number for internal use.
        apr_uint32_t _seqnum;

        // Atomic count of pending operations for internal use.
        volatile apr_uint32_t _pending_operations;

        // Topic IDs and their mapping to topic paths as received by
        // subscription notifications.
        HASH_T *_topic_ids;

        // Mapping of topic path to topic IDs.
        HASH_T *_topic_paths;

        // (Internal) topic routing struct
        TOPIC_ROUTING_T *topic_routing;

        // Sequence number of last message sent to the server ("put on the
        // wire", rather than queued).
        apr_uint32_t lastSentSequence;

        // Sequence number of last message received from the server.
        apr_uint32_t lastReceivedSequence;

        // A recovery buffer of messages that have been sent, for use in case
        // we need to resend on reconnection.
        RECOVERY_BUFFER_T *recovery_buffer;

        // Counter tracking the number of times this session has been
        // successfully connected.
        apr_uint32_t _connection_count;

        // Map of topic paths to values, used for binary topics so we can
        // apply diffs.
        HASH_T *_topic_values;

        // The strategy will determine whether a failure to open a session due to a
        // transient exception should be retried and if so, at what interval and for
        // how many attempts.
        DIFFUSION_RETRY_STRATEGY_T *initial_retry_strategy;
} SESSION_T;

#endif
