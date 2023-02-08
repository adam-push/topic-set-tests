#ifndef _diffusion_system_authentication_control_h_
#define _diffusion_system_authentication_control_h_ 1

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
 * @file system-authentication-control.h
 *
 * Allows the update of the Server system authentication store, which controls principals and trusted proposed session properties.
 */

#include "session.h"

/**
 * @brief Structure describing a principal in the authentication store.
 */
typedef struct system_principal_s {
        const char *name;
        const SET_T *role_set;
        const char *locking_principal;
} SYSTEM_PRINCIPAL_T;

/**
 * @brief Action to be taken by the system authentication handler for
 * connection attempts that do not provide a principal name and
 * credentials.
 */
typedef enum {
        /// Accept anonymous connection attempts.
        ANONYMOUS_CONNECTION_ACTION_ALLOW = 0,
        /// Deny anonymous connection attempts.
        ANONYMOUS_CONNECTION_ACTION_DENY = 1,
        /// Defer authentication decision for anonymous connection
        /// attempts to subsequent authentication handlers.
        ANONYMOUS_CONNECTION_ACTION_ABSTAIN = 2
} ANONYMOUS_CONNECTION_ACTION_T;

typedef struct system_authentication_store_s {
        /// A hash map of the server's principals
        /// mapped against `SYSTEM_PRINCIPAL_T`
        /// as its value.
        HASH_T *system_principals;
        /// The action to be taken for anonymous connection attempts.
        ANONYMOUS_CONNECTION_ACTION_T anonymous_connection_action;
        /// The roles that the system authentication handler will assign to anonymous sessions.
        SET_T *anon_roles;
} SYSTEM_AUTHENTICATION_STORE_T;

/**
 * @brief Callback for get_system_authentication_store().
 *
 * @param session The current active session.
 * @param store The response containing the authentication store contents. This structure will be
 * freed on exit so a copy should be taken if it needs to be used outside of the scope of the callback.
 * @param context User-supplied context from the get_system_authentication_store() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_get_system_authentication_store_cb)(SESSION_T *session,
                                                     const SYSTEM_AUTHENTICATION_STORE_T store,
                                                     void *context);

#define GET_SYSTEM_AUTHENTICATION_STORE_HANDLERS                        \
        /** Received authentication store */                            \
        on_get_system_authentication_store_cb on_get; \
        /** Standard service error handler. */                          \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard handler. */                        \
        DISCARD_HANDLER_T on_discard;

/**
 * @brief Structure passed when requesting the system authentication store.
 */
typedef struct get_system_authentication_store_params_s {
        GET_SYSTEM_AUTHENTICATION_STORE_HANDLERS
        /// User-supplied context returned to callbacks.
        void *context;
} GET_SYSTEM_AUTHENTICATION_STORE_PARAMS_T;

/**
 * @brief Callback for update_system_authentication_store().
 *
 * @param session The current active session.
 * @param error_reports Any errors that have occurred while processing the update.
 * @param context User-supplied context from the update_system_authentication_store() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_update_system_authentication_store_cb)(SESSION_T *session,
                                                        const LIST_T *error_reports,
                                                        void *context);

#define UPDATE_SYSTEM_AUTHENTICATION_STORE_HANDLERS             \
        /** Update received by Diffusion */                     \
        on_update_system_authentication_store_cb on_update;     \
        /** Standard service error handler. */                  \
        ERROR_HANDLER_T on_error;                               \
        /** Standard service discard handler. */                \
        DISCARD_HANDLER_T on_discard;

/**
 * @brief Structure passed when updating the system authentication store.
 */
typedef struct update_system_authentication_store_params_s {
        UPDATE_SYSTEM_AUTHENTICATION_STORE_HANDLERS
        /// Update script to apply to the system authentication store.
        SCRIPT_T *update_script;
        /// User-supplied context returned to callbacks.
        void *context;
} UPDATE_SYSTEM_AUTHENTICATION_STORE_PARAMS_T;

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Create a new, empty system authentication store structure.
 *
 * The returned structure is unsynchronized, so appropriate mutexes should be
 * use if the structure is to be used concurrently.
 *
 * This should be freed wth `system_authentication_store_free` when no longer needed.
 *
 * @return A newly allocated system authentication store.
 */
