#ifndef _diffusion_update_stream_h_
#define _diffusion_update_stream_h_ 1

/*
 * Copyright © 2018, 2021 Push Technology Ltd., All Rights Reserved.
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
 * @file update-stream.h
 *
 * An update stream provides a method for updating a specific topic.
 *
 * An update stream is associated with a specific topic. The type of the
 * topic must match the type of values passed to the update stream. It can
 * be created with an optional `DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T`. The
 * existence of the topic, its type and the constraint are validated lazily
 * by the first `diffusion_topic_update_stream_set` or
 * `diffusion_topic_update_stream_validate` operation. Subsequent
 * operations issued before the first operation completes will be deferred
 * until the completion of the first operation.
 *
 * An update stream can be used to send any number of updates. It sends a
 * sequence of updates for a specific topic to the server. If supported by the
 * data type, updates will be sent to the server as a stream of binary deltas.
 * An update stream does not prevent other sessions from updating the topic. If
 * exclusive access is required update streams should be used with session locks
 * as constraints.
 *
 * Once validated an update stream can be invalidated. An invalidated
 * update stream rejects the operations applied to it. The update stream
 * will be invalidated if:
 *
 * - the topic is removed
 * - another update stream is created for the same topic
 * - the topic is updated to a new value by anything other than the stream
 * - the session does not have the `TOPIC_PERMISSION_UPDATE_TOPIC`
 * - an operation fails because of cluster repartitioning
 *
 * Update streams are thread-safe.
 *
 * Update streams are created using the topic-update feature.
 */

#include "session.h"
#include "buf.h"
#include "datatypes/diffusion-value.h"

typedef enum {
        /**
         * A new topic was created.
         */
        TOPIC_CREATED,
        /**
         * A topic with the same specification already exists.
         */
        TOPIC_EXISTS
} DIFFUSION_TOPIC_CREATION_RESULT_T;

/**
 * Opaque diffusion topic update stream struct
 */
typedef struct DIFFUSION_TOPIC_UPDATE_STREAM_T DIFFUSION_TOPIC_UPDATE_STREAM_T;

/**
 * @brief Callback from an operation with a topic update stream.
 *
 * @param result  The topic creation result
 * @param context User-supplied context
 *
 * @return HANDLERS_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_update_stream_result_cb)(DIFFUSION_TOPIC_CREATION_RESULT_T result, void *context);

/**
 * @brief Structure provided when calling `diffusion_topic_update_stream_set`
 */
typedef struct diffusion_topic_update_stream_params_s {
        /// Callback
        on_topic_update_stream_result_cb on_topic_creation_result;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User-supplied context returned to callbacks.
        void *context;
} DIFFUSION_TOPIC_UPDATE_STREAM_PARAMS_T;

/**
 * @brief Sets the topic to a specified value.
 *
 * The `NULL` value can only be passed to the `value` parameter
 * when updating `TOPIC_TYPE_STRING`, `TOPIC_TYPE_INT64` or
 * `TOPIC_TYPE_DOUBLE` topics.
 *
 * When a `TOPIC_TYPE_STRING`, `TOPIC_TYPE_INT64` or `TOPIC_TYPE_DOUBLE`
 * topic is set to `NULL`, the topic will be updated
 * to have no value. If a previous value was present subscribers
 * will receive a notification that the new value is `NULL`.
 * New subscribers will not receive a value notification.
 *
 * The first call to this function may fail with
 * `DIFF_ERR_NO_SUCH_TOPIC` or `DIFF_ERR_INCOMPATIBLE_TOPIC`.
 * Subsequent calls may fail with `DIFF_ERR_INVALID_UPDATE_STREAM`.
 * Any call can fail with `DIFF_ERR_CLUSTER_ROUTING`,
 * `DIFF_ERR_CLUSTER_REPARTITION`, `DIFF_ERR_ACCESS_DENIED` or
 * `DIFF_ERR_NO_SESSION` being called to the error callback.
 *
 * If an update constraint was provided when creating the
 * update stream, the first call to this function may also fail with
 * `DIFF_ERR_UNSATISFIED_CONSTRAINT`.
 *
 * If the update stream was created with `diffusion_topic_update_create_update_stream_adding_topic`,
 * the first call to this function may also fail with `DIFF_ERR_INCOMPATIBLE_EXISTING_TOPIC`
 * and it will not fail with `DIFF_ERR_NO_SUCH_TOPIC` being called to the error callback.
 *
 * If this function fails all subsequent calls to `diffusion_topic_update_stream_set`
 * will fail with `DIFF_ERR_INVALID_UPDATE_STREAM` being called to the error callback.
 *
 * @param session The session handle. If NULL, this function returns immediately.
 * @param stream  The update stream to enact the update.
 * @param value   The update value.
 * @param params  Parameters describing a topic update stream set request and callbacks handlers
 *                which may be invoked in response.
 */
