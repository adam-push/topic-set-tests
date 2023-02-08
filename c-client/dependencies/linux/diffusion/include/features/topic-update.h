#ifndef _diffusion_topic_update_h_
#define _diffusion_topic_update_h_ 1

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
 * @file topic-update.h
 *
 * Provides a client session with the ability to update topics.
 *
 * Topics can be set to new values using stateless `diffusion_topic_update_set`
 * operations or a `DIFFUSION_TOPIC_UPDATE_STREAM_T`. Both ensure that new values
 * are applied safely to appropriate topics.
 *
 * - Update streams
 *
 * An update stream is created for a specific topic. An update stream builder
 * can be obtained using the `diffusion_update_stream_builder_init` method.
 * The type of the topic must match the type of values passed to the update stream.
 * An update stream can be used to send any number of updates. It sends a
 * sequence of updates for a specific topic to the server. If supported by the
 * data type, updates will be sent to the server as a stream of binary deltas.
 *
 * Update streams have additional ways of failing compared to stateless set
 * operations but when used repeatedly have lower overheads. This is because
 * update streams maintain a small amount of state that reduces the overhead of
 * operations but can become invalid for example, if the topic is deleted, or
 * some other session updates the topic value.
 *
 * By default, update streams use a form of optimistic locking. An update
 * stream can update its topic incrementally as long as nothing else updates
 * the topic. If the topic is updated independently (for example, by another
 * session, or by the current session via set or a different update stream),
 * then the next update performed by the update stream will result in an
 * `DIFF_ERR_INVALID_UPDATE_STREAM`.
 *
 * Applications can chose to use collaborative locking to coordinate exclusive
 * access to a topic. To follow this pattern acquire a `DIFFUSION_SESSION_LOCK_T`,
 * and use it with a `diffusion_topic_update_constraint_locked` constraint.
 * The application is responsible for designing a locking scheme which
 * determines which lock is required to access a particular topic, and for
 * ensuring that all parts of the application that update the topic follow this
 * scheme. Lock constraints and an application locking scheme can also ensure a
 * sequence of set operations has exclusive access to the topic.
 *
 * - Removing values
 *
 * When a `TOPIC_TYPE_STRING`, `TOPIC_TYPE_INT64`, or
 * `TOPIC_TYPE_DOUBLE` topic is set to null, the topic will
 * be updated to have no value. If a previous value was present subscribers will
 * receive a notification that the new value is null. New subscribers
 * will not receive a value notification. Attempting to set any other type of
 * topic to `NULL` will cause the error callback to be called with
 * `DIFF_ERR_INVALID_DATA`
 *
 * - Adding topics
 *
 * When setting a value using either stateless operations or update streams it
 * is possible to add a topic if one is not present. This is done using the
 * `diffusion_topic_update_add_and_set` function or providing a topic specification
 * when creating the update stream. If a topic exists these methods will update the
 * existing topic.
 *
 * - Access control
 *
 * To update a topic a session needs `PATH_PERMISSION_UPDATE_TOPIC` permission for
 * the topic path. To create a topic a session needs `PATH_PERMISSION_MODIFY_TOPIC`
 * permission for the topic path. Requests that combine adding a topic and setting
 * the value such as `diffusion_topic_update_add_and_set`require both permissions.
 */

#include "session.h"
#include "datatypes.h"
#include "topic-specification.h"
#include "update-constraint.h"
#include "update-stream.h"
#include "update-stream-builder.h"

typedef struct DIFFUSION_JSON_PATCH_RESULT_T DIFFUSION_JSON_PATCH_RESULT_T;

/**
 * @brief Callback when a topic set is successful.
 *
 * @param context      User supplied context passed in from `DIFFUSION_TOPIC_UPDATE_SET_PARAMS_T`
 *
 * @return HANDLERS_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_update_set_success_cb)(void *context);

/**
 * @brief Callback when a topic add and set is successful.
 *
 * @param result       The topic creation result of the add and set operation. Will be either
 *                     `TOPIC_CREATED` showing that the topic was created and then updated. Or
 *                     `TOPIC_EXISTS` showing that the topic already exists and was subsequently
 *                     updated.
 * @param context      User supplied context passed in from `DIFFUSION_TOPIC_UPDATE_SET_PARAMS_T`
 *
 * @return HANDLERS_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_update_topic_creation_cb)(DIFFUSION_TOPIC_CREATION_RESULT_T result, void *context);

/**
 * @brief Callback with the result of the JSON patch operation.
 *
 * @param result       The JSON patch result.
 * @param context      User supplied context passed in from `DIFFUSION_APPLY_JSON_PATCH_PARAMS_T`
 *
 * @return HANDLERS_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_json_patch_result_cb)(const DIFFUSION_JSON_PATCH_RESULT_T *result, void *context);

/**
 * @brief Return the index of the first failure in the JSON patch.
 *
 * @param result The JSON patch result.
 *
 * @return the index of the first failure in the JSON patch. -1 if there were no failures or `result`
 *         is `NULL`.
 */
