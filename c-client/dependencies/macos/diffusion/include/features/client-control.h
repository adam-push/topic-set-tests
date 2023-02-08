#ifndef _diffusion_client_control_h_
#define _diffusion_client_control_h_ 1

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
 * @file client-control.h
 *
 * Allows access to other sessions' properties and related operations for session management.
 */

#include "session.h"
#include "diffusion-api-error.h"

#define PROPERTIES_SELECTOR_ALL_FIXED_PROPERTIES "*F"
#define PROPERTIES_SELECTOR_ALL_USER_PROPERTIES "*U"

typedef enum {
        /// Indicates a session has been opened (e.g. new client
        /// connection).
        SESSION_PROPERTIES_EVENT_TYPE_OPEN = 0,
        /// Indicates that a session has updated properties.
        SESSION_PROPERTIES_EVENT_TYPE_UPDATE = 1,
        /// Indicates that a session has been closed.
        SESSION_PROPERTIES_EVENT_TYPE_CLOSE = 2,
        /// Indicates that a session has been disconnected
        SESSION_PROPERTIES_EVENT_TYPE_DISCONNECTED = 3
} SESSION_PROPERTIES_EVENT_TYPE_T;

typedef enum {
        /// The session properties have been updated.
        SESSION_PROPERTIES_UPDATE_TYPE_UPDATED = 0,
        /// The session properties have been updated due to
        /// reconnection with the server.
        SESSION_PROPERTIES_UPDATE_TYPE_RECONNECTED = 1,
        /// The session properties have been updated due to the
        /// session failing over to a different host.
        SESSION_PROPERTIES_UPDATE_TYPE_FAILED_OVER = 2,
        /// Session has disconnected
        SESSION_PROPERTIES_UPDATE_TYPE_DISCONNECTED = 3
} SESSION_PROPERTIES_UPDATE_TYPE_T;

typedef struct session_properties_event_s {
        /// The reason why a property event was triggered.
        SESSION_PROPERTIES_EVENT_TYPE_T type;
        /// If the session was closed, this indicates the reason for
        /// closure.
        SESSION_CLOSE_REASON_T close_reason;
        /// If the session properties were updated, this indicates the
        /// reason for the change.
        SESSION_PROPERTIES_UPDATE_TYPE_T update_type;
        /// The session ID of the client session.
        SESSION_ID_T session_id;
        /// A key/value map of properties of the client, as requested
        /// when registering the session properties listener.
        HASH_T *properties;
} SESSION_PROPERTIES_EVENT_T;

typedef int (*on_session_properties_registered_cb)(SESSION_T *session, void *context);

/// Callbacks for session open/close and property updates have this
/// function signature.
typedef int (*on_session_properties_event_cb)(SESSION_T *session,
                                              const SESSION_PROPERTIES_EVENT_T *event,
                                              void *context);

#define SESSION_PROPERTIES_REGISTRATION_HANDLERS                        \
        /** Callback fired when the listener has been registered. */    \
        on_session_properties_registered_cb on_registered;              \
        /** Error callback if an error occurs during listener registration.  */ \
        ERROR_HANDLER_T on_registration_error;                          \
        /** Standard discard handler. */                                \
        DISCARD_HANDLER_T on_discard;

#define SESSION_PROPERTIES_EVENT_HANDLERS                               \
        /** Callback fired when a new client session is opened. */      \
        on_session_properties_event_cb on_session_open;                 \
        /** Callback fired when an existing client session closes. */   \
        on_session_properties_event_cb on_session_close;                \
        /** Callback fired when a client's session detail are updated. */ \
        on_session_properties_event_cb on_session_update;               \
        /** Callback fired when a client was disconnected. */           \
        on_session_properties_event_cb on_session_disconnect;           \
        /** Error callback if an error occurs with a session update.  */ \
        ERROR_HANDLER_T on_session_error;

typedef struct session_properties_registration_handlers_s {
        SESSION_PROPERTIES_REGISTRATION_HANDLERS
        SESSION_PROPERTIES_EVENT_HANDLERS
} SESSION_PROPERTIES_REGISTRATION_HANDLERS_T;

