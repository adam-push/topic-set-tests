#ifndef _diffusion_remote_servers_h_
#define _diffusion_remote_servers_h_ 1

#include "hash_num.h"

/*
 * Copyright © 2020 - 2022 Push Technology Ltd., All Rights Reserved.
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
 * @file remote-servers.h
 *
 * Allows a client session to manage remote servers.
 *
 * A remote server provides the configuration to connect to a Diffusion server
 * belonging to a different cluster. Each server in the local cluster will
 * establish a session with each remote server.
 *
 * Higher level components, such as remote topic views, can specify the use of
 * such remote servers by name. The connecting and disconnecting is handled
 * automatically by the server (or servers in the same cluster) where the remote
 * servers are defined.
 *
 * A component can specify a remote server by name even if it does not exist
 * (has not yet been created) and when the remote server is created the
 * connection will take place automatically.
 *
 * If a remote server is removed and there are components that depend upon it,
 * those components will be disabled.
 *
 * An example of the use of remote servers is within remote topic views (those
 * that indicate that their source topics are to be taken from a different
 * server) where the name of such a server can be specified.
 *
 * <h3>Remote Server persistence and replication</h3>
 *
 * Remote server configurations created through this feature are replicated
 * across a cluster and persisted to disk.
 *
 * <h3>Access control</h3>
 *
 * The following access control restrictions are applied:
 *
 * <ul>
 *  <li>To create, remove or check a remote server, a session needs the
 *      `GLOBAL_PERMISSION_CONTROL_SERVER` permission.
 *  <li> To list remote servers, a session needs the
 *      `GLOBAL_PERMISSION_VIEW_SERVER` permission.
 * </ul>
 */

#include "session.h"
#include "diffusion-api-error.h"

typedef enum {
        /**
         * Specifies the reconnection timeout session attribute.
         *
         * This is the total time in milliseconds that will be allowed to
         * reconnect a failed connection to the remote server
         * for reconnection to work, the remove server connector must have
         * been configured to support reconnection
         *
         * If a value is not specified `DIFFUSION_DEFAULT_RECONNECTION_TIMEOUT`
         * is used.
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_OPTION_RECONNECTION_TIMEOUT = 1,

        /**
         * Specifies the delay after losing a connection before attempting
         * a reconnection.
         *
         * The value is specified in milliseconds, Default 1000 (1 second)
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_OPTION_RETRY_DELAY = 2,

        /**
         * Specifies the recovery buffer size session attribute.
         *
         * If the remote server is configured to support reconnection, a
         * session established with a non-zero reconnect-timeout retains a
         * buffer of sent messages. If the session disconnects and
         * seconnects, this buffer is used to re-send messages that the
         * server has not received.
         *
         * The default value is 10,000 messages. If reconnect-timeout is 0
         * then this value is ignored.
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_OPTION_RECOVERY_BUFFER_SIZE = 3,

        /**
         * Specifies the input buffer size session attribute.
         *
         * This is the size of the input buffer to use for the connection
         * with the remote server. It is used to receive messages from the
         * remote server. This should be set to the same size as the output
         * buffer used at the remote server.
         *
         * If not specified, a default of 1024k is used.
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_OPTION_INPUT_BUFFER_SIZE = 4,

        /**
         * Specifies the output buffer size session attribute.
         *
         * This is the size of the output buffer to use for the connection
         * with the remote server. It is used to send messages to the remote
         * server. This should be set to the same size as the input buffer
         * used by the remote server.
         *
         * If not specified, a default of 1024k is used.
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_OPTION_OUTPUT_BUFFER_SIZE = 5,

        /**
         * Specifies the maximum queue size session attribute.
         *
         * This is the maximum number of messages that can be queued to send
         * to the remote server. If this number is exceeded, the connection
         * will be closed. This must be sufficient to cater for messages
         * that may be queued whilst disconnected (awaiting reconnect).
         *
         * The default value is 10,000 messages.
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_OPTION_MAXIMUM_QUEUE_SIZE = 6,

        /**
         * Specifies the connection timeout session attribute value (in
         * milliseconds).
         *
         * If a value is not specified `DIFFUSION_DEFAULT_CONNECTION_TIMEOUT`
         * is used.
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_OPTION_CONNECTION_TIMEOUT = 7,

        /**
         * Specifies the write timeout session attribute value (in
         * milliseconds).
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_OPTION_WRITE_TIMEOUT = 8
} DIFFUSION_REMOTE_SERVER_CONNECTION_OPTION_T;


typedef enum {
        /**
         * The connection is inactive.
         *
         * This means that the remote server can successfully connect but a
         * physical connection is not being maintained as there are no
         * components that require the remote server.
         *
         * If in an inactive or failed state, a test connection will have
         * been tried to check that the connection can be made and the
         * connection will then have been closed.
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_STATE_INACTIVE = 1,

        /**
         * The remote server is connected and actively in use by components
         * that require it.
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_STATE_CONNECTED = 2,

        /**
         * The connection has failed but a retry is scheduled.
         * In this case `diffusion_check_remote_server_response_get_failure_message`
         * will provide details of the failure that resulted in a retry.
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_STATE_RETRYING = 3,

        /**
         *  The connection failed to establish.
         * If the connection was in an inactive or failed state state, a
         * test connection was tried and failed.
         *
         * In this case `diffusion_check_remote_server_response_get_failure_message`
         * will provide more detail.
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_STATE_FAILED = 4,

        /**
         * The named remote server did not exist.
         */
        DIFFUSION_REMOTE_SERVER_CONNECTION_STATE_MISSING = 5
} DIFFUSION_REMOTE_SERVER_CONNECTION_STATE_T;


