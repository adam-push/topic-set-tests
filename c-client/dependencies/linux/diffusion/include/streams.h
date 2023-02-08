#ifndef _diffusion_streams_h_
#define _diffusion_streams_h_ 1

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
 * @file streams.h
 *
 * Functions for adding value streams to a session.
 */
#include <stdlib.h>

#include "value-stream.h"
#include "types/session_types.h"

/**
 * @brief Token returned when a value stream is added. This token
 * is then required when removing the value stream.
 */
typedef struct VALUE_STREAM_HANDLE_T VALUE_STREAM_HANDLE_T;

/**
 * @brief Add a value stream.
 *
 * @param session        the current active session
 * @param topic_selector topic selector to register the stream for
 * @param value_stream   value stream to add
 *
 * @return a unique value stream handle required when the value stream is
 *         removed. If this function is unable to add the value stream, NULL
 *         will be returned.
 */
VALUE_STREAM_HANDLE_T *add_stream(SESSION_T *session, const char *topic_selector, const VALUE_STREAM_T *value_stream);

/**
 * @brief Add a time series stream.
 *
 * @param session        the current active session
 * @param topic_selector topic selector to register the stream for
 * @param value_stream   time series stream to add
 *
 * @return a unique value stream handle required when the value stream is
 *         removed. If this function is unable to add the value stream, NULL
 *         will be returned.
 */
VALUE_STREAM_HANDLE_T *add_time_series_stream(SESSION_T *session, const char *topic_selector, const VALUE_STREAM_T *value_stream);

/**
 * @brief Add a fallback value stream.
 *
 * @param session      The current active session
 * @param value_stream value stream to add
 *
 * @return a unique value stream handle required when the value stream is
 *         removed. If this function is unable to add the fallback value stream
 *         NULL will be returned.
 */
VALUE_STREAM_HANDLE_T *add_fallback_stream(SESSION_T *session, const VALUE_STREAM_T *value_stream);


/**
 * @brief Add a time series fallback value stream.
 *
 * @param session      The current active session
 * @param value_stream time series value stream to add
 *
 * @return a unique value stream handle required when the value stream is
 *         removed. If this function is unable to add the fallback value stream
 *         NULL will be returned.
 */
VALUE_STREAM_HANDLE_T *add_time_series_fallback_stream(SESSION_T *session, const VALUE_STREAM_T *value_stream);

/**
 * @brief Remove a value stream. The handle provided to this function
 * will be freed when the function completes.
 *
 * @param session The current active session
 * @param handle  value stream handle returned from an `add_stream` call
 */
void remove_stream(SESSION_T *session, const VALUE_STREAM_HANDLE_T *handle);

#endif