typedef struct session_properties_registration_params_s {
        SESSION_PROPERTIES_REGISTRATION_HANDLERS
        SESSION_PROPERTIES_EVENT_HANDLERS
        /// A set containing properties of clients to be reported back
        /// to the listener. The properties are NULL-terminated
        /// strings, or the constants PROPERTIES_SELECTOR_ALL_FIXED_PROPERTIES or
        /// PROPERTIES_SELECTOR_ALL_USER_PROPERTIES.
        SET_T *required_properties;
        /// User-supplied context to return to callbacks.
        void *context;
} SESSION_PROPERTIES_REGISTRATION_PARAMS_T;

typedef struct svc_get_session_properties_response_s {
        /// Returned properties for the session.
        HASH_T *properties;
} SVC_GET_SESSION_PROPERTIES_RESPONSE_T;

typedef int (*get_session_properties_cb)(SESSION_T *session,
                                         const SVC_GET_SESSION_PROPERTIES_RESPONSE_T *response,
                                         void *context);

/**
 * @brief Callback when a session's session properties have been set.
 *
 * @param properties A map of session properties that have changed.
 * @param context    User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*set_session_properties_cb)(const HASH_T *properties, void *context);

/**
 * @brief Callback when all sessions matching the filter have their session properties set.
 *
 * @param response   The response from the server
 * @param context    User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*set_session_properties_filter_cb)(const int number_of_matching_sessions, void *context);

/**
 * @brief Callback when a session's roles have been successfully changed.
 *
 * @param context User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*change_roles_with_session_id_cb)(void *context);

/**
 * @brief Callback when a session's roles have been successfully changed.
 *
 * @param number_of_matching_sessions An integer value which represents a number of sessions that have
 *                                    matched the filter and for which the specified role changes have
 *                                    been applied.
 * @param context                     User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*change_roles_with_filter_cb)(int number_of_matching_sessions, void *context);

/**
 * @brief Callback when client sessions matching the filter have been closed.
 *
 * @param number_of_matching_sessions An integer value which represents a number of sessions that have
 *                                    matched the filter and for which sessions have been closed
 * @param context                     User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*client_close_with_filter_cb)(int number_of_matching_sessions, void *context);

/**
 * @brief Callback when client session has been closed.
 *
 * @param context                     User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*client_close_with_session_cb)(void *context);

/**
 * @brief Callback when client sessions matching the filter have had conflation set to true or false.
 *
 * @param number_of_matching_sessions An integer value which represents a number of sessions that have
 *                                    matched the filter and all for which sessions conflation has been
 *                                    set to true or false.
 * @param context                     User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*client_conflated_set_with_filter_cb)(int number_of_matching_sessions, void *context);

#define GET_SESSION_PROPERTIES_HANDLERS                             \
        /** Callback fired when session properties are received. */ \
        get_session_properties_cb on_session_properties;            \
        /** Error callback. */                                      \
        ERROR_HANDLER_T on_error;                                   \
        /** Discard callback. */                                    \
        DISCARD_HANDLER_T on_discard;

typedef struct get_session_properties_params_s {
        GET_SESSION_PROPERTIES_HANDLERS
        /// The session ID of the connected client whose properties we
        /// are requesting.
        SESSION_ID_T *session_id;
        /// A set containing properties of clients to be reported back
        /// to the listener. The properties are NULL-terminated
        /// strings, or the constants PROPERTIES_SELECTOR_ALL_FIXED_PROPERTIES or
        /// PROPERTIES_SELECTOR_ALL_USER_PROPERTIES.
        SET_T *required_properties;
        /// User-supplied context to return to callbacks.
        void *context;
} GET_SESSION_PROPERTIES_PARAMS_T;

/**
 * Structure supplied when issuing a `diffusion_set_session_properties` call.
 */
