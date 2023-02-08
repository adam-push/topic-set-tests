/**
 * @file service_types.h
 *
 * This file lists the types of services supported by Diffusion (although
 * some of these may not yet be supported by the C API) and associated
 * structures.
 *
 * Copyright © 2014, 2022 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */

#ifndef _diffusion_service_types_h_
#define _diffusion_service_types_h_ 1

#include "buf.h"
#include "types/conversation_types.h"
#include "types/common_types.h"

/**
 * These are the services (and commands) that can be sent and received between
 * Diffusion and a client.
 * They are equivalent to those listed in the Java API in the StandardServices
 * class.
 */
typedef enum {
        // Client services
        SVC_PING = 1,
        SVC_SUBSCRIBE = 3,
        SVC_UNSUBSCRIBE = 4,
        SVC_CHANGE_PRINCIPAL = 5,

        // Control services
        SVC_SUBSCRIBE_CONTROL = 10,
        SVC_UNSUBSCRIBE_CONTROL = 11,

        SVC_CLOSE_CLIENT = 14,
        SVC_TOPIC_CONTROL_REGISTRATION = 20,
        SVC_TOPIC_CONTROL_DEREGISTRATION = 21,
        SVC_AUTHENTICATION = 24,
        SVC_SESSION_CLOSE_VIEW_UPDATE = 27,
        SVC_SEND_RECEIVER = 28,

        // Topic source services (deprecated names))
        SVC_TOPIC_SOURCE_REGISTRATION = 30,
        SVC_TOPIC_SOURCE_DEREGISTRATION = 31,
        SVC_TOPIC_SOURCE_STATE = 32,
        SVC_TOPIC_SOURCE_UPDATE = 34,

        SVC_NOTIFY_SUBSCRIPTION = 40,
        SVC_NOTIFY_UNSUBSCRIPTION = 42,

        // Other topic services
        SVC_ADD_TOPIC = 46,
        SVC_REMOVE_TOPICS = 47,

        SVC_MISSING_TOPIC = 50,
        SVC_ROUTING_SUBSCRIBE_CONTROL = 51,
        SVC_ROUTING_SUBSCRIBE_CALLBACK = 52,

        SVC_PING_SYSTEM = 55,
        SVC_PING_USER = 56,

        SVC_GET_SYSTEM_AUTHENTICATION = 57,
        SVC_UPDATE_SYSTEM_AUTHENTICATION = 58,
        SVC_GET_SECURITY = 59,
        SVC_UPDATE_SECURITY = 60,

        SVC_FILTER_SUBSCRIBE = 65,
        SVC_FILTER_UNSUBSCRIBE = 66,

        SVC_GET_SESSION_PROPERTIES = 67,
        SVC_SESSION_PROPERTIES_REGISTRATION = 69,
        SVC_SESSION_PROPERTIES_EVENT = 70,

        SVC_SESSION_PROPERTIES_REGISTRATION_2 = 81,
        SVC_SESSION_PROPERTIES_EVENT_2 = 82,

        SVC_TOPIC_REMOVAL = 83,

        SVC_RANGE_QUERY = 84,

        SVC_NOTIFY_SUBSCRIPTION_TOPIC_SPECIFICATION = 87,
        SVC_TOPIC_ADD = 112,

        SVC_MESSAGING_SEND = 85,
        SVC_MESSAGING_RECEIVER_SERVER = 86,
        SVC_MESSAGING_RECEIVER_CLIENT = 88,

        SVC_TOPIC_NOTIFICATION_DEREGISTRATION = 91,
        SVC_TOPIC_NOTIFICATION_EVENTS = 92,
        SVC_TOPIC_DESCENDANT_EVENTS = 93,
        SVC_TOPIC_NOTIFICATION_SELECTION = 94,
        SVC_TOPIC_NOTIFICATION_DESELECTION = 95,

        SVC_MESSAGING_RECEIVER_CONTROL_REGISTRATION = 97,
        SVC_MESSAGING_RECEIVER_CONTROL_DEREGISTRATION = 98,

        SVC_TIME_SERIES_APPEND = 99,
        SVC_TIME_SERIES_EDIT = 100,

        SVC_MESSAGING_FILTER_SENDER = 102,
        SVC_FILTER_RESPONSE = 103,

        SVC_SET_SESSION_PROPERTIES = 105,
        SVC_SET_SESSION_PROPERTIES_FILTER = 106,

        SVC_ACQUIRE_SESSION_LOCK = 114,
        SVC_RELEASE_SESSION_LOCK = 116,

        SVC_FETCH_QUERY = 117,
        SVC_SET_TOPIC = 118,
        SVC_ADD_AND_SET_TOPIC = 122,

        SVC_AUTHENTICATOR_REGISTRATION = 119,
        SVC_AUTHENTICATOR = 120,
        SVC_AUTHENTICATOR_DEREGISTRATION = 121,

        SVC_CHANGE_AUTHORISATION_ROLES = 123,
        SVC_CHANGE_AUTHORISATION_ROLES_FILTER = 124,

        SVC_CREATE_UPDATE_STREAM = 125,
        SVC_CREATE_UPDATE_STREAM_AND_SET = 126,
        SVC_CHECK_UPDATE_STREAM = 127,
        SVC_STREAM_SET_TOPIC = 128,
        SVC_STREAM_APPLY_DELTA = 129,
        SVC_STREAM_ADD_TOPIC = 130,
        SVC_STREAM_ADD_AND_SET_TOPIC = 131,

        SVC_PUT_SESSION_METRIC_COLLECTOR = 135,
        SVC_REMOVE_SESSION_METRIC_COLLECTOR = 136,
        SVC_LIST_SESSION_METRIC_COLLECTORS = 137,

        SVC_CREATE_TOPIC_VIEW = 138,
        SVC_REMOVE_TOPIC_VIEW = 139,
        SVC_LIST_TOPIC_VIEWS = 141,

        SVC_LIST_GLOBAL_PERMISSIONS = 143,
        SVC_LIST_PATH_PERMISSIONS = 144,

        SVC_PUT_TOPIC_METRIC_COLLECTOR = 146,
        SVC_REMOVE_TOPIC_METRIC_COLLECTOR = 147,
        SVC_LIST_TOPIC_METRIC_COLLECTORS = 148,

        SVC_APPLY_JSON_PATCH = 151,

        SVC_CREATE_REMOTE_SERVER = 157,
        SVC_REMOVE_REMOTE_SERVER = 158,

        SVC_CLOSE_CLIENT_FILTER = 162,
        SVC_SET_CLIENT_CONFLATION_FILTER = 163,
        SVC_LIST_REMOTE_SERVERS = 164,
        SVC_CHECK_REMOTE_SERVER = 165,
        SVC_TIME_SERIES_TIMESTAMP_APPEND = 166,

        SVC_PUT_BRANCH_MAPPING_TABLE = 176,
        SVC_GET_SESSION_TREE_BRANCHES = 177,
        SVC_GET_BRANCH_MAPPING_TABLE = 178,

        SVC_GET_TOPIC_VIEW = 182

} SERVICE_TYPE_T;

