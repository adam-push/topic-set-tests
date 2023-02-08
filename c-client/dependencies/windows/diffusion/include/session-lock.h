#ifndef _diffusion_session_lock_h_
#define _diffusion_session_lock_h_ 1

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
 * @file session-lock.h
 *
 * Functions for creating and managing session locks.
 */
#include <stdlib.h>
#include <stdbool.h>

#include "session.h"

typedef enum {
        /**
         * The lock will be released when the acquiring session loses its
         * current connection to the server.
         */
        UNLOCK_ON_CONNECTION_LOSS,
        /**
         * The lock will be released when the acquiring session is closed.
         */
        UNLOCK_ON_SESSION_LOSS
} DIFFUSION_SESSION_LOCK_SCOPE;

/**
 * An opaque session lock struct.
 */
typedef struct DIFFUSION_SESSION_LOCK_T DIFFUSION_SESSION_LOCK_T;

/**
 * @brief Callback when a session lock has been acquired.
 *
 * @param session_lock The session lock
 * @param context      User supplied context passed in from `DIFFUSION_SESSION_LOCK_PARAMS_T`
 *
 * @return HANDLERS_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_lock_acquired_cb)(const DIFFUSION_SESSION_LOCK_T *session_lock, void *context);

/**
 * @brief Callback when a session lock has been unlocked.
 *
 * @param lock_owned   A true value indicates this session previously owned the lock and a false value
 *                     indicates it did not.
 * @param context      User supplied context passed in from `DIFFUSION_SESSION_LOCK_UNLOCK_PARAMS_T`
 *
 * @return HANDLERS_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_unlock_cb)(bool lock_owned, void *context);

/**
 * @brief Structure describing a session lock request.
 */
typedef struct diffusion_session_lock_params_s {
        /// Callback when a session lock
        /// has been returned
        on_lock_acquired_cb on_lock_acquired;
        /// Callback on error
        ERROR_HANDLER_T on_error;
        /// Callback on discard
        DISCARD_HANDLER_T on_discard;
        /// User supplied context
        void *context;
} DIFFUSION_SESSION_LOCK_PARAMS_T;

/**
 * @brief Structure describing a session lock unlock request.
 */
typedef struct diffusion_session_lock_unlock_params_s {
        /// Callback when a session lock
        /// has been unlocked
        on_unlock_cb on_unlock;
        /// Callback on error
        ERROR_HANDLER_T on_error;
        /// Callback on discard
        DISCARD_HANDLER_T on_discard;
        /// User supplied context
        void *context;
} DIFFUSION_SESSION_LOCK_UNLOCK_PARAMS_T;

/**
 * @brief Returns a copy of a lock's name
 *
 * @param session_lock the session lock to retrieve its name from.
 *
 * @return the session lock's name
 */
char *diffusion_session_lock_get_name(const DIFFUSION_SESSION_LOCK_T *session_lock);

/**
 * @brief A value that identifying the acquisition of the lock with the
 * given name. Session locks that are acquired later are guaranteed to have
 * bigger sequence values, allowing the sequence number to be used
 * as a fencing token.
 *
 * @param session_lock the session lock to retrieve its sequence number from
 *
 * @return a value that identifies the acquisition of this lock
 */
long diffusion_session_lock_get_sequence(const DIFFUSION_SESSION_LOCK_T *session_lock);

/**
 * @brief Test whether the session lock is still owned.
 *
 * @param session_lock the session lock to verify if still owned
 *
 * @return true if the session lock is still owned by the session
 */
bool diffusion_session_lock_is_owned(const DIFFUSION_SESSION_LOCK_T *session_lock);

/**
 * @brief The scope of the lock.
 *
 * The scope determines when the lock will be released automatically.
 *
 * If a session makes multiple `diffusion_session_lock_with_scope` requests for a lock
 * using different scopes, and the server assigns the lock to the session
 * fulfilling the requests, the lock will be given the weakest scope
 * `UNLOCK_ON_CONNECTION_LOSS`. Consequently, an individual request can
 * complete with a lock that has a different scope to that requested.
 *
 * @param session_lock The session lock to retrieve its scope from
 *
 * @return the lock scope
 */