/**
 * @ingroup PublicAPI_RemoteServers
 *
 * Opaque remote server struct.
 *
 * @since 6.7
 */
typedef struct DIFFUSION_REMOTE_SERVER_T DIFFUSION_REMOTE_SERVER_T;


/**
 * @ingroup PublicAPI_RemoteServers
 *
 * @brief Returns the remote server name.
 *
 * `free` should be called on this pointer when no longer needed.
 *
 * @param remote_server     the remote server.
 *
 * @return                  the remote server name.
 *
 * @since 6.7
 */
char *diffusion_remote_server_get_name(DIFFUSION_REMOTE_SERVER_T *remote_server);


/**
 * @ingroup PublicAPI_RemoteServers
 *
 * @brief Returns the url for connection to primary server.
 *
 * `free` should be called on this pointer when no longer needed.
 *
 * @param remote_server     the remote server.
 *
 * @return                  the url for connection to primary server.
 *
 * @since 6.7
 */
char *diffusion_remote_server_get_url(DIFFUSION_REMOTE_SERVER_T *remote_server);


/**
 * @ingroup PublicAPI_RemoteServers
 *
 * @brief Returns the principal used for the remote server connection.
 *
 * `free` should be called on this pointer when no longer needed.
 *
 * @param remote_server     the remote server.
 *
 * @return                  the principal used for the remote server connection.
 *
 * @since 6.7
 */
char *diffusion_remote_server_get_principal(DIFFUSION_REMOTE_SERVER_T *remote_server);


/**
 * @ingroup PublicAPI_RemoteServers
 *
 * @brief Returns the remote server connection options.
 *
 * `hash_num_free` should be called on this pointer when no longer needed.
 *
 * @param remote_server     the remote server.
 *
 * @return                  the remote server connection options.
 *
 * @since 6.7
 */
HASH_NUM_T *diffusion_remote_server_get_connection_options(DIFFUSION_REMOTE_SERVER_T *remote_server);


/**
 * @ingroup PublicAPI_RemoteServers
 *
 * @brief Return the missing topic notification filter expression or NULL if
 *        one has not been specified.
 *
 * `free` should be called on this pointer when no longer needed, if not NULL.
 *
 * @param remote_server     the remote server.
 *
 * @return                  the missing topic notification filter if specified, NULL otherwise.
 *
 * @since 6.7
 */
char *diffusion_remote_server_get_missing_topic_notification_filter(DIFFUSION_REMOTE_SERVER_T *remote_server);