typedef struct diffusion_set_session_properties_params_s {
        /// Identifies the client session for which the
        /// change will be applied.
        const SESSION_ID_T *session_id;
        /// The properties to change. Each entry in the map is a
        /// property name and the new value. If the value is `NULL`, any
        /// existing property with that name will be removed (unless it is a fixed
        /// property). Otherwise if the property name does not match any existing
        /// property, that entry will be added as a new property (although
        // properties starting with $ will be ignored).
        const HASH_T *properties;
        /// Callback when the roles have been successfully changed.
        set_session_properties_cb on_session_properties_set;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_SET_SESSION_PROPERTIES_PARAMS_T;

/**
 * Structure supplied when issuing a `diffusion_set_session_properties_filter` call.
 */
typedef struct diffusion_set_session_properties_filter_params_s {
        /// The session filter.
        const char *filter;
        /// The properties to change. Each entry in the map is a
        /// property name and the new value. If the value is `NULL`, any
        /// existing property with that name will be removed (unless it is a fixed
        /// property). Otherwise if the property name does not match any existing
        /// property, that entry will be added as a new property (although
        // properties starting with $ will be ignored).
        const HASH_T *properties;
        /// Callback when the roles have been successfully changed.
        set_session_properties_filter_cb on_session_properties_set;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_SET_SESSION_PROPERTIES_FILTER_PARAMS_T;

/**
 * Structure supplied when issuing a `diffusion_change_roles_with_session_id` call.
 */
typedef struct diffusion_change_roles_with_session_id_params_s {
        /// Identifies the client session for which the
        /// change will be applied.
        const SESSION_ID_T *session_id;
        /// A set of roles to be removed from the session. If
        /// one or more roles from the list are not currently assigned,
        /// they are ignored.
        const SET_T *roles_to_remove;
        /// A set of roles to be added to the session. If one or
        /// more roles from the list are already assigned, they are ignored.
        const SET_T *roles_to_add;
        /// Callback when the roles have been successfully changed.
        change_roles_with_session_id_cb on_roles_changed;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_CHANGE_ROLES_WITH_SESSION_ID_PARAMS_T;

/**
 * Structure supplied when issuing a `diffusion_change_roles_with_filter` call.
 */
typedef struct diffusion_change_roles_params_with_filter_s {
        /// Identifies the set of client sessions for which the change
        /// will be applied. For details on how to specify session filters see
        /// `session.h`.
        const char *filter;
        /// A set of roles to be removed from the session. If
        /// one or more roles from the list are not currently assigned,
        /// they are ignored.
        const SET_T *roles_to_remove;
        /// A set of roles to be added to the session. If one or
        /// more roles from the list are already assigned, they are ignored.
        const SET_T *roles_to_add;
        /// Callback when the roles have been successfully changed.
        change_roles_with_filter_cb on_roles_changed;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_CHANGE_ROLES_WITH_FILTER_PARAMS_T;

/**
 * Structure supplied when issuing a `diffusion_client_close_with_filter` call.
 */
typedef struct diffusion_client_close_with_filter_params_s {
        /// Identifies the set of client sessions for which the change
        /// will be applied. For details on how to specify session filters see
        /// `session.h`
        const char *filter;
        /// Callback when the sessions matching the filter have been closed.
        client_close_with_filter_cb on_clients_closed;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_CLIENT_CLOSE_WITH_FILTER_PARAMS_T;

/**
 * Structure supplied when issuing a `diffusion_client_close_with_session` call.
 */
typedef struct diffusion_client_close_with_session_params_s {
        /// The session_id to be closed by the call.
        SESSION_ID_T *session_id;
        /// Callback when the sessions matching the filter have been closed.
        client_close_with_session_cb on_closed;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_CLIENT_CLOSE_WITH_SESSION_PARAMS_T;

/**
 * Structure supplied when issuing a `diffusion_client_set_conflated_with_filter` call.
 */
typedef struct diffusion_client_set_conflated_with_filter_params_s {
        /// Identifies the set of client sessions for which the change
        /// will be applied. For details on how to specify session filters see
        /// `session.h`
        const char *filter;
        /// `true` to enable conflate, `false to disable conflation.
        bool conflate;
        /// Callback when the sessions matching the filter have had conflation
        /// enabled or disabled.
        client_conflated_set_with_filter_cb on_clients_conflated_set;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_CLIENT_SET_CONFLATED_WITH_FILTER_PARAMS_T;

/**
 * @ingroup PublicAPI_ClientControl
 *
 * @brief Register a listener for changes to other clients' properties.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing which properties should
 *                      be reported, and the callbacks to be invoked.
 */
void session_properties_listener_register(SESSION_T *session, const SESSION_PROPERTIES_REGISTRATION_PARAMS_T params);

/**
 * @ingroup PublicAPI_ClientControl
 *
 * @brief Request properties of a connected client.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing which client and
 *                      properties we're interested in, and the callbacks
 *                      to be invoked.
 */
void get_session_properties(SESSION_T *session, const GET_SESSION_PROPERTIES_PARAMS_T params);

/**
 * @ingroup PublicAPI_ClientControl
 *
 * @brief Send a request to the server to change the user-defined session
 *        properties for a session.
 *
 * It is also permissible to change the values of the following fixed
 * session properties :-
 *
 * $Country - will be normalised to upper case
 * $Language - will be normalised to lower case
 * $Latitude - Invalid value will be set to "NaN"
 * $Longitude - Invalid value will be set to "NaN"
 *
 * If values are provided for any other fixed session properties they will
 * be ignored.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameters defining the `diffusion_set_session_properties` request
 *                      and callbacks.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_set_session_properties(SESSION_T *session, const DIFFUSION_SET_SESSION_PROPERTIES_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_ClientControl
 *
 * @brief Send a request to the server to set all sessions that satisfy a session
 *        filter with the new user-defined session properties.
 *
 * It is also permissible to change the values of the following fixed
 * session properties :-
 *
 * $Country - will be normalised to upper case
 * $Language - will be normalised to lower case
 * $Latitude - Invalid value will be set to "NaN"
 * $Longitude - Invalid value will be set to "NaN"
 *
 * If values are provided for any other fixed session properties they will
 * be ignored.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameters defining the `diffusion_set_session_properties_filter` request
 *                      and callbacks.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_set_session_properties_filter(SESSION_T *session, const DIFFUSION_SET_SESSION_PROPERTIES_FILTER_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_ClientControl
 *
 * @brief Changes the assigned roles of another session.
 *
 * Initially a session has a set of roles assigned during authentication.
 * The set of assigned roles can be obtained from the session's `$Roles`
 * session property.
 *
 * When a session's assigned roles change, its `$Roles` property changes
 * accordingly. Changing the assigned roles can change the `READ_TOPIC`
 * permissions granted to the session. The session's subscriptions will be
 * updated accordingly.
 *
 * The same role must not occur in both `params.roles_to_remove` and
 * `params.roles_to_add` sets. Either set can be an empty (or `NULL`) set but not both.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing which client and
 *                      properties we're interested in, and the callbacks
 *                      to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_change_roles_with_session_id(SESSION_T *session, const DIFFUSION_CHANGE_ROLES_WITH_SESSION_ID_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_ClientControl
 *
 * @brief Allows a session to change the assigned roles of all sessions that
 * satisfy a given session filter.
 *
 * Initially a session has a set of roles assigned during authentication.
 * A current roles set can be obtained from the `$Roles` session property.
 *
 * When a set of session roles changes, its `$Roles` property changes
 * accordingly. As a role can constrain 'topic' permissions, session
 * subscriptions are re-evaluated based on the new roles set.
 *
 * The same role must not occur in both `params.roles_to_remove` and
 * `params.roles_to_add` sets. Either set can be an empty (or `NULL`) set but not both.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing which client and
 *                      properties we're interested in, and the callbacks
 *                      to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_change_roles_with_filter(SESSION_T *session, const DIFFUSION_CHANGE_ROLES_WITH_FILTER_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_ClientControl
 *
 * @brief Close a client session.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing which client and
 *                      properties we're interested in, and the callbacks
 *                      to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_client_close_with_session(SESSION_T *session, const DIFFUSION_CLIENT_CLOSE_WITH_SESSION_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_ClientControl
 *
 * @brief Close all client sessions matching a given session filter.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing which client and
 *                      properties we're interested in, and the callbacks
 *                      to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_client_close_with_filter(SESSION_T *session, const DIFFUSION_CLIENT_CLOSE_WITH_FILTER_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_ClientControl
 *
 * @brief Control client queue conflation.
 *
 * Each session begins with conflation enabled or disabled based on the
 * queue configuration of the connector it is using. This method allows
 * conflation to be enabled or disabled for a set of sessions matching a
 * filter at runtime. For more detail on specifying session filters see
 * `session.h`
 *
 * Conflation is the process of merging or discarding topic updates queued
 * for a session to reduce the server memory footprint and network data.
 * Conflation needs to be enabled for a session and a policy configured for
 * the topic to have an effect.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing which client and
 *                      properties we're interested in, and the callbacks
 *                      to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_client_set_conflated_with_filter(SESSION_T *session, const DIFFUSION_CLIENT_SET_CONFLATED_WITH_FILTER_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

#endif