DIFFUSION_SESSION_LOCK_SCOPE diffusion_session_lock_get_scope(const DIFFUSION_SESSION_LOCK_T *session_lock);

/**
 * @brief Attempt to acquire a {@link DIFFUSION_SESSION_LOCK_T session lock}.
 *
 * Acquiring the lock can take an arbitrarily long time if other sessions
 * are competing for the lock. The server will retain the session's request
 * for the lock until it is assigned to the session or the session is closed
 *
 * A session can call this method multiple times. If the lock is acquired,
 * all calls will complete successfully with equal SessionLocks.
 *
 * A session that acquires a lock will remain its owner until it is
 * unlocked with `diffusion_session_lock_unlock` or the session closes. The
 * `diffusion_session_lock_with_scope` variant of this method takes a
 * scope parameter that provides the further option of releasing the lock
 * when the session loses its connection to the server.
 *
 * To allow fine-grained access control, lock names are interpreted as path
 * names, controlled with the `ACQUIRE_LOCK` path permission. This allows
 * permission to be granted to a session to acquire the lock `update-topic/a`
 * while preventing the session from acquiring the lock `update-topic/b`, for example.
 *
 * @param session   The session handle. If NULL, the function returns immediately.
 * @param lock_name The name of the session lock
 * @param params    Parameters describing the session lock request and
 *                  callbacks handlers which may be invoked in response.
 */
void diffusion_session_lock(SESSION_T *session, const char *lock_name, DIFFUSION_SESSION_LOCK_PARAMS_T params);

/**
 * @brief Variant of `diffusion_session_lock` that provides control over when a lock
 * will be released.
 *
 * If called with `UNLOCK_ON_SESSION_LOSS` this method behaves exactly like
 * `diffusion_session_lock`.
 *
 * If called with `UNLOCK_ON_CONNECTION_LOSS`, any lock that are returned will be unlocked
 * if the session loses its connection to the server. This is useful to
 * allow another session to take ownership of the lock while this session is
 * reconnecting.
 *
 * @param session   The session handle. If NULL, the function returns immediately.
 * @param lock_name The name of the session lock
 * @param scope     The preferred scope. The scope of a lock controls when it will
 *                  be released automatically. If a session makes multiple requests
 *                  for a lock using different scopes, and the server assigns the lock
 *                  to the session fulfilling the requests, the lock will be given the
 *                  weakest scope `UNLOCK_ON_CONNECTION_LOSS`
 * @param params    Parameters describing the session lock request and
 *                  callbacks handlers which may be invoked in response.
 */
void diffusion_session_lock_with_scope(SESSION_T *session, const char *lock_name, DIFFUSION_SESSION_LOCK_SCOPE scope, DIFFUSION_SESSION_LOCK_PARAMS_T params);

/**
 * @brief Release a session lock, if owned.
 *
 * @param session      The session handle. If NULL, the function returns immediately.
 * @param session_lock The session lock to unlock. If NULL, the function returns immediately.
 * @param params       Parameters describing the session lock unlock request and
 *                     callbacks handlers which may be invoked in response.
 */
void diffusion_session_lock_unlock(SESSION_T *session, const DIFFUSION_SESSION_LOCK_T *session_lock, DIFFUSION_SESSION_LOCK_UNLOCK_PARAMS_T params);

/**
 * @brief Duplicate an existing `DIFFUSION_SESSION_LOCK_T`. `diffusion_session_lock_free` should be called
 *        on the pointer when no longer needed.
 *
 * @param session_lock The session lock to be duplicated
 *
 * @return a duplicate of the `DIFFUSION_SESSION_LOCK_T` passed into the function
 */
DIFFUSION_SESSION_LOCK_T *diffusion_session_lock_dup(const DIFFUSION_SESSION_LOCK_T *session_lock);

/**
 * @brief Free a memory allocated `DIFFUSION_SESSION_LOCK_T`
 *
 * @param session_lock The session lock to be freed.
 */
void diffusion_session_lock_free(DIFFUSION_SESSION_LOCK_T *session_lock);

#endif