int diffusion_json_patch_result_get_first_failure(const DIFFUSION_JSON_PATCH_RESULT_T *result);

/**
 * @brief Structure describing a topic set request
 */
typedef struct diffusion_topic_update_params_s {
        /// Topic path to be updated
        const char *topic_path;
        /// The topic update datatype
        DIFFUSION_DATATYPE datatype;
        /// Update value. Can be `NULL` for the following
        /// datatypes: `DATATYPE_STRING`,`DATATYPE_INT64`
        /// or `DATATYPE_DOUBLE`
        BUF_T *update;
        /// Callback when the topic update is successful
        on_topic_update_set_success_cb on_topic_update;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User-supplied context returned to callbacks.
        void *context;
} DIFFUSION_TOPIC_UPDATE_SET_PARAMS_T;

/**
 * @brief Structure describing a topic add and set request
 */
typedef struct diffusion_topic_update_add_and_set_params_s {
        /// Topic path to be added
        const char *topic_path;
        /// The topic's specification
        TOPIC_SPECIFICATION_T *specification;
        /// The topic update datatype
        DIFFUSION_DATATYPE datatype;
        /// Update value. Can be `NULL` for the following
        /// datatypes: `DATATYPE_STRING`,`DATATYPE_INT64`
        /// or `DATATYPE_DOUBLE`
        BUF_T *update;
        /// Callback when the topic add and set is successful
        on_topic_update_topic_creation_cb on_topic_update_add_and_set;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User-supplied context returned to callbacks.
        void *context;
} DIFFUSION_TOPIC_UPDATE_ADD_AND_SET_PARAMS_T;

/**
 * @brief Structure describing a JSON patch application request.
 */
typedef struct diffusion_apply_json_patch_params_s {
        /// Topic path to be added
        const char *topic_path;
        /// The patch to apply
        const char *patch;
        /// Callback with the result of the JSON patch operation.
        on_json_patch_result_cb on_json_patch_result;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User-supplied context returned to callbacks.
        void *context;
} DIFFUSION_APPLY_JSON_PATCH_PARAMS_T;

/**
 * @ingroup PublicAPI_TopicUpdate
 *
 * @brief Sets the topic to a specified value.
 *
 * The `NULL` value can only be passed to the `params.update`
 * property when updating `DATATYPE_STRING`,`DATATYPE_INT64`
 * or `DATATYPE_DOUBLE` topics.
 *
 * When a topic of type `TOPIC_TYPE_STRING`, `TOPIC_TYPE_INT64` or
 * `TOPIC_TYPE_DOUBLE` is set to `null`, the topic will be
 * updated to have no value. If a previous value was present subscribers
 * will receive a notification that the new value is `null`.
 * New subscribers will not receive a value notification.
 *
 * @param session The session handle. If NULL, this function returns immediately.
 * @param params  Parameters describing a topic update set request and callbacks handlers
 *                which may be invoked in response.
 */
void diffusion_topic_update_set(SESSION_T *session, DIFFUSION_TOPIC_UPDATE_SET_PARAMS_T params);

/**
 * @ingroup PublicAPI_TopicUpdate
 *
 * @brief Sets the topic to a specified value.
 *
 * Takes a constraint that must be satisfied for the update to be
 * applied.
 *
 * In other respects it works in the same way as `diffusion_topic_update_set`
 *
 * @param session    The session handle. If NULL, this function returns immediately.
 * @param constraint The conditional constraint to be applied to topic update set operation
 * @param params     Parameters describing a topic update set request and callbacks handlers
 *                   which may be invoked in response.
 */
void diffusion_topic_update_set_with_constraint(SESSION_T *session, const DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *constraint, DIFFUSION_TOPIC_UPDATE_SET_PARAMS_T params);

/**
 * @ingroup PublicAPI_TopicUpdate
 *
 * @brief Sets the topic to a specified value.
 *
 * If a topic does not exist at `params.topic_path`, one will be created using
 * the `params.specification`. If a topic does exist, its specification must
 * match `params.specification`, otherwise the operation will fail with
 * an error dispatched to `params.on_error`, if non NULL.
 *
 * @param session The session handle. If NULL, this function returns immediately.
 * @param params  Parameters describing a topic add and set request and callbacks
 *                handlers which may be invoked in response.
 */
