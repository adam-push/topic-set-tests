#ifndef _diffusion_update_constraint_h_
#define _diffusion_update_constraint_h_ 1

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
 * @file update-constraint.h
 *
 * A constraint to be applied to an update operation or the creation of an
 * update stream.
 *
 * Constraints describe a condition that must be satisfied for an operation to
 * succeed. Constraints can be applied to the setting of a value or creation
 * of an update stream. Constraints are only evaluated on the server.
 *
 * The constraints are evaluated using the:
 *
 * - active session locks
 * - existence of the topic
 * - current value of the topic
 *
 * The value of a topic can be described in several ways. The value can be
 * described as an exact value, a partial value or an unset value.
 *
 * Constraints can be composed with one another. It is only possible to
 * construct logical ANDs of constraints. Constraints can only be composed if
 * the resulting constraint is satisfiable. Multiple session locks can be held
 * but a topic can only have a single value. Constraints specifying multiple
 * topic values cannot be constructed.
 */

#include "session-lock.h"
#include "datatypes.h"

/**
 * Opaque topic update constraint struct
 */
typedef struct DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T;

/**
 * @brief Create a constraint requiring a lock to be held by the session.
 *
 * This can be used to co-ordination operations between multiple
 * sessions.
 *
 * `diffusion_topic_update_constraint_free` should be called on the pointer when no longer needed.
 *
 * @param lock the lock
 *
 * @return the constraint
 */
DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *diffusion_topic_update_constraint_locked(const DIFFUSION_SESSION_LOCK_T *lock);

/**
 * @brief Create a constraint requiring the current value of the topic to match
 * the supplied value.
 *
 * `NULL` value cannot be passed as a parameter because
 * it would prevent the required topic type being inferred.
 *
 * This can be used to change the value of a topic. This constraint is
 * unsatisfied if no topic is present at the path making it unsuitable
 * for operations that try to add topics.
 *
 * `diffusion_topic_update_constraint_free` should be called on the pointer when no longer needed.
 *
 * @param value the value
 *
 * @return the constraint
 */
DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *diffusion_topic_update_constraint_value(const BUF_T *value);

/**
 * @brief Create a constraint requiring the topic to have no value.
 *
 * This can be used set the first value of a topic. This constraint is
 * unsatisfied if no topic is present at the path making it unsuitable
 * for operations that try to add topics.

 * `diffusion_topic_update_constraint_free` should be called on the pointer when no longer needed.
 *
 * @return the constraint
 */
DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *diffusion_topic_update_constraint_no_value(void);

/**
 * @brief Create a constraint requiring the path to have no topic.
 *
 * This can be used set the first value of a topic being added using
 * `diffusion_topic_update_add_and_set` without changing the value
 * if the topic already exists. This constraint is unsatisfied if a
 * topic is present at the path making it unsuitable for operations
 * that try to set topics without adding them.
 *
 * `diffusion_topic_update_constraint_free` should be called on the pointer when no longer needed.
 *
 * @return the constraint
 */
DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *diffusion_topic_update_constraint_no_topic(void);

/**
 * @brief Require a value at a specific position in the JSON object.
 *
 * The `pointer` is a
 * <a href="https://tools.ietf.org/html/rfc6901">JSON Pointer</a>
 * syntax reference locating the `value` in the JSON object.
 *
 * Only string, int64, and double values are supported. The null value
 * may be passed for any type.
 *
 * `diffusion_topic_update_constraint_free` should be called on the pointer when no longer needed.
 *
 * @param pointer  the pointer expression
 * @param datatype the value datatype
 * @param value    the value
 *
 * @return the constraint. `NULL` if `datatype` isn't one of `DATATYPE_STRING`, `DATATYPE_INT64` or
 *         `DATATYPE_DOUBLE`. Also `NULL` if `pointer` is `NULL`
 */
DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *diffusion_topic_update_constraint_partial_json_with_value(const char *pointer, DIFFUSION_DATATYPE datatype, const BUF_T *value);

/**
 * @brief Require a specific position in the JSON object to be absent. This
 * does not match positions that have null values.
 *
 * The `pointer` is a
 * <a href="https://tools.ietf.org/html/rfc6901">JSON Pointer</a>
 * syntax reference that should have no value in the JSON object.
 *
 * `diffusion_topic_update_constraint_free` should be called on the pointer when no longer needed.
 *
 * @param pointer the pointer expression
 *
 * @return the constraint. `NULL` if `pointer` is `NULL`
 */
DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *diffusion_topic_update_constraint_partial_json_without_value(const char *pointer);

/**
 * @brief Create a composed constraint that represents a logical AND of two constraints. `diffusion_topic_update_constraint_free`
 * should be called on the pointer when no longer needed.
 *
 * @param constraint       a constraint that will be logically-ANDed with other_constraint
 * @param other_constraint a constraint that will be logically-ANDed with constraint
 *
 * @return a composed constraint that represents a logical AND of `constraint` and
 *         `other_constraint`
 */
DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *diffusion_topic_update_constraint_and(const DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *constraint, const DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *other_constraint);

/**
 * @brief Free a memory allocated `DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T`
 * 
 * @param constraint the constraint to be freed
 */
void diffusion_topic_update_constraint_free(DIFFUSION_TOPIC_UPDATE_CONSTRAINT_T *constraint);

#endif