/**
 * @ingroup PublicAPI_RemoteServers
 *
 * @brief Free a remote server.
 *
 * @param remote_server     the remote server.
 *
 * @since 6.7
 */
void diffusion_remote_server_free(DIFFUSION_REMOTE_SERVER_T *remote_server);


/**
 * @ingroup PublicAPI_RemoteServerBuilder
 *
 * Opaque remote server builder struct.
 *
 * @since 6.7
 */
typedef struct DIFFUSION_REMOTE_SERVER_BUILDER_T DIFFUSION_REMOTE_SERVER_BUILDER_T;


/**
 * @ingroup PublicAPI_RemoteServerBuilder
 *
 * @brief Initialize a new remote server builder.
 *        <code>diffusion_remote_server_builder_free</code> should be called on this pointer
 *        when no longer needed.
 *
 * @return          the remote server builder.
 *
 * @since 6.7
 */
DIFFUSION_REMOTE_SERVER_BUILDER_T *diffusion_remote_server_builder_init();


/**
 * @ingroup PublicAPI_RemoteServerBuilder
 *
 * @brief Reset the builder.
 *
 * @param builder   the remote server builder.
 *
 * @return          the remote server builder.
 *
 * @since 6.7
 */
DIFFUSION_REMOTE_SERVER_BUILDER_T *diffusion_remote_server_builder_reset(
        DIFFUSION_REMOTE_SERVER_BUILDER_T *builder);


/**
 * @ingroup PublicAPI_RemoteServerBuilder
 *
 * @brief Specifies the name of a principal used by the remote server to
 *        connect to the primary server.
 *
 * The default, if not specified, is the anonymous principal.
 *
 * @param builder       the remote server builder.
 * @param principal     pricipal name or a zero length string to indicate an
 *                      anonymous connection.
 *
 * @return              the remote server builder.
 *
 * @since 6.7
 */
DIFFUSION_REMOTE_SERVER_BUILDER_T *diffusion_remote_server_builder_principal(
        DIFFUSION_REMOTE_SERVER_BUILDER_T *builder,
        char *principal);


/**
 * @ingroup PublicAPI_RemoteServerBuilder
 *
 * @brief Specifies a single connection option.
 *
 * This will add to the options currently specified to this builder,
 * or will replace a value if it has already been specified.
 *
 * @param builder   the remote server builder.
 * @param option    the connection option
 * @param value     the connection option value or NULL to remove the option.
 *
 * @return          the remote server builder.
 *
 * @since 6.7
 */
DIFFUSION_REMOTE_SERVER_BUILDER_T *diffusion_remote_server_builder_connection_option(
        DIFFUSION_REMOTE_SERVER_BUILDER_T *builder,
        DIFFUSION_REMOTE_SERVER_CONNECTION_OPTION_T option,
        char *value);


/**
 * @ingroup PublicAPI_RemoteServerBuilder
 *
 * @brief Specifies a map of {@link DIFFUSION_REMOTE_SERVER_CONNECTION_OPTION_T} settings.
 *
 * This will replace any options currently set for this builder.
 * Any options not supplied will take their default values.
 * If no connection options are specified, either using this method or
 * {@link diffusion_remote_server_builder_connection_option}
 * then all options will take their default value.
 *
 * @param builder               the remote server builder.
 * @param connection_options    map of options.
 *
 * @return                      the remote server builder.
 *
 * @since 6.7
 */
DIFFUSION_REMOTE_SERVER_BUILDER_T *diffusion_remote_server_builder_connection_options(
        DIFFUSION_REMOTE_SERVER_BUILDER_T *builder,
        HASH_NUM_T *connection_options);


/**
 * @ingroup PublicAPI_RemoteServerBuilder
 *
 * @brief Specifies the credentials to use when connecting to the primary server.
 *
 * The default, if not specified, is {@link CREDENTIALS_TYPE_T#NONE NONE}.
 *
 * @param builder       the remote server builder.
 * @param credentials   the credentials to use.
 *
 * @return              the remote server builder.
 *
 * @since 6.7
 */