void diffusion_topic_update_stream_set(SESSION_T *session, const DIFFUSION_TOPIC_UPDATE_STREAM_T *stream, const BUF_T *value, DIFFUSION_TOPIC_UPDATE_STREAM_PARAMS_T params);

/**
 * @brief Return the latest value of the topic set using this update stream.
 *
 * The returned value reflects the last value that has been set, before it
 * is sent to the server.
 *
 * If the server rejects a set operation, the topic value will not change
 * and this update stream will be invalidated.
 *
 * `diffusion_value_free` should be called on the pointer when no longer needed.
 *
 * @param stream The stream to get the latest value from
 *
 * @return the cached value of the topic. NULL, if the update stream is in an invalid state
 */
DIFFUSION_VALUE_T *diffusion_topic_update_stream_get(const DIFFUSION_TOPIC_UPDATE_STREAM_T *stream);

/**
 * @brief Validates the update stream.
 *
 * Update streams are validated lazily when `diffusion_topic_update_stream_set`
 * This function allows the stream to be validated before a value needs to be set.
 *
 * If the update stream has not been validated yet, calling this function
 * checks the topic exists, the topic type is correct, the constraint is
 * satisfied and the session has permission to update the topic. Once
 * it has been validated calling this function checks the topic has not been
 * removed, no other stream has been created for the topic, the value
 * of the topic has not been changed by anything else and the session
 * still has permission to update the topic.
 *
 * This function may fail with `DIFF_ERR_INCOMPATIBLE_EXISTING_TOPIC`,
 * if it is the first call to `diffusion_topic_update_stream_validate`,
 * `diffusion_topic_update_stream_set` has not been called and a topic specification
 * was provided when creating the update stream, otherwise it will never
 * fail with this cause.
 *
 * The first call to this function may fail with
 * `DIFF_ERR_NO_SUCH_TOPIC` or `DIFF_ERR_INCOMPATIBLE_TOPIC`.
 * Subsequent calls may fail with a `DIFF_ERR_INVALID_UPDATE_STREAM`.
 * Any call can fail with `DIFF_ERR_CLUSTER_ROUTING`,
 * `DIFF_ERR_CLUSTER_REPARTITION`, `DIFF_ERR_ACCESS_DENIED` or
 * `DIFF_ERR_NO_SESSION` being called to the error callback.
 *
 * If an update constraint was provided when creating the
 * update stream, the first call to this function may also fail with
 * `DIFF_ERR_UNSATISFIED_CONSTRAINT` being called to the error callback.
 *
 * If the update stream was created with `diffusion_topic_update_create_update_stream_adding_topic`,
 * the first call to this function may also fail with `DIFF_ERR_INCOMPATIBLE_EXISTING_TOPIC` and it
 * will not fail with `DIFF_ERR_NO_SUCH_TOPIC`.
 *
 * If this function fails all subsequent calls to `diffusion_topic_update_stream_set` or
 * `diffusion_topic_update_stream_validate` will fail with `DIFF_ERR_INVALID_UPDATE_STREAM`.
 *
 * @param session The session handle. If NULL, this function returns immediately.
 * @param stream  The update stream to be validated.
 * @param params  Parameters describing a topic update stream validate request and callbacks handlers
 *                which may be invoked in response.
 */
void diffusion_topic_update_stream_validate(SESSION_T *session, const DIFFUSION_TOPIC_UPDATE_STREAM_T *stream, DIFFUSION_TOPIC_UPDATE_STREAM_PARAMS_T params);

/**
 * @brief Free a memory allocated `DIFFUSION_TOPIC_UPDATE_STREAM_T`
 *
 * @param stream The topic update stream to be freed
 */
void diffusion_topic_update_stream_free(DIFFUSION_TOPIC_UPDATE_STREAM_T *stream);

#endif
