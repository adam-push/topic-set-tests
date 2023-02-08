#ifndef _diffusion_update_stream_builder_h_
#define _diffusion_update_stream_builder_h_ 1

/*
 * Copyright © 2022 Push Technology Ltd., All Rights Reserved.
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
 * @file update-stream-builder.h
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

/**
 * Opaque Diffusion Update Stream Builder.
 */
typedef struct DIFFUSION_UPDATE_STREAM_BUILDER_T DIFFUSION_UPDATE_STREAM_BUILDER_T;

/**
 * @brief Initialize a new update stream builder.
 *
 * <code>diffusion_update_stream_builder_free</code> should be called on this pointer
 * when no longer needed.
 *
 * @return the update stream builder.
 *
* @since 6.9
 */
DIFFUSION_UPDATE_STREAM_BUILDER_T *diffusion_update_stream_builder_init();

/**
 * @brief Resets the builder to its default parameters.
 *
 * @param builder   the update stream builder.
 * @param error     populated if an error occurrs. Can be NULL.
 *
 * @return          the update stream builder or NULL if an error occurred.
 *
 * @since 6.9
 */
DIFFUSION_UPDATE_STREAM_BUILDER_T *diffusion_update_stream_builder_reset(
        DIFFUSION_UPDATE_STREAM_BUILDER_T *builder,
        DIFFUSION_API_ERROR *error);


/**
 * @brief Specifies a topic specification for this update stream.
 *
 * If a topic does not exist at the <code>topic path</code>, one will be created using the
 * topic specification when the update stream is validated. If a topic does exist, its topic
 * specification must match, otherwise the operation will fail.
 *
 * @param builder           the update stream builder.
 * @param specification     the required specification of the topic.
 * @param error             populated if an error occurrs. Can be NULL.
 *
 * @return                  the update stream builder or NULL if an error occurred.
 *
 * @since 6.9
 */
DIFFUSION_UPDATE_STREAM_BUILDER_T *diffusion_update_stream_builder_topic_specification(
        DIFFUSION_UPDATE_STREAM_BUILDER_T *builder,
        const TOPIC_SPECIFICATION_T *specification,
        DIFFUSION_API_ERROR *error);


/**
 * @brief Specifies an update constraint for this update stream.
 *
 * Constraints can be applied to the setting of a value and creation of an update stream.
 * Constraints describe a condition that must be satisfied for the operation to succeed.
 * The constraints are evaluated on the server. The available constraints are:
 * - an active session lock
 * - the absence of a topic
 * - the current value of the topic being updated
 * - part of the current value of the topic being updated
 *
 * Constraint is Unconstrained by default. Calling this method with a NULL parameter resets
 * the constraint for this builder.
 *
 * @param builder       the update stream builder.
 * @param constraint    the constraint that must be satistied for the update stream to be validated.
 * @param error         populated if an error occurrs. Can be NULL.
 *
 * @return              the update stream builder or NULL if an error occurred.
 *
 * @since 6.9
 */
DIFFUSION_UPDATE_STREAM_BUILDER_T *diffusion_update_stream_builder_update_constraint(
        DIFFUSION_UPDATE_STREAM_BUILDER_T *builder,
        const DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *constraint,
        DIFFUSION_API_ERROR *error);


/**
 * @brief Free an update stream builder.
 *
 * @param builder       the update stream builder.
 *
 * @since 6.9
 */
void diffusion_update_stream_builder_free(
        DIFFUSION_UPDATE_STREAM_BUILDER_T *builder);


/**
 * @brief Creates an update stream to use for updating a specific topic.
 *
 * The type of the topic being updated must match the <code>datatype</code>.
 *
 * Update streams send a sequence of updates for a specific topic. The updates
 * may be delivered to the server as binary deltas. They do not provide exclusive
 * access to the topic. If exclusive access is required, update streams should be used
 * with session locks as constraints.
 *
 * Streams are validated lazily when the first <code>diffusion_topic_update_stream_set</code> or
 * <code>diffusion_topic_update_stream_validate</code> operation is completed. Once validated, a
 * stream can be invalidated, after which it rejects future updates.
 *
 * @param builder       the update stream builder.
 * @param topic_path    the path to the topic.
 * @param datatype      the topic's datatype.
 * @param error         populated if an error occurrs. Can be NULL.
 *
 * @return              the update stream or NULL if an error occurred.
 *
 * @since 6.9
 */
DIFFUSION_TOPIC_UPDATE_STREAM_T *diffusion_update_stream_builder_create_update_stream(
        DIFFUSION_UPDATE_STREAM_BUILDER_T *builder,
        const char *topic_path,
        DIFFUSION_DATATYPE datatype,
        DIFFUSION_API_ERROR *error);

#endif