DIFFUSION_REMOTE_SERVER_BUILDER_T *diffusion_remote_server_builder_credentials(
        DIFFUSION_REMOTE_SERVER_BUILDER_T *builder,
        CREDENTIALS_T *credentials);


/**
 * @ingroup PublicAPI_RemoteServerBuilder
 *
 * @brief Specifies a topic selector expression which will filter missing topic notifications
 *        propagated from secondary to primary server.
 *
 * {@link SVC_MISSING_TOPIC_REQUEST_T Missing topic notifications} are notified when a client
 * subscribes using a topic selector that matches no existing topics. By specifying a missing
 * topic selector filter, all notifications that match the filter on the secondary server will
 * be propagated to the primary server.
 *
 * A match occurs if the path prefix of the subscription selector matches the path prefix of the
 * specified selector.
 * If no filter is specified then no missing topic notifications will be propagated.
 *
 * The special selector expressions <code>*.*</code> may be used to indicate that all missing topic
 * notifications should be propagated.
 *
 * Only the path prefix of the specified selector(s) is considered when matching, therefore any
 * use of regular expressions would be ignored.
 *
 * @param builder   the remote server builder.
 * @param filter    a topic selector expression specifying the filter to apply, or NULL to remove
 *                  any existing filter.
 *
 * @return          the remote server builder.
 *
 * @since 6.7
 */
DIFFUSION_REMOTE_SERVER_BUILDER_T *diffusion_remote_server_builder_missing_topic_notification_filter(
        DIFFUSION_REMOTE_SERVER_BUILDER_T *builder,
        char *filter);


/**
 * @ingroup PublicAPI_RemoteServerBuilder
 *
 * @brief Free a remote server builder.
 *
 * @param           builder the remote server builder.
 *
 * @since 6.7
 */
void diffusion_remote_server_builder_free(DIFFUSION_REMOTE_SERVER_BUILDER_T *builder);


/**
 * @ingroup PublicAPI_RemoteServerBuilder
 *
 * @brief Creates a remote server struct using the current values known to this builder.
 *
 * @param builder       the remote server builder.
 * @param name          the name of the remote server.
 * @param url           the URL to use to connect to the primary server.
 * @param api_error     populated on API error. Can be NULL.
 *
 * @return  a new remote server struct if the operation was successful, NULL otherwise.
 *          In this case, if a non-NULL <code>api_error<code> pointer has been provided, this will
 *          be populated with the error information and should be freed with
 *          <code>diffusion_api_error_free</code>.
 *
 * @since 6.7
 */
DIFFUSION_REMOTE_SERVER_T *diffusion_remote_server_builder_create(
        DIFFUSION_REMOTE_SERVER_BUILDER_T *builder,
        char *name,
        char *url,
        DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_RemoteServerFeature
 *
 * @brief Callback when a remote server creation attempt has been made.
 *
 * If the remote server definition is nil, this could
 * mean an error has occured. These may include:
 * <ul>
 *  <li>RemoteServerAlreadyExists - if a remote server with the given name
 *      already exists
 *  <li>ClusterRepartition - if the cluster was repartitioning
 *  <li>SessionSecurity - if the calling session does not have
 *      `GLOBAL_PERMISSION_CONTROL_SERVER` permission
 *  <li>SessionClosed - if the session is closed
 * </ul>
 *
 * @param remote_server   The remote server created if no errors are returned
 * @param errors          Errors encounted during the attempted creation of the remote server
 * @param context         User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_remote_server_created_cb)(DIFFUSION_REMOTE_SERVER_T *remote_server, LIST_T *errors, void *context);


