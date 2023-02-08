#ifndef _diffusion_session_h_
#define _diffusion_session_h_ 1

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
 * @file session.h
 *
 * Functions relating to a session and session management.
 *
 * A client session to a server or cluster of servers.
 *
 * A new session can be created by connecting to a server using
 * `session_create_with_session_factory`, specifying the server URL. There is also
 * a non-blocking variant `session_create_async_with_session_factory`. The session
 * factory can be configured to control the behavior the session.
 *
 * The session provides a variety of operations to the application. These are
 * grouped into feature headers, such as `topics.h` and `messaging.h`.
 *
 * <h3>Session lifecycle</h3>
 *
 * Each session is managed by a server. The server assigns the session a
 * unique identity, and manages the session's topic
 * subscriptions, security details, and session properties.
 *
 * A session can be terminated using `session_close()`. A session may also be
 * terminated by the server because of an error or a time out, or by other
 * privileged sessions using the ClientControl feature.
 *
 * A client can become disconnected from the server, and reconnect to the server
 * without loss of the session. Reconnection can be configured using
 * `diffusion_session_factory_reconnection_strategy` via the session factory.
 * The server must be configured to allow reconnection.
 *
 * If a session is connected to a server that belongs to a cluster with session
 * replication enabled, and then becomes disconnected, it will attempt to
 * reconnect to the original server. A properly configured load balancer can
 * detect that the original server is unavailable and re-route the reconnection
 * request to a second server in the cluster. The second server can recover
 * session data and continue the session. This process is known as "fail over".
 * Unlike reconnection, in-flight messages can be lost during failover, and the
 * application will be unsubscribed and re-subscribed to topics.
 *
 * The current state of the session can be retrieved with `session_state_get()`.
 * A listener can be registered when the session is created, which will be
 * notified when the session state changes.
 *
 * <h3>Session properties</h3>
 *
 * For each session, the server stores a set of session properties that describe
 * various attributes of the session.
 *
 * There are two types of session property. Fixed properties are assigned by the
 * server. User-defined properties are assigned by the application.
 *
 * Many operations use session filter expressions that use session properties
 * to select sessions.
 *
 * A privileged client can monitor other sessions, including changes to their
 * session properties, using a session properties listener. When registering to
 * receive session properties, special key values of
 * `PROPERTIES_SELECTOR_ALL_FIXED_PROPERTIES` and `PROPERTIES_SELECTOR_ALL_USER_PROPERTIES`
 * can be used.
 *
 * Each property is identified by a key. Most properties have a single string
 * value. The exception is the `$Roles` fixed property which has a set of string
 * values.
 *
 * Fixed properties are identified by keys with a `$` prefix. The available
 * fixed session properties are:
 * <table>
 *  <tr>
 *    <td><b>Key</b></td>
 *    <td><b>Description</b></td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$ClientIP`</td>
 *    <td>The Internet address of the client in string format.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$ClientType`</td>
 *    <td>The client type of the session. One of `ANDROID`, `C`,
 *        `DOTNET`, `IOS`, `JAVA`, `JAVASCRIPT_BROWSER`, `MQTT`, `PYTHON`, or
 *        `OTHER`.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$Connector`</td>
 *    <td>The configuration name of the server connector that the client connected
 *        to.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$Country`</td>
 *    <td>The country code for the country where the client's Internet address was
 *        allocated (for example, `NZ` for New Zealand). Country codes are as
 *        defined by `Locale`. If the country code could not be determined, this
 *        will be a zero length string.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$GatewayType`</td>
 *    <td>Gateway client type. Only set for gateway client sessions.
 *        If present it indicates the type of gateway client (e.g. Kafka).</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$GatewayId`</td>
 *    <td>The identity of a gateway client session. Only present if the $GatewayType
 *        session property is present.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$Language`</td>
 *    <td>The language code for the official language of the country where the
 *        client's Internet address was allocated (for example, `en` for
 *        English). Language codes are as defined by `Locale`. If the language
 *        could not be determined or is not applicable, this will be a zero length
 *        string.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$Latitude`</td>
 *    <td>The client's latitude, if available. This will be the string
 *        representation of a floating point number and will be `NaN` if not
 *        available.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$Longitude`</td>
 *    <td>The client's longitude, if available. This will be the string
 *        representation of a floating point number and will be `NaN` if not
 *        available.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$MQTTClientId`</td>
 *    <td>The MQTT client identifier. Only set for MQTT sessions. If present,
 *    the value of the `$ClientType` session property will be MQTT.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$Principal`</td>
 *    <td>The security principal associated with the client session.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$Roles`</td>
 *    <td>Authorisation roles assigned to the session. This is a set of roles
 *        represented as quoted strings (for example, `"role1","role2"`). The
 *        utility method `diffusion_string_to_roles()` can be used to parse
 *        the string value into a set of roles.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$ServerName`</td>
 *    <td>The name of the server to which the session is connected.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$SessionId`</td>
 *    <td>The session identifier. Equivalent to `session_id_to_string()`.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$StartTime`</td>
 *    <td>The session's start time in milliseconds since the epoch.</td>
 *  </tr>
 *  <tr valign="top">
 *    <td>`$Transport`</td>
 *    <td>The session transport type. One of `WEBSOCKET`,
 *        `HTTP_LONG_POLL`, `TCP`, or `OTHER`.</td>
 *  </tr>
 * </table>
 *
 * All user-defined property keys are non-empty strings. The characters <code>' '</code>,
 * <code>'\\t'</code>, <code>'\\r'</code>, <code>'\\n'</code>, <code>'"'</code>, <code>'''</code>,
 * <code>'('</code>, <code>')'</code> are not allowed.
 *
 * Session properties are initially associated with a session as follows:
 * <ol>
 *  <li>When a client starts a new session, it can optionally propose
 *      user-defined session properties (see `diffusion_session_factory_property`
 *      and `diffusion_session_factory_properties`). Session properties proposed in this
 *      way must be accepted by the authenticator. This safeguard prevents abuse by a
 *      rogue, unprivileged client.
 *  <li>The server allocates all fixed property values.
 *  <li>The new session is authenticated by registered authenticators. An
 *      authenticator that accepts a session can veto or change the user-defined
 *      session properties and add new user-defined session properties. The
 *      authenticator can also change certain fixed properties.
 * </ol>
 *
 * Once a session is established, its user-defined session properties can be
 * modified by clients with `GLOBAL_PERMISSION_VIEW_SESSION` and `GLOBAL_PERMISSION_MODIFY_SESSION`
 * permissions using `diffusion_set_session_properties()`.
 * A privileged client can also modify its own session properties.
 *
 * If a session re-authenticates (see `change_principal()`), the
 * authenticator that allows the re-authentication can modify the user-defined
 * session properties and a subset of the fixed properties as mentioned above.
 *
 * <h3>Session filters</h3>
 *
 * Session filters are query expressions for session properties. They can be
 * used to address a set of sessions based on their session properties. For
 * example, it is possible to send a message to all sessions that satisfy a
 * specified filter. Session filters are parsed and evaluated at the server.
 *
 * A session filter expression consists of either a single clause, or multiple
 * clauses connected by the binary operators `and` and `or`. The
 * `and` operator takes precedence over `or` but parentheses can be
 * used to override the precedence. For example:
 * <ul>
 *  <li>`Department is "Accounts"`
 *  <li>`hasRoles ["operator" "trading desk"]`
 *  <li>`Department is "Payroll" and Status is "Closed"`
 *  <li>`(Department is "Accounts" or Department is "Payroll") and Status is "Closed`
 * </ul>
 *
 * The unary `not` operator can be used to negate the following clause or
 * an expression within parentheses:
 * <ul>
 *  <li>`Department is "Payroll"`
 *  <li>`not (Department is "Payroll" or Department is "Accounts"`
 * </ul>
 *
 * An equality clause has the form <em>key operator value</em> where
 * <em>key</em> is the name of a session property and <em>value</em> is the
 * property value. The supported operators are `is` or `eq`, both of
 * which mean "equals", and `ne` which means "does not equal". Values are
 * strings enclosed within single or double quotes. Special characters
 * (<code>"</code>, <code>'</code> or <code>\\</code>) can be included within the value by
 * preceding with the escape character <code>\\</code>. The utility method `diffusion_escape()`
 * can be used to insert escape characters into a value.
 *
 * The `all` operator matches all sessions.
 *
 * The `has` operator can be used to check if a property is present. This is useful
 * for determining whether a user-defined property or an optional fixed property is set for a session.
 *
 * `hasRoles` is a special operator for querying the `$Roles`
 * session property. A `hasRoles` clause has the form <em>hasRoles
 * ["role1" "role2" ... "roleN"]</em>. The clause will match sessions that have
 * all the specified authorisation roles. Each role is a string enclosed within
 * either single or double quotes. Roles can be space or comma separated.
 *
 * The `in` operator allows for the querying of a specific session property
 * to see if it exists in a defined set. An example of this would to be filter all
 * sessions from a set of countries, say Germany, France, and the UK. The query would be
 * `$Country in 'UK','DE','FR'`.
 *
 * The lists provided to `in` and `hasRoles` can optionally use square brackets and commas as delimiters.
 * For example `$Country in ['UK','DE','FR']`.
 *
 * The `$Roles` session property can also be queried with an equality
 * clause, for example, <code>$Roles eq '"admin","client"'</code>, but the
 * `hasRoles` clause is usually more convenient. An equality clause will
 * match sessions that have exactly the listed roles. In contrast, a
 * `hasRoles` clause will match any sessions with the listed roles,
 * regardless of whether they have other roles. The equality clause requires the
 * value to be in the canonical form produced by the `diffusion_roles_to_string()`
 * utility method.
 *
 * All operators are case insensitive.
 * <h4>Examples</h4>
 *
 * The following are further examples of valid session filter expressions:
 * <ul>
 *  <li><code>$Principal is "Alice"</code>
 *  <li><code>Department is "Accounts" and $Country ne "US"</code>
 *  <li><code>$Language EQ "en" and $Country NE "US"</code>
 *  <li><code>not (Department is "Accounts" or Department is "Payroll") and $Country is
 *      "FR"</code>
 *  <li><code>Text is "xyz\"\\\\\"</code>
 *  <li><code>hasRoles ["operator"]</code>
 *  <li><code>$Transport is "wss" and hasRoles ["accountancy" "administrator"]</code>
 *  <li><code>hasRoles ["operator"] and not hasRoles ["administrator"]</code>
 *  <li><code>$Version in {"6","7","8"}</code>
 *  <li><code>$Country in ['UK','DE','FR']</code>
 *  <li><code>has Department</code>
 *  <li><code>all</code>
 * </ul>
 *
 * <h3>Session locks</h3>
 *
 * The actions of multiple sessions can be coordinated using session locks. See
 * file `session-lock.h`.
 */

#include <apr_pools.h>
#include <apr_atomic.h>

#include "session-factory.h"
#include "retry-strategy.h"

#include "hash.h"
#include "reconnect.h"

#include "types/error_types.h"
#include "types/security_types.h"
#include "types/topic_types.h"
#include "types/session_types.h"


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Create a new session and synchronously open a connection to the
 * Diffusion server at the endpoint specified by the URL.
 *
 * @note This call retains a reference to the memory allocated for the {@link
 * CREDENTIALS_T credentials} and it is the callers' responsibility to ensure
 * that this memory is not freed or overwritten for the lifetime of the
 * session structure.
 *
 * @note The error structure contains the result of the session creation
 * attempt and will not be overwritten if the session is subsequently
 * reconnected or fails over. Once session_create() returns, it is safe to
 * release any memory owned by the error structure.
 *
 * @param url_str               A URL describing the endpoint to connect to.
 * @param principal             Typically, the username of the connecting user or application.
 *                              NULL indicates that the principal will not be passed on connection.
 * @param credentials           The credentials associated with the principal, or NULL.
 *                              \em MUST be available for the duration of the session.
 * @param listener              A pointer to a {@link SESSION_LISTENER_T} structure containing callbacks
 *                              to be called on session-specific events.
 * @param reconnection_strategy How to handle reconnection situations when the server is initially
 *                              unavailable. If NULL, a default strategy is provided that
 *                              attempts to connect every 5 seconds for up to 1 minute.
 * @param error                 A pointer to an error structure, initialized to zero, which is populated in
 *                              case of error, or NULL if not required.
 *
 * @retval "SESSION_T *"        A pointer to a session handle.
 * @retval NULL                 If the session could not be created.
 */
SESSION_T *session_create(
        const char *url_str,
        const char *principal,
        CREDENTIALS_T *credentials,
        SESSION_LISTENER_T *listener,
        RECONNECTION_STRATEGY_T *reconnection_strategy,
        DIFFUSION_ERROR_T *error);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Create a new session and synchronously open a connection to the
 * Diffusion server at the endpoint specified by the URL.
 *
 * @note This call retains a reference to the memory allocated for the {@link
 * CREDENTIALS_T credentials} and it is the callers' responsibility to ensure
 * that this memory is not freed or overwritten for the lifetime of the
 * session structure.
 *
 * @note The error structure contains the result of the session creation
 * attempt and will not be overwritten if the session is subsequently
 * reconnected or fails over. Once session_create() returns, it is safe to
 * release any memory owned by the error structure.
 *
 * @param url_str               A URL describing the endpoint to connect to.
 * @param principal             Typically, the username of the connecting user or application.
 *                              NULL indicates that the principal will not be passed on connection.
 * @param credentials           The credentials associated with the principal, or NULL.
 *                              \em MUST be available for the duration of the session.
 * @param listener              A pointer to a {@link SESSION_LISTENER_T} structure containing callbacks
 *                              to be called on session-specific events.
 * @param reconnection_strategy How to handle reconnection situations when the server is initially
 *                              unavailable. If NULL, a default strategy is provided that
 *                              attempts to connect every 5 seconds for up to 1 minute.
 * @param user_context          User-supplied context for this session. It is the user's responsibility
 *                              to free any memory that this points to when the session is no longer required.
 * @param error                 A pointer to an error structure, initialized to zero, which is populated in
 *                              case of error, or NULL if not required.
 *
 * @retval "SESSION_T *"        A pointer to a session handle.
 * @retval NULL                 If the session could not be created.
 */
SESSION_T *session_create_with_user_context(
        const char *url_str,
        const char *principal,
        CREDENTIALS_T *credentials,
        SESSION_LISTENER_T *listener,
        RECONNECTION_STRATEGY_T *reconnection_strategy,
        void *user_context,
        DIFFUSION_ERROR_T *error);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Create a new session and synchronously open a connection to the
 * Diffusion server with a `DIFFUSION_SESSION_FACTORY_T`.
 *
 * @param session_factory   The session factory to initiate the session from
 * @param url_str           A URL describing the endpoint to connect to. This can be NULL
 *                          if the supplied `session_factory` sufficiently describes the
 *                          end point to connect to with `diffusion_session_factory_server_host`,
 *                          `diffusion_session_factory_server_port` and `diffusion_session_factory_secure_transport`
 *
 * @retval "SESSION_T *"    A pointer to a session handle.
 * @retval NULL             If the session could not be created.
 */
SESSION_T *session_create_with_session_factory(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        const char *url_str);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Create a new session, but do not wait for the connection handshake
 * to be performed.
 *
 * @note This call retains a reference to the memory allocated for the {@link
 * CREDENTIALS_T credentials}, and it is the callers' responsibility to ensure
 * that this memory is not freed or overwritten for the lifetime of the
 * session structure.
 *
 * @note The error structure contains the result of the session creation
 * attempt and will not be overwritten if the session is subsequently
 * reconnected or fails over. Since session_create_async() returns
 * immediately, you must ensure the memory that this parameter points to is
 * available until the session has been created, or the creation has
 * failed. It is highly recommended that the error callback (see the {@link
 * SESSION_CREATE_CALLBACK_T callbacks} parameter) is used instead, and NULL
 * passed here.
 *
 * @param url_str                   A URL describing the endpoint to connect to.
 * @param principal                 Typically, the username of the connecting user or application.
 *                                  NULL indicates that the principal will not be passed on connection.
 * @param credentials               The credentials associated with the principal, or NULL.
 *                                  \em MUST be available for the duration of the session.
 * @param listener                  A pointer to a \ref SESSION_LISTENER_T structure containing callbacks
 *                                  to be called on session-specific events.
 * @param reconnection_strategy     How to handle reconnection situations when the server is initially
 *                                  unavailable. If NULL, a default strategy is provided that
 *                                  attempts to connect every 5 seconds for up to 1 minute.
 * @param callbacks                 A pointer to a structure containing callbacks for successful
 *                                  connection or error reporting.
 * @param error                     A pointer to an error structure to be asynchronously populated if
 *                                  an error occurs while the session is established, or NULL if not required.
 *                                  \em MUST be available for the duration of the session.
 *
 * @retval "SESSION_T *"            A pointer to a session handle that should be closed and freed on exit.
 *                                  The session is not open when returned and you should use the
 *                                  {@link SESSION_CREATE_CALLBACK_T on_connected} callback to obtain a
 *                                  valid session. This handle is merely provided for proper cleanup.
 */
SESSION_T *session_create_async(
        const char *url_str,
        const char *principal,
        CREDENTIALS_T *credentials,
        SESSION_LISTENER_T *listener,
        RECONNECTION_STRATEGY_T *reconnection_strategy,
        SESSION_CREATE_CALLBACK_T *callbacks,
        DIFFUSION_ERROR_T *error);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Create a new session, but do not wait for the connection handshake
 * to be performed.
 *
 * @note This call retains a reference to the memory allocated for the {@link
 * CREDENTIALS_T credentials}, and it is the callers' responsibility to ensure
 * that this memory is not freed or overwritten for the lifetime of the
 * session structure.
 *
 * @note The error structure contains the result of the session creation
 * attempt and will not be overwritten if the session is subsequently
 * reconnected or fails over. Since session_create_async() returns
 * immediately, you must ensure the memory that this parameter points to is
 * available until the session has been created, or the creation has
 * failed. It is highly recommended that the error callback (see the {@link
 * SESSION_CREATE_CALLBACK_T callbacks} parameter) is used instead, and NULL
 * passed here.
 *
 * @param url_str                   A URL describing the endpoint to connect to.
 * @param principal                 Typically, the username of the connecting user or application.
 *                                  NULL indicates that the principal will not be passed on connection.
 * @param credentials               The credentials associated with the principal, or NULL.
 *                                  \em MUST be available for the duration of the session.
 * @param listener                  A pointer to a \ref SESSION_LISTENER_T structure containing callbacks
 *                                  to be called on session-specific events.
 * @param reconnection_strategy     How to handle reconnection situations when the server is initially
 *                                  unavailable. If NULL, a default strategy is provided that
 *                                  attempts to connect every 5 seconds for up to 1 minute.
 * @param callbacks                 A pointer to a structure containing callbacks for successful
 *                                  connection or error reporting.
 * @param user_context              User-supplied context for this session. It is the user's responsibility
 *                                  to free any memory that this points to when the session is no longer required.
 * @param error                     A pointer to an error structure to be asynchronously populated if
 *                                  an error occurs while the session is established, or NULL if not required.
 *                                  \em MUST be available for the duration of the session.
 *
 * @retval "SESSION_T *"            A pointer to a session handle that should be closed and freed on exit.
 *                                  The session is not open when returned and you should use the
 *                                  {@link SESSION_CREATE_CALLBACK_T on_connected} callback to obtain a
 *                                  valid session. This handle is merely provided for proper cleanup.
 */
SESSION_T *session_create_async_with_user_context(
        const char *url_str,
        const char *principal,
        CREDENTIALS_T *credentials,
        SESSION_LISTENER_T *listener,
        RECONNECTION_STRATEGY_T *reconnection_strategy,
        SESSION_CREATE_CALLBACK_T *callbacks,
        void *user_context,
        DIFFUSION_ERROR_T *error);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Create a new session and asynchronously open a connection to the
 * Diffusion server with a `DIFFUSION_SESSION_FACTORY_T`.
 *
 * @param session_factory       The session factory to initiate the session from
 * @param callbacks             A pointer to a structure containing callbacks for successful
 *                              connection or error reporting.
 * @param url_str               A URL describing the endpoint to connect to. This can be NULL
 *                              if the supplied `session_factory` sufficiently describes the
 *                              end point to connect to with `diffusion_session_factory_server_host`,
 *                              `diffusion_session_factory_server_port` and `diffusion_session_factory_secure_transport`
 *
 * @retval "SESSION_T *"        A pointer to a session handle.
 * @retval NULL                 If the session could not be created.
 */
SESSION_T *session_create_async_with_session_factory(
        const DIFFUSION_SESSION_FACTORY_T *session_factory,
        SESSION_CREATE_CALLBACK_T *callbacks,
        const char *url_str);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Free memory associated with a session.
 *
 * The session must be closed with session_close() prior to calling
 * this function. If session is NULL, the function returns
 * immediately.
 *
 * @param session   The session to be freed.
 */
void session_free(SESSION_T *session);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Stop accepting messages from a Diffusion server and close the connection.
 *
 * @param session   The session handle. If NULL, an error is returned.
 * @param error     A structure for storing error messages, or NULL if
 *                  detailed error reporting is not required.
 *
 * @retval int      0 on error
 * @retval int      1 on success
 */
int session_close(SESSION_T *session, DIFFUSION_ERROR_T *error);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Returns the current session state.
 *
 * @param session           The session handle. If NULL, \ref SESSION_STATE_UNKNOWN is returned.
 *
 * @retval SESSION_STATE_T  The session state, or \ref SESSION_STATE_UNKNOWN if session is NULL.
 */
SESSION_STATE_T session_state_get(SESSION_T *session);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Returns the last connection response code.
 *
 * @param session                       The session handle. May not be NULL.
 *
 * @retval CONNECTION_RESPONSE_CODE_T   The last connection response code.
 */
CONNECTION_RESPONSE_CODE_T session_connection_response_code(SESSION_T *session);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Returns a human-readable representation of the session state.
 *
 * @param state             A session state value.
 *
 * @return                  A textual representation of the session state.
 */
const char *session_state_as_string(const SESSION_STATE_T state);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Determines if a session is connected to a server.
 *
 * @param session           A session handle. A NULL value returns
 *                          \ref DIFFUSION_FALSE.
 *
 * @retval DIFFUSION_TRUE	If the session is connected.
 * @retval DIFFUSION_FALSE  If the session is not connected.
 */
int session_is_connected(const SESSION_T *session);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Determines if a session is in a recovering state, where it is trying
 * to reconnect to a server.
 *
 * @param session           A session handle. A NULL value returns \ref DIFFUSION_FALSE.
 *
 * @retval DIFFUSION_TRUE   If the session is recovering.
 * @retval DIFFUSION_FALSE  If the session is not recovering.
 */
int session_is_recovering(const SESSION_T *session);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Determines if a session is closed.
 *
 * @param session           A session handle. A NULL value returns \ref DIFFUSION_TRUE.
 *
 * @retval DIFFUSION_TRUE   If the session is closed.
 * @retval DIFFUSION_FALSE  If the session is not closed.
 */
int session_is_closed(const SESSION_T *session);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Takes a session ID and produces a human-readable string. It is the
 * responsibility of the caller to free the memory allocated for the returned
 * string.
 *
 * @param session_id    A \ref SESSION_ID_T. If the session ID is NULL, this function
 *                      returns NULL.
 *
 * @retval "char *"     A string containing a textual representation of the session id.
 * @retval NULL         If an error occurs.
 */
char *session_id_to_string(const SESSION_ID_T *session_id);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Given a session id in string form, this function returns it to the
 * internal structure form.
 *
 * @param str                   The session id as a string. If NULL, this function returns NULL.
 *
 * @retval "SESSION_ID_T *"     A populated session id structure.
 * @retval NULL                 If an error occurs.
 */
SESSION_ID_T *session_id_create_from_string(const char *str);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Free memory associated with a session ID structure.
 *
 * @param session_id    The session id structure. If NULL, this function has no effect.
 */
void session_id_free(SESSION_ID_T *session_id);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Compare two session IDs.
 *
 * @param s1    The first session ID to compare.
 * @param s2    The second session ID to compare.
 *
 * @retval 0    if the session IDs are equal.
 * @retval 1    if only their server instances are equal.
 * @retval -1   otherwise.
 */
int session_id_cmp(const SESSION_ID_T s1, const SESSION_ID_T s2);


/**
 * @ingroup PublicAPI_Session
 *
 * @brief Sets the handler for topics received but not subscribed to.
 *
 * If a topic message is received that does not have a registered
 * handler, it is passed to the global topic handler. This function
 * allows you to override the default handler (which does nothing).
 *
 * @param session   The session handle. <em>MUST NOT</em> be NULL.
 * @param handler   Pointer to a function which will receive the
 *                  unhandled messages, or NULL to reset to the default handler.
 *
 * @retval          The previous topic handler.
 */
TOPIC_HANDLER_T set_global_topic_handler(SESSION_T *session, const TOPIC_HANDLER_T handler);

#endif