SYSTEM_AUTHENTICATION_STORE_T *system_authentication_store_create(void);

/**
 * @brief Free all memory associated with a system authentication store structure.
 *
 * @param store The system authentication store to be freed.
 */
void system_authentication_store_free(SYSTEM_AUTHENTICATION_STORE_T *store);

/**
 * @brief Free all memory associated with a system principal store structure.
 *
 * @param system_principal The system principal store to be freed.
 */
void system_principal_free(SYSTEM_PRINCIPAL_T *system_principal);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Copy an existing system authentication store structure. `system_authentication_store_free`
 *        should be called on this pointer when no longer needed.
 *
 * @param store The system authentication store to be copied.
 *
 * The returned structure is unsynchronized, so appropriate mutexes should be
 * use if the structure is to be used concurrently.
 *
 * @return A deep copy of a system authentication store.
 */
SYSTEM_AUTHENTICATION_STORE_T *system_authentication_store_dup(const SYSTEM_AUTHENTICATION_STORE_T *store);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Copy an existing system principal structure. `system_principal_free` should be
 *        called on this pointer when no longer needed.
 *
 * @param system_principal The system principal to be copied.
 *
 * The returned structure is unsynchronized, so appropriate mutexes should be
 * use if the structure is to be used concurrently.
 *
 * @return A deep copy of a system principal
 */
SYSTEM_PRINCIPAL_T *system_principal_dup(const SYSTEM_PRINCIPAL_T *system_principal);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Get the server's system authentication store.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure containing callbacks to receive the
 *                      contents of the system authentication store.
 */
void get_system_authentication_store(SESSION_T *session, const GET_SYSTEM_AUTHENTICATION_STORE_PARAMS_T params);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Get an array of principals defined in the system authentication store.
 *
 * @param store         A system authentication store.
 *
 * @return A NULL-terminated array of strings.
 */
char **get_principal_names(const SYSTEM_AUTHENTICATION_STORE_T store);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Get an array of role names for a given principal.
 *
 * @param store         A system authentication store.
 * @param principal     The principal for which roles are requested.
 *
 * @return A NULL-terminated array of strings.
 */
char **get_roles_for_principal(const SYSTEM_AUTHENTICATION_STORE_T store, const char *principal);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Get an array of role names for anonymous connections.
 *
 * @param store         A system authentication store.
 *
 * @return A NULL-terminated array of strings.
 */
char **get_anonymous_roles(const SYSTEM_AUTHENTICATION_STORE_T store);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Send updates to the system authentication store.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing the updates
 *                      to perform and callbacks to handle success
 *                      or failure notifications.
 */
void update_system_authentication_store(SESSION_T *session, const UPDATE_SYSTEM_AUTHENTICATION_STORE_PARAMS_T params);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Updates a script to add a command to deny anonymous connections.
 *
 * @param script        The script to update.
 * @return The updated script. (It is also modified in-place.)
 */
SCRIPT_T *update_auth_store_deny_anonymous_connections(SCRIPT_T *script);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Updates a script to add a command to abstain from decisions
 * relating to anonymous connections.
 *
 * @param script        The script to update.
 * @return The updated script. (It is also modified in-place.)
 */
SCRIPT_T *update_auth_store_abstain_anonymous_connections(SCRIPT_T *script);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Updates a script to add a command to allow anonymous connections,
 * and a list of roles those connections should have.
 *
 * @param script        The script to update.
 * @param roles         A list of strings representing the roles to be
 *                      assigned to the anonymous user.
 * @return The updated script. (It is also modified in-place.)
 */
SCRIPT_T *update_auth_store_allow_anonymous_connections(SCRIPT_T *script, const LIST_T *roles);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Updates a script to add a command to add a new principal to
 * the authentication store, and a list of roles assigned to them.
 *
 * @param script        The script to update.
 * @param principal     The principal to add.
 * @param password      The unencoded password for the principal.
 * @param roles         A list of strings representing the roles to be
 *                      assigned to the principal.
 * @return The updated script or NULL if either the principal or password are NULL.
 */