void diffusion_topic_update_add_and_set(SESSION_T *session, DIFFUSION_TOPIC_UPDATE_ADD_AND_SET_PARAMS_T params);

/**
 * @ingroup PublicAPI_TopicUpdate
 *
 * @brief Sets the topic to a specified value.
 *
 * If a topic does not exist at `params.topic_path`, one will be created using
 * the `params.specification`. If a topic does exist, its specification must
 * match `params.specification`, otherwise the operation will fail with
 * an error dispatched to `params.on_error`, if non NULL.
 *
 * Takes a constraint that must be satisfied for the topic to be created or
 * the update to be applied.
 *
 * In other respects it works in the same way as `diffusion_topic_update_set`
 *
 * @param session    The session handle. If NULL, this function returns immediately.
 * @param constraint The conditional constraint to be applied to topic add and set operation
 * @param params     Parameters describing a topic add and set request and callbacks
 *                   handlers which may be invoked in response.
 */
void diffusion_topic_update_add_and_set_with_constraint(SESSION_T *session, const DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *constraint, DIFFUSION_TOPIC_UPDATE_ADD_AND_SET_PARAMS_T params);

/**
 * @ingroup PublicAPI_TopicUpdate
 *
 * @brief Applies a JSON Patch to a JSON topic.
 *
 * The `patch` argument should be formatted according to the JSON
 * Patch standard (RFC 6902).
 *
 * Patches are a sequence of JSON Patch operations contained in an array.
 * They are applied as an atomic update to the previous value if the
 * resulting update is successfully calculated. The following patch will
 * check the value at a specific key and update if the expected value is
 * correct:
 *
 * [{"op":"test", "path":"/price", "value" : 22},
 * {"op":"add", "path":"/price", "value": 23}]
 *
 * The available operations are:
 *
 * <ul>
 * <li>
 * Add: { "op": "add", "path": "/a/b/c", "value": [ "foo", "bar" ] }
 * <li>
 * Remove: { "op": "remove", "path": "/a/b/c" }
 * <li>
 * Replace: { "op": "replace", "path": "/a/b/c", "value": 43 }
 * <li>
 * Move: { "op": "move", "from": "/a/b/c", "path": "/a/b/d" }
 * <li>
 * Copy: { "op": "copy", "from": "/a/b/c", "path": "/a/b/e" }
 * <li>
 * Test: { "op": "test", "path": "/a/b/c", "value": "foo" }
 * </ul>
 *
 * The test operation checks that the CBOR representation of the value of a
 * topic is identical to value provided in the patch after converting it to
 * CBOR. If the value is represented differently as CBOR, commonly due to
 * different key ordering, then the patch will return the index of the
 * failed operation . e.g the values `{"foo": "bar", "count": 43}` and
 * `{"count": 43, "foo": "bar"}` are unequal despite semantic equality
 * due to the differences in a byte for byte comparison.
 *
 * If the patch is not a valid JSON patch, `DIFF_ERR_INVALID_PATCH` will be
 * the error code called to error callback. If the patch contains invalid CBOR,
 * `DIFF_ERR_FAILED_PATCH` will be the error code instead.
 *
 * @param session   The session handle. If NULL, this function returns immediately.
 * @param params    Parameters describing a JSON patch application request and callbacks handlers
 *                  which may be invoked in response.
 * @param api_error Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_apply_json_patch(SESSION_T *session, DIFFUSION_APPLY_JSON_PATCH_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_TopicUpdate
 *
 * @brief Applies a JSON Patch to a JSON topic.
 *
 * Takes a constraint that must be satisfied for the update to be
 * applied.
 *
 * In other respects this method works in the same way as `diffusion_apply_json_patch`.
 *
 * @param session    The session handle. If NULL, this function returns immediately.
 * @param constraint The constraint that must be satisfied for the patch to be applied
 * @param params     Parameters describing a JSON patch application request and callbacks handlers
 *                   which may be invoked in response.
 * @param api_error  Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_apply_json_patch_with_constraint(SESSION_T *session, const DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *constraint, DIFFUSION_APPLY_JSON_PATCH_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_TopicUpdate
 *
 * @brief Creates an update strean to use for updating a specific topic.
 *
 * The type of the topic being updated must match the dataype from the
 * `params.datatype` parameter.
 *
 * Update streams send a sequence of updates for a specific topic. They
 * take advantage of the sending a sequence of updates to generate and send
 * binary deltas to the server. They do not provide exclusive access to the
 * topic. If exclusive access is required update streams should be used
 * with session locks as constraints.
 *
 * Streams are validated lazily when the first `diffusion_topic_update_stream_set` is
 * operation is completed. Once validated a stream can be invalidated, after which it
 * rejects future updates.
 *
 * `diffusion_topic_update_stream_free` should be called on this pointer when no longer needed.
 *
 * @param session    The session handle. If NULL, this function returns immediately.
 * @param topic_path The path of the topic
 * @param datatype   The topic's datatype
 *
 * @return a memory allocated diffusion topic update stream
 *
 * @deprecated since 6.9. Use DIFFUSION_UPDATE_STREAM_BUILDER_T instead.
 */
