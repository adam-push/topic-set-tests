#ifndef _diffusion_value_stream_h_
#define _diffusion_value_stream_h_ 1

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
 * @file value-stream.h
 *
 * Value stream related callback functions.
 */
#include <stdlib.h>

#include "features/topics.h"

#include "topic-specification.h"
#include "datatypes.h"

/**
 * @brief Callback for subscription.
 *
 * @param topic_path    the path of the topic subscribed to
 * @param specification the specification of the topic
 * @param context       user-supplied context from the initial `add_topic_from_specification` call.
 *
 * @return `HANDLER_SUCCESS` or `HANDLER_FAILURE`.
 */
typedef int (*value_stream_on_subscription)(const char *const topic_path,
                               const TOPIC_SPECIFICATION_T *specification,
                               void *context);

/**
 * @brief Callback for unsubscription.
 *
 * @param topic_path    the path of the topic subscribed to
 * @param specification the specification of the topic
 * @param reason        the unsubscription reason
 * @param context       user-supplied context from the initial `add_topic_from_specification` call.
 *
 * @return `HANDLER_SUCCESS` or `HANDLER_FAILURE`.
 */
typedef int (*value_stream_on_unsubscription)(const char *const topic_path,
                               const TOPIC_SPECIFICATION_T *const specification,
                               NOTIFY_UNSUBSCRIPTION_REASON_T reason,
                               void *context);

/**
 * @brief Callback for when a value has been received for the topic.
 *
 * @param topic_path    the path of the topic subscribed to
 * @param specification the specification of the topic
 * @param datatype      the value stream datatype
 * @param old_value     the previous value for topic. Will be null for the initial call to on_value
 *                      for a topic. It can also be null if the topic's data type supports null values.
 *                      This value is intended to be called with its corresponding datatype value reader.
 *                      i.e `read_diffusion_string_value`, `read_diffusion_binary_value`, `read_diffusion_int64_value`, etc.
 * @param new_value     the new value derived from the last update received from the server.
 *                      It can be null if the topic's data type supports null values.
 *                      This value is intended to be called with its corresponding datatype value reader.
 *                      i.e `read_diffusion_string_value`, `read_diffusion_binary_value`, `read_diffusion_int64_value`, etc.
 * @param context       User-supplied context from the initial `add_topic_from_specification` call.
 *
 * @return `HANDLER_SUCCESS` or `HANDLER_FAILURE`.
 */
typedef int (*value_stream_on_value)(const char *const topic_path,
                               const TOPIC_SPECIFICATION_T *const specification,
                               DIFFUSION_DATATYPE datatype,
                               const DIFFUSION_VALUE_T *const old_value,
                               const DIFFUSION_VALUE_T *const new_value,
                               void *context);

/**
 * @brief Callback for when a value stream has been closed.
 */
typedef void (*value_stream_on_close)(void);

/**
 * @brief Callback for when a value stream has encountered an error
 *
 * @param error containing the error code and description of the error.
 *              The context to this will be the value stream's context.
 */
typedef void (*value_stream_on_error)(const DIFFUSION_ERROR_T *error);

typedef struct value_stream_s {
        /**
         * value stream datatype
         */
        DIFFUSION_DATATYPE datatype;
        /**
         * on topic subscription callback function
         */
        value_stream_on_subscription on_subscription;
        /**
         * on topic unsubscription callback function
         */
        value_stream_on_unsubscription on_unsubscription;
        /**
         * on topic value callback function
         */
        value_stream_on_value on_value;
        /**
         * on value stream close callback function
         */
        value_stream_on_close on_close;
        /**
         * on value stream error callback function
         */
        value_stream_on_error on_error;
        /**
         * context
         */
        void *context;
} VALUE_STREAM_T;

#endif