SCRIPT_T *update_auth_store_add_principal(SCRIPT_T *script, const char *principal, const char *password, const LIST_T *roles);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Updates a script to add a command to add a new locked principal to
 * the authentication store.
 *
 * A locked principal can only be edited by the principal defined in the
 * lock. The script will fail if the principal is already defined at the
 * server.
 *
 * @param script              The script to update.
 * @param principal           The principal to add.
 * @param password            The unencoded password for the principal.
 * @param roles               A list of strings representing the roles to be
 *                            assigned to the principal.
 * @param locking_principal   The name of the principal that can edit this principal
 * @return The updated script or NULL if either the principal or password are NULL.
 */
SCRIPT_T *update_auth_store_add_locked_principal(SCRIPT_T *script, const char *principal, const char *password, const LIST_T *roles, const char *locking_principal);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Updates a script to add a command to remove an existing
 * principal from the authentication store.
 *
 * @param script        The script to update.
 * @param principal     The principal to remove.
 * @return The updated script or NULL if the principal is NULL.
 */
SCRIPT_T *update_auth_store_remove_principal(SCRIPT_T *script, const char *principal);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Updates a script to assign roles to an existing principal.
 *
 * @param script        The script to update.
 * @param principal     The principal whose roles are to be updated.
 * @param roles         A list of strings representing the roles to be
 *                      assigned to the principal.
 * @return The updated script or NULL if the principal is NULL.
 */
SCRIPT_T *update_auth_store_assign_roles(SCRIPT_T *script, const char *principal, const LIST_T *roles);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Updates a script to change the password for an existing principal.
 *
 * @param script        The script to update.
 * @param principal     The principal whose password is to be changed.
 * @param password      The unencoded password for the principal.
 *
 * @return The updated script. (It is also modified in-place.)
 */
SCRIPT_T *update_auth_store_set_password(SCRIPT_T *script, const char *principal, const char *password);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Updates a script to include a command to verify a
 * principal's password.
 *
 * This command does not update the store. It can be used in
 * conjunction with setPassword to create a script that updates a
 * password only if the previous password is supplied.
 *
 * @param script        The script to update.
 * @param principal     The principal whose password is to be verified.
 * @param password      The unencoded password for the principal.
 *
 * @return The updated script. (It is also modified in-place.)
 */
SCRIPT_T *update_auth_store_verify_password(SCRIPT_T *script, const char *principal, const char *password);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Updates a script to specify a client proposed session property and
 * its allowed values.
 *
 * Specifies the name of a client proposed session property that should be
 * allowed by the system authenticator along with a set of permissible values.
 * The property will only be allowed if the supplied value matches one of those
 * in the set of values specified.
 *
 * @param script          The script to update.
 * @param property_name   The name of the client proposed property to be allowed
 * @param allowed_values  The set of allowed values for the client proposed property
 *
 * @return The updated script. (It is also modified in-place.)
 */
SCRIPT_T *update_auth_store_trust_client_proposed_property_in(SCRIPT_T *script, const char *property_name, const SET_T *allowed_values);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Updates a script to specify a client proposed session property and
 * a regular expression to validate its value.
 *
 * Specifies the name of a client proposed session property that should be
 * allowed by the system authenticator along with a regular expression to
 * validate the property value.
 * The property will only be allowed if the supplied value matches with the regular expression.
 *
 * @param script              The script to update.
 * @param property_name       The name of the client proposed property to be allowed
 * @param regular_expression  The regular expression which will be matched against
                              supplied values to determine whether they are valid
 *
 * @return The updated script. (It is also modified in-place.)
 */
SCRIPT_T *update_auth_store_trust_client_proposed_property_matches(SCRIPT_T *script, const char *property_name, const char *regular_expression);

/**
 * @ingroup PublicAPI_SystemAuthenticationControl
 *
 * @brief Updates a script to specify that a client proposed session property should
 * be ignored.
 *
 * Specifies the name of a client proposed session property that should be
 * ignored by the system authenticator.
 * This cancels the effect of a previous trust of the named property.
 *
 * @param script              The script to update.
 * @param property_name       The name of the client proposed property to be ignored
 *
 * @return The updated script. (It is also modified in-place.)
 */
SCRIPT_T *update_auth_store_ignore_client_proposed_property(SCRIPT_T *script, const char *property_name);

#endif