DECLARATION_DEPRECATED(DIFFUSION_TOPIC_UPDATE_STREAM_T *diffusion_topic_update_create_update_stream(SESSION_T *session, const char *topic_path, DIFFUSION_DATATYPE datatype))

/**
 * @ingroup PublicAPI_TopicUpdate
 *
 * @brief Creates an update stream to use for updating a specific topic.
 *
 * Takes a constraint that must be satisfied for the update stream to be
 * validated.
 *
 * In other respects it works in the same way as `diffusion_topic_update_create_update_stream`.
 *
 * `diffusion_topic_update_stream_free` should be called on this pointer when no longer needed.
 *
 * @param session    The session handle. If NULL, this function returns immediately.
 * @param constraint The constraint that must be satisfied for the update
 *                   stream to be validated
 * @param topic_path The path of the topic
 * @param datatype   The topic's datatype
 *
 * @return a memory allocated diffusion topic update stream
 *
 * @deprecated since 6.9. Use DIFFUSION_UPDATE_STREAM_BUILDER_T instead.
 */
DECLARATION_DEPRECATED(DIFFUSION_TOPIC_UPDATE_STREAM_T *diffusion_topic_update_create_update_stream_with_constraint(SESSION_T *session, const DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *constraint, const char *topic_path, DIFFUSION_DATATYPE datatype))

/**
 * @ingroup PublicAPI_TopicUpdate
 *
 * @brief Creates an update stream to use for creating and
 * updating a specific topic.
 *
 * If a topic does exist, its specification must match `specification`,
 * otherwise the operation will fail with `DIFF_ERR_INCOMPATIBLE_TOPIC`
 * being called to the error callback
 *
 * In other respects this method works in the same way as
 * `diffusion_topic_update_create_update_stream`.
 *
 * `diffusion_topic_update_stream_free` should be called on this pointer when no longer needed.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param topic_path    The topic path
 * @param specification The topic's specification
 * @param datatype      The topic's datatype
 * @param params        Parameters describing a request to create an update stream
 *
 * @return a memory allocated diffusion topic update stream
 *
 * @deprecated since 6.9. Use DIFFUSION_UPDATE_STREAM_BUILDER_T instead.
 */
DECLARATION_DEPRECATED(DIFFUSION_TOPIC_UPDATE_STREAM_T *diffusion_topic_update_create_update_stream_adding_topic(SESSION_T *session, const char *topic_path, const TOPIC_SPECIFICATION_T *specification,
                                                                                          DIFFUSION_DATATYPE datatype, DIFFUSION_TOPIC_UPDATE_STREAM_PARAMS_T params))

/**
 * @ingroup PublicAPI_TopicUpdate
 *
 * @brief Creates an update stream to use for creating and
 * updating a specific topic.
 *
 * If a topic does exist, its specification must match `specification`,
 * otherwise the operation will fail with `DIFF_ERR_INCOMPATIBLE_TOPIC`
 * being called to the error callback.
 *
 * Takes a constraint that must be satisfied for the update stream to be
 * validated.
 *
 * In other respects this method works in the same way as
 * `diffusion_topic_update_create_update_stream`.
 *
 * `diffusion_topic_update_stream_free` should be called on this pointer when no longer needed.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param constraint    The constraint that must be satisfied for the update
 *                      stream to be validated
 * @param topic_path    The topic path
 * @param specification The topic's specification
 * @param datatype      The topic's datatype
 * @param params        Parameters describing a request to create an update stream
 *
 * @return a memory allocated diffusion topic update stream
 *
 * @deprecated since 6.9. Use DIFFUSION_UPDATE_STREAM_BUILDER_T instead.
 */
DECLARATION_DEPRECATED(DIFFUSION_TOPIC_UPDATE_STREAM_T *diffusion_topic_update_create_update_stream_adding_topic_with_constraint(SESSION_T *session, const DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *constraint, const char *topic_path,
                                                                                                          const TOPIC_SPECIFICATION_T *specification, DIFFUSION_DATATYPE datatype, DIFFUSION_TOPIC_UPDATE_STREAM_PARAMS_T params))

#endif
