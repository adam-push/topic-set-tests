#ifndef _diffusion_registration_h_
#define _diffusion_registration_h_ 1

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
 * @file registration.h
 *
 * Functions for `DIFFUSION_REGISTRATION_T` management.
 */

#include "session.h"

/**
 * @brief A reference to a registered handler. Such a handler reference is
 * provided whenever a handler with a server side presence is registered.
 */
typedef struct DIFFUSION_REGISTRATION_T DIFFUSION_REGISTRATION_T;

/**
 * @brief Request that the handler is unregistered from the server.
 *
 * After the handler is unregistered, the handler's `on_close` function
 * will be called.
 *
 * @param session      The session handle. If NULL, this function returns immediately.
 * @param registration The handler registration. If NULL, this function returns immediately.
 */
void diffusion_registration_close(SESSION_T *session, const DIFFUSION_REGISTRATION_T *registration);

/**
 * @brief Returna a copy of a `DIFFUSION_REGISTRATION_T`. `diffusion_registration_free` should be called
 * on the pointer when no longer needed.
 *
 * @param registration diffusion registration to be copied.
 *
 * @return a copy of a `DIFFUSION_REGISTRATION_T`. NULL, if `registration` is NULL.
 */
DIFFUSION_REGISTRATION_T *diffusion_registration_dup(const DIFFUSION_REGISTRATION_T *registration);

/**
 * @brief Free a memory allocated `DIFFUSION_REGISTRATION_T`
 *
 * @param registration the `DIFFUSION_REGISTRATION_T` to be freed.
 */
void diffusion_registration_free(DIFFUSION_REGISTRATION_T *registration);

#endif