typedef struct diffusion_create_remote_server_params_s {
        /// The remote server
        DIFFUSION_REMOTE_SERVER_T *remote_server;
        /// Callback when a remote server creation attempt has been made.
        on_remote_server_created_cb on_remote_server_created;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_CREATE_REMOTE_SERVER_PARAMS_T;


/**
 * @ingroup PublicAPI_RemoteServerFeature
 *
 * @brief Create a new remote server instance with default connection options.
 *
 * If a remote server with the same name already exists an error will be returned.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameters defining the `diffusion_create_remote_server` request
 *                      and callbacks.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_create_remote_server(
        SESSION_T *session,
        const DIFFUSION_CREATE_REMOTE_SERVER_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_RemoteServerFeature
 *
 * @brief Callback when a remote server removal attempt has been made.
 *
 * @param context         User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_remote_server_removed_cb)(void *context);


typedef struct diffusion_remove_remote_server_params_s {
        /// The name of the remote server to be removed
        char *name;
        /// Callback when the remote server removal attempt has been made
        on_remote_server_removed_cb on_remote_server_removed;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_REMOVE_REMOTE_SERVER_PARAMS_T;


/**
 * @ingroup PublicAPI_RemoteServerFeature
 *
 * @brief Remove a named remote server if it exists
 *
 * If the named remote server does not exist the callback
 * will return without an error
 *
 * When a named remote server is removed, any components that specify it
 * would be disabled.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameters defining the `diffusion_remove_remote_server` request
 *                      and callbacks.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_remove_remote_server(
        SESSION_T *session,
        const DIFFUSION_REMOVE_REMOTE_SERVER_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_RemoteServerFeature
 *
 * @brief Callback when a response is received from the server,
 *        returning a list of remote servers.
 *
 * @param remote_servers  The list of remote servers
 * @param context         User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_remote_servers_listed_cb)(LIST_T *remote_servers, void *context);


typedef struct diffusion_list_remote_servers_params_s {
        /// Callback when the remote server removal attempt has been made
        on_remote_servers_listed_cb on_remote_servers_listed;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_LIST_REMOTE_SERVERS_PARAMS_T;


/**
 * @ingroup PublicAPI_RemoteServerFeature
 *
 * @brief Lists all the remote servers that have been created.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameters defining the `diffusion_list_remote_servers` request
 *                      and callbacks.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_list_remote_servers(
        SESSION_T *session,
        const DIFFUSION_LIST_REMOTE_SERVERS_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_RemoteServerFeature
 *
 * Opaque check remote server response struct
 */
typedef struct DIFFUSION_CHECK_REMOTE_SERVER_RESPONSE_T DIFFUSION_CHECK_REMOTE_SERVER_RESPONSE_T;


/**
 * @ingroup PublicAPI_RemoteServerFeature
 *
 * @brief Returns the state of the connection of the remote server.
 *
 * @param response  the check remote server response
 *
 * @return          the check remote server response's state
 */
DIFFUSION_REMOTE_SERVER_CONNECTION_STATE_T diffusion_check_remote_server_response_get_state(
        DIFFUSION_CHECK_REMOTE_SERVER_RESPONSE_T *response);


/**
 * @ingroup PublicAPI_RemoteServerFeature
 *
 * @brief Returns a memory allocated copy of the check remote server response failure message.
 *        `free` should be called on this pointer when no longer needed.
 *
 * @param response  the check remote server response
 *
 * @return          the check remote server response's failure message
 */
char *diffusion_check_remote_server_response_get_failure_message(DIFFUSION_CHECK_REMOTE_SERVER_RESPONSE_T *response);


/**
 * @ingroup PublicAPI_RemoteServerFeature
 *
 * @brief Callback when a response is received from the server,
 *        returning the details of the remote server state.
 *
 * @param response  The response from the server
 * @param context   User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_remote_server_checked_cb)(DIFFUSION_CHECK_REMOTE_SERVER_RESPONSE_T *response, void *context);


typedef struct diffusion_check_remote_server_params_s {
        /// The name of the remote server to be removed
        char *name;
        /// Callback when the remote server removal attempt has been made
        on_remote_server_checked_cb on_remote_server_checked;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_CHECK_REMOTE_SERVER_PARAMS_T;


/**
 * @ingroup PublicAPI_RemoteServerFeature
 *
 * @brief Checks the current state of a named remote server.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameters defining the `diffusion_check_remote_server` request
 *                      and callbacks.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_check_remote_server(
        SESSION_T *session,
        const DIFFUSION_CHECK_REMOTE_SERVER_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);

#endif