/** The maximum value of the service identifier. */
#define SERVICE_TYPE_MAX_VALUE 0x100

/**
 * Interactions with Diffusion take the form of a REQUEST or a RESPONSE.
 */
typedef enum {
        MODE_ERROR = 0,
        MODE_REQUEST = 1,
        MODE_RESPONSE = 2
} SERVICE_MODE_T;

/**
 * Service messages differ in their structure depending on the requirements
 * of the service, but they all begin with these common fields.
 */
#define SVC_MSG_COMMON_FIELDS                   \
        SERVICE_TYPE_T service_type;            \
        SERVICE_MODE_T service_mode;            \
        CONVERSATION_ID_T conversation_id;

/** Base type for service messages */
typedef struct svc_msg_common_s {
        SVC_MSG_COMMON_FIELDS
} SVC_MSG_COMMON_T;

/**
 * Every service that a client can provide has to implement this core set of
 * functions.
 */
typedef struct svc_defn_s {
        /// Service request handler.
        void (*request_handler)();
        /// Service response handler.
        void (*response_handler)();
        /// Service error handler.
        int (*error_handler)();
        /// Deserialise a request to the appropriate structure.
        void *(*request_unmarshal)();
        /// Serialise a request structure to bytes.
        BUF_T *(*request_marshal)();
        /// Deserialise a response to the appropriate structure.
        void *(*response_unmarshal)();
        /// Serialise a response structure to bytes.
        BUF_T *(*response_marshal)();
        /// Create a default request for this service.
        void *(*request_create)();
        /// Create a default response for this service.
        void *(*response_create)();
        /// Free memory associated with the request structure.
        void (*request_free)();
        /// Free memory associated with the response structure.
        void (*response_free)();
        /// Local data for service-specific use.
        void *local;
} SVC_DEFN_T;

#endif
