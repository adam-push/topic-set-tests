#ifndef _diffusion_time_series_h_
#define _diffusion_time_series_h_ 1

/*
 * Copyright © 2019 - 2022 Push Technology Ltd., All Rights Reserved.
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
 * @file time-series.h
 *
 * Allows update and query of time series topics.
 *
 * <h2>Time series topics</h2>
 *
 * A time series is a sequence of events. Each event contains a value
 * and has server-assigned metadata comprised of a sequence number, timestamp,
 * and author. Events in a time series are ordered by increasing sequence
 * number. Sequence numbers have values between `0` and
 * `LONG_MAX` and are contiguous: an event with sequence number
 * `n` will be followed by one with sequence number `n + 1`. Two
 * events with the same sequence number will be equal; having the same
 * timestamp, author, and value.
 *
 * A time series topic allows sessions to access a time series that is
 * maintained by the server. A time series topic has an associated
 * event data type, such as `Binary`, `String`, or `JSON`, that determines
 * the type of value associated with each event.
 *
 * This feature provides a historic query API for time series topics, allowing a
 * session to query arbitrary sub-sequences of a time series. The
 * topic-control and topics features complete the API, providing
 * ways to create and subscribe to a time series topic.
 *
 * The API presents a time series as an append-only data structure of immutable
 * events that is only changed by adding new events.
 *
 * <h2>Edit events</h2>
 *
 * Although a time series is append-only, an event can be overridden by
 * appending an edit event. An edit event is a special type of event
 * that overrides an earlier event in the time series (referred to as the
 * original event) with a new value. When an edit event is added to a
 * time series, the server retains both the original event and the edit event,
 * allowing subscription and query results to reflect the edit.
 *
 * For example, suppose a time series has two events with the values `A`
 * and `B`, and the first event has been overridden by a later edit event
 * that provides a new value of `X`. The server has the following
 * information about the time series.
 *
 * <table>
 * <tr>
 * <th>Sequence</th>
 * <th>Value</th>
 * <th>Type</th>
 * </tr>
 * <tr>
 * <td>0</td>
 * <td>A</td>
 * <td><em>original event</em></td>
 * </tr>
 * <tr>
 * <td>1</td>
 * <td>B</td>
 * <td><em>original event</em></td>
 * </tr>
 * <tr>
 * <td>2</td>
 * <td>X</td>
 * <td><em>edit of sequence 0</em></td>
 * </tr>
 * </table>
 *
 * The current value of the event with sequence number 0 is `X`.
 *
 * If an original event has several edit events, the latest edit event (the one
 * with the highest sequence number) determines its current value. Each edit
 * event refers to an original event, never to another edit event.
 *
 * Extending the example by appending a further edit event to the time series:
 *
 * <table>
 * <tr>
 * <th>Sequence</th>
 * <th>Value</th>
 * <th>Type</th>
 * </tr>
 * <tr>
 * <td>3</td>
 * <td>Y</td>
 * <td><em>second edit of sequence 0</em></td>
 * </tr>
 * </table>
 *
 * The current value of the event with sequence number 0 is now `Y`.
 *
 * <h2>Retained range</h2>
 *
 * A time series topic retains a range of the most recent events. When a new
 * event is added to the time series, older events that fall outside of the
 * range are discarded. By default, this range includes the ten most recent
 * events. A different range can be configured by setting the
 * `DIFFUSION_TIME_SERIES_RETAINED_RANGE` property.
 *
 * <h2>Subscribing to a time series topic</h2>
 *
 * A session can subscribe to a time series topic and add a value stream to
 * receive updates about events appended to the time series. Events are
 * represented by `DIFFUSION_TIME_SERIES_EVENT_T` instances. Each event has a
 * value and `DIFFUSION_TIME_SERIES_EVENT_METADATA_T`. An edit event has two sets of
 * metadata; its own metadata and that of the original event that it
 * replaces.
 *
 * <h2>Subscription range</h2>
 *
 * New subscribers are sent a range of events from the end of the time series.
 * This is known as the subscription range. Configuring a subscription
 * range is a convenient way to provide new subscribers with an appropriate
 * subset of the latest events.
 *
 * The default subscription range depends on whether the topic is configured to
 * publish delta streams. If delta streams are enabled, new subscribers are sent
 * the latest event if one exists. If delta streams are disabled, new
 * subscribers are sent no events. Delta streams are enabled by default and can
 * be disabled by setting the `DIFFUSION_PUBLISH_VALUES_ONLY` property to "true".
 *
 * A larger subscription range can be configured by setting the
 * `DIFFUSION_TIME_SERIES_SUBSCRIPTION_RANGE` property. Regardless of the
 * `DIFFUSION_TIME_SERIES_SUBSCRIPTION_RANGE` property, if delta streams are
 * enabled, new subscribers will be sent at least the latest event if one
 * exists.
 *
 * If the range of events is insufficient, the subscribing session can use a
 * range query to retrieve older events.
 *
 * When configuring a non-default subscription range for a time series topic,
 * register value streams before subscribing to the topic. The session only
 * maintains a local cache of the latest value received for a topic, not the
 * full subscription range. If a value stream is added after a session has
 * subscribed to a matching time series topic, the new stream will only be
 * notified of the latest value.
 *
 * <h2>Updating a time series topic</h2>
 *
 * A session can use `diffusion_time_series_append` to submit a value to be added to a
 * time series. The server will add an event to the end of the time series based
 * on the supplied value, with a new sequence number, timestamp, and the author
 * set to the authenticated principal of the session.
 *
 * A session can use `diffusion_time_series_timestamp_append` to submit a value and
 * supplied timestamp. This provides control over the timestamp of the event. The
 * supplied timestamp must not be before the latest event stored by the time series
 * topic. There are no other restrictions.
 *
 * A session can use `diffusion_time_series_edit` to submit an edit to an original time
 * series event, identified by its sequence number. The server will add an edit
 * event to the end of the time series based on the supplied value, with a new
 * sequence number, timestamp, and the author set to the authenticated principal
 * of the session.
 *
 * Time series topics can also be updated using the functionality provided by the
 * {@link topic-update.h Topic Update} feature. This includes`diffusion_topic_update_set`,
 * `diffusion_topic_update_add_and_set` and {@link update-stream.h update streams}.
 * This usage performs an append operation with the added benefits of
 * {@link update-constraint.h update constraints}, topic creation when updating
 * (upsert), and delta streams. When using methods from {@link topic-update.h Topic Update},
 * the sequence number, timestamp, and author metadata will be generated using the same
 * rules as `diffusion_time_series_append` but the associated
 * `DIFFUSION_TIME_SERIES_EVENT_METADATA_T` will not be returned to the caller.
 *
 * <h2>Querying a time series topic</h2>
 *
 * A range quert is a configured query that can be evaluated for a time series
 * topic using `diffusion_time_series_select_from`. Results are
 * provided as streams of `DIFFUSION_TIME_SERIES_EVENT_T` instances.
 *
 * `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` is a builder for configuring a Query that
 * selects a range of a time series. There are two types of range query that differ
 * in how edits are processed; value range queries and edit range queries.
 *
 * <h2>Value range queries</h2>
 *
 * A value range query returns a merged view of part of a time series. This is
 * the most common time series query and appropriate for most applications.
 *
 * The result of a value range query reflects the latest available edits and the
 * query result list is ordered by the original event sequence number, presenting
 * edit events instead of the original events they replace. Original events that
 * have no edit events are included verbatim. Original events that have edit events
 * are replaced by the latest edit event.
 *
 * A value range query of the example time series, with no range constraints so
 * the entire time series is selected, returns two events:
 *
 * <pre>
 * sequence=3, value=Y; original event sequence=0
 * sequence=1, value=B
 * </pre>
 *
 * The original value of the first event is not provided. It's apparent that the
 * first event is an edit event because it provides the metadata of the original
 * event it replaces.
 *
 * <h2>Edit range queries</h2>
 *
 * Applications with auditing and other administrative requirements can access
 * original event values using an edit range query. An edit range query returns
 * an unmerged view of a time series that can include both original events and
 * the edit events that replace them. Edit range queries are rarely needed
 * &ndash; value range queries satisfy most use cases.
 *
 * Edit range queries provide a detailed view of a time series. Because this is
 * potentially sensitive information, an edit range query can only be performed
 * by a session that has the `QUERY_OBSOLETE_TIME_SERIES_EVENTS` permission for
 * the target topic.
 *
 * There are two sub-types of edit range query.
 *
 * A full audit trail of edit events can be obtained using an all edits
 * edit range query. The result contains all original events selected by the
 * query, together with all subsequent edit events that affect the original
 * events. The query result stream provides events in time series order. An all
 * edits query of the example time series, with no range constraints so the
 * entire time series is selected, returns four events:
 *
 * <pre>
 * sequence=0; value=A
 * sequence=1; value=B
 * sequence=2; value=X; original event sequence=0
 * sequence=3; value=Y; original event sequence=0
 * </pre>
 *
 * <p>
 * A latest edits edit range query returns a query result stream in
 * time series order that contains all original events selected by the query,
 * together with the latest edit events that affect the original events. A
 * latest edits query of the example time series, with no range constraints so
 * the entire time series is selected, returns three events:
 *
 * <pre>
 * sequence=0; value=A
 * sequence=1; value=B
 * sequence=3; value=Y; original event sequence=0
 * </pre>
 *
 * The initial range of events delivered for a subscription to a time series
 * topic is derived from a latest edits edit range query. See
 * Subscription Range.
 *
 * When evaluated for a time series that has no edit events, an edit range query
 * will return the same results as a similarly configured value range query.
 *
 * <h2>Changes to a time series made outside the API</h2>
 *
 * The API presents a time series as an append-only data structure of immutable
 * events that is only changed by adding new events. The API does not allow
 * events to be deleted or edited.
 *
 * There are circumstances in which events can be removed from a time series by
 * server operations outside the API. For example, a time series topic can be
 * configured to discard or archive older events to save storage space; or the
 * time series may be held in memory and lost if the server restarts. Subscribed
 * sessions are not notified when events are removed in this way, but a session
 * can infer the removal of events that are no longer included in query results.
 * Similarly, an event's value can be changed on the server. For example, if an
 * administrator changes its value to redact sensitive data. Again, subscribed
 * sessions are not notified when events are modified, but a session can infer
 * this has happened from query results.
 *
 * Whether such changes can happen for a particular time series topic depends on
 * the topic specification, and the administrative actions that are allowed. To
 * write a robust application, do not rely on two Event instances with the same
 * sequence number but obtained though different API calls, being equal; nor
 * that there are no sequence number gaps between events in query results.
 *
 * <h2>Access control</h2>
 *
 * The session must have the `READ_TOPIC` path permission for a topic to query a
 * time series topic. The `QUERY_OBSOLETE_TIME_SERIES_EVENTS` path permission is
 * additionally required to evaluate an `diffusion_time_series_range_query_for_edits`
 * query, or a `diffusion_time_series_range_query_for_values` with a
 * `diffusion_time_series_range_query_edit_range`.
 *
 * The session must have the `UPDATE_TOPIC` path permission for a
 * topic to append a new event to a time series topic. The `EDIT_TIME_SERIES_EVENTS`
 * path permission is additionally required to submit an edit to any time series
 * topic event. The more restrictive `EDIT_OWN_TIME_SERIES_EVENTS`
 * path permission allows a session to submit edits to time series topic
 * events that are authored by the principal of the calling session.
 */

#include "session.h"
#include "datatypes.h"
#include "range-query.h"
#include "diffusion-api-error.h"

/**
 * Opaque diffusion time series event metadata struct
 */
typedef struct DIFFUSION_TIME_SERIES_EVENT_METADATA_T DIFFUSION_TIME_SERIES_EVENT_METADATA_T;

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Return the sequence number identifying this event within its time series.
 * Assigned by the server when the event is created.
 *
 * Sequence numbers are unique within a time series. Each event appended
 * to a time series is assigned a sequence number that is is equal to
 * the sequence number of the preceding event plus one.
 *
 * @param event_metadata The event metadata
 *
 * @return the sequence number. If `event_metadata` is `NULL`, -1 is returned.
 */
long diffusion_time_series_event_metadata_get_sequence(const DIFFUSION_TIME_SERIES_EVENT_METADATA_T *event_metadata);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Return the event timestamp. Assigned by the server when the event is created.
 *
 * Events do not have unique timestamps. Events with different sequence
 * numbers may have the same timestamp.
 *
 * Subsequent events in a time series usually have timestamps that are
 * greater or equal to the timestamps of earlier events, but this is not
 * guaranteed due to changes to the time source used by the server.
 *
 * @param event_metadata The event metadata
 *
 * @return the difference, measured in milliseconds, between the time
 *         the server added the event to the time series and midnight,
 *         January 1, 1970 UTC. If `event_metadata` is `NULL`, -1 is returned.
 */
long diffusion_time_series_event_metadata_get_timestamp(const DIFFUSION_TIME_SERIES_EVENT_METADATA_T *event_metadata);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Returns a memory allocated string of the server-authenticated identity of the session that created the event.
 *
 * @param event_metadata The event metadata
 *
 * @return the principal that created the event, or `ANONYMOUS` if the session that created the
 *         event was not authenticated. If `event_metadata` is `NULL`, `NULL` will be returned.
 */
char *diffusion_time_series_event_metadata_get_author(const DIFFUSION_TIME_SERIES_EVENT_METADATA_T *event_metadata);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Returns a memory allocated copy of a `DIFFUSION_TIME_SERIES_EVENT_METADATA_T`
 *
 * @param event_metadata The event metadata to be copied.
 *
 * This should be freed wth `diffusion_time_series_event_metadata_free` when no longer needed.
 *
 * @return a copy of a `DIFFUSION_TIME_SERIES_EVENT_METADATA_T`. `NULL`, if `event_metadata` is NULL.
 */
DIFFUSION_TIME_SERIES_EVENT_METADATA_T *diffusion_time_series_event_metadata_dup(const DIFFUSION_TIME_SERIES_EVENT_METADATA_T *event_metadata);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Free a memory allocated `DIFFUSION_TIME_SERIES_EVENT_METADATA_T`
 *
 * @param event_metadata The `DIFFUSION_TIME_SERIES_EVENT_METADATA_T` to be freed.
 */
void diffusion_time_series_event_metadata_free(DIFFUSION_TIME_SERIES_EVENT_METADATA_T *event_metadata);

/**
 * Opaque diffusion time series event struct
 */
typedef struct DIFFUSION_TIME_SERIES_EVENT_T DIFFUSION_TIME_SERIES_EVENT_T;

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Return the sequence number identifying this event within its time series.
 * Assigned by the server when the event is created.
 *
 * Sequence numbers are unique within a time series. Each event appended
 * to a time series is assigned a sequence number that is is equal to
 * the sequence number of the preceding event plus one.
 *
 * @param event The time series event
 *
 * @return the sequence number. If `event` is `NULL`, -1 is returned.
 */
long diffusion_time_series_event_get_sequence(const DIFFUSION_TIME_SERIES_EVENT_T *event);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Return the event timestamp. Assigned by the server when the event is created.
 *
 * Events do not have unique timestamps. Events with different sequence
 * numbers may have the same timestamp.
 *
 * Subsequent events in a time series usually have timestamps that are
 * greater or equal to the timestamps of earlier events, but this is not
 * guaranteed due to changes to the time source used by the server.
 *
 * @param event The time series event
 *
 * @return the difference, measured in milliseconds, between the time
 *         the server added the event to the time series and midnight,
 *         January 1, 1970 UTC. If `event` is `NULL`, -1 is returned.
 */
long diffusion_time_series_event_get_timestamp(const DIFFUSION_TIME_SERIES_EVENT_T *event);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Returns a memory allocated string of the server-authenticated identity of the session that created the event.
 * This should be freed with `free` when no longer needed.
 *
 * @param event The time series event
 *
 * @return the principal that created the event, or `ANONYMOUS` if the session that created the
 *         event was not authenticated. If `event` is `NULL`, `NULL` will be returned.
 */
char *diffusion_time_series_event_get_author(const DIFFUSION_TIME_SERIES_EVENT_T *event);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Returns a memory allocated `DIFFUSION_VALUE_T`, the time series event's value.
 * This should be freed with `diffusion_value_free` when no longer needed.
 *
 * @param event The time series event
 *
 * @return the time series event's value. If `event` is `NULL`, `NULL` will be returned.
 */
DIFFUSION_VALUE_T *diffusion_time_series_event_get_value(const DIFFUSION_TIME_SERIES_EVENT_T *event);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief If this is an edit event, returns the metadata of the original event
 * that this event replaces; otherwise returns this event.
 *
 * The result is always the metadata of an original event, never that of
 * an edit event.
 *
 * @param event The time series event
 *
 * @return if the returned metadata is equal to `event` metadata, this is not an edit event;
 *         otherwise, the sequence number of the event replaced by this edit event. If `event`
 *         is `NULL`, `NULL` will be returned
 */
DIFFUSION_TIME_SERIES_EVENT_METADATA_T *diffusion_time_series_event_get_original_event(const DIFFUSION_TIME_SERIES_EVENT_T *event);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Return whether this is an edit event.
 *
 * @return 1 if this is an edit event, otherwise, 0 is returned showing that this is an original
 *         event. If `event` is `NULL`, -1 is returned.
 */
int diffusion_time_series_event_is_edit_event(const DIFFUSION_TIME_SERIES_EVENT_T *event);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Clone this event with a different value.
 *
 * This method is useful when further transformation of the received
 * value is needed, but the application wishes to preserve other event
 * attributes. For example, if a String value is received which the
 * session wishes to interpret as JSON, it can do the following.
 *
 * <pre>
 * DIFFUSION_VALUE_T *value = diffusion_time_series_event_get_value(event);
 * char *result;
 * read_diffusion_string_value(result, NULL);
 *
 * BUF_T *json_value = buf_create();
 * write_diffusion_json_value(result, json_value);
 *
 * DIFFUSION_TIME_SERIES_EVENT_T *event_json = diffusion_time_series_event_with_value(event, json_value);
 * </pre>
 *
 * All attributes other than the value will be copied from this event.
 * The result will only equal this event if newValue equals this event's
 * value.
 *
 * @param event The time series event
 * @param value The new value
 *
 * @return a copy of this event with a different value. If `event` or `value` is `NULL`, `NULL` will be
 *         returned.
 */
DIFFUSION_TIME_SERIES_EVENT_T *diffusion_time_series_event_with_value(const DIFFUSION_TIME_SERIES_EVENT_T *event, const BUF_T *value);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Get the `DIFFUSION_TIME_SERIES_EVENT_T` update value.
 *
 * @param value Value to read from.
 * @param event Pointer to a pointer where the time series event data will be stored
 * @param error Populated if an error occurs. Can be NULL.
 *
 * @return true if the value is successfully read and the `event` pointer value is
 *         set to a pointer containing the string data. False otherwise.
 */
bool read_diffusion_time_series_event(const DIFFUSION_VALUE_T *value, DIFFUSION_TIME_SERIES_EVENT_T **event, DIFFUSION_API_ERROR *error);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Returns a memory allocated copy of a `DIFFUSION_TIME_SERIES_EVENT_T`. `diffusion_time_series_event_free` should be called
 *        on this pointer when no longer needed.
 *
 * @param event the event to be copied.
 *
 * @return a copy of a `DIFFUSION_TIME_SERIES_EVENT_T`. NULL, if `event` is NULL.
 */
DIFFUSION_TIME_SERIES_EVENT_T *diffusion_time_series_event_dup(const DIFFUSION_TIME_SERIES_EVENT_T *event);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Free a memory allocated `DIFFUSION_TIME_SERIES_EVENT_T`
 *
 * @param event the `DIFFUSION_TIME_SERIES_EVENT_T` to be freed.
 */
void diffusion_time_series_event_free(DIFFUSION_TIME_SERIES_EVENT_T *event);

/**
 * @brief Callback when a time series append operation is successful.
 *
 * @param event_metadata The event metadata.
 * @param context        User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_append_cb)(const DIFFUSION_TIME_SERIES_EVENT_METADATA_T *event_metadata, void *context);

/**
 * @brief Callback when a time series edit operation is successful.
 *
 * @param event_metadata The event metadata.
 * @param context        User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_edit_cb)(const DIFFUSION_TIME_SERIES_EVENT_METADATA_T *event_metadata, void *context);

/**
 * @brief Structure supplied when issuing a `diffusion_time_series_append` request.
 */
typedef struct diffusion_time_series_append_params_s {
        /// Callback when a time series append operation
        /// is successful.
        on_append_cb on_append;
        /// The path of the time series topic to update.
        const char *topic_path;
        /// The type of the supplied value. This must match the
        /// value type of the datatype configured as the time series
        /// topic's `TIME_SERIES_EVENT_VALUE_TYPE` property.
        DIFFUSION_DATATYPE datatype;
        /// The event value.
        BUF_T *value;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_TIME_SERIES_APPEND_PARAMS_T;

/**
 * @brief Structure supplied when issuing a `diffusion_time_series_timestamp_append` request.
 */
typedef struct diffusion_time_series_timestamp_append_params_s {
        /// Callback when a time series append operation
        /// is successful.
        on_append_cb on_append;
        /// The path of the time series topic to update.
        const char *topic_path;
        /// The type of the supplied value. This must match the
        /// value type of the datatype configured as the time series
        /// topic's `TIME_SERIES_EVENT_VALUE_TYPE` property.
        DIFFUSION_DATATYPE datatype;
        /// The event value.
        BUF_T *value;
        /// The user supplied timestamp.
        long timestamp;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_TIME_SERIES_TIMESTAMP_APPEND_PARAMS_T;

/**
 * @brief Structure supplied when issuing a `diffusion_time_series_edit` request.
 */
typedef struct diffusion_time_series_edit_params_s {
        /// Callback when a time series edit operation
        /// is successful.
        on_edit_cb on_edit;
        /// The path of the time series topic to update.
        const char *topic_path;
        /// The sequence number of the original event to edit.
        long original_sequence;
        /// The type of the supplied value. This must match the
        /// value type of the datatype configured as the time series
        /// topic's `TIME_SERIES_EVENT_VALUE_TYPE` property.
        DIFFUSION_DATATYPE datatype;
        /// The event value.
        BUF_T *value;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_TIME_SERIES_EDIT_PARAMS_T;

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Update a time series topic by appending a new value.
 *
 * The server will add an event to the end of the time series based on the
 * supplied value, with a new sequence number, timestamp, and the author set
 * to the authenticated principal of the session.
 *
 * @param session   The current session. If NULL, this function returns immediately.
 * @param params    Parameters defining the `diffusion_time_series_append` request
 *                  and notification callbacks.
 * @param api_error Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_append(SESSION_T *session, const DIFFUSION_TIME_SERIES_APPEND_PARAMS_T params, DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Update a time series topic by appending a new value with a supplied
 * timestamp.
 *
 * The server will add an event to the end of the time series based on the
 * supplied value and timestam, with a new sequence number, timestamp, and the author
 * set to the authenticated principal of the session.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameters defining the `diffusion_time_series_timestamp_append` request
 *                      and notification callbacks.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_timestamp_append(SESSION_T *session, const DIFFUSION_TIME_SERIES_TIMESTAMP_APPEND_PARAMS_T params, DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Update a time series topic by appending a new value that overrides the
 * value of an existing event.
 *
 * The existing event is identified by its sequence number and must be an
 * original event.
 *
 * The server will add an edit event to the end of the time series based on
 * the supplied value, with a new sequence number, timestamp, and the author
 * set to the authenticated principal of the session.
 *
 * @param session   The current session. If NULL, this function returns immediately.
 * @param params    Parameters defining the `diffusion_time_series_edit` request
 *                  and notification callbacks.
 * @param api_error Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_edit(SESSION_T *session, const DIFFUSION_TIME_SERIES_EDIT_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Return a default range query that performs a value range query of an
 * entire time series.
 *
 * Further queries with different parameters can be configured using the
 * `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` functions (located in range-query.h).
 *
 * The result provides raw CBOR encoded bytes values, making it compatible with any
 * event data type supported by time series topics. A query with a more
 * specific value type can be configured using `diffusion_time_series_range_query_as`.
 *
 * A `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` equal to the one returned by this method
 * can be created from an arbitrary range query as follows.
 *
 * <pre>
 * DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query = diffusion_time_series_range_query();
 * diffusion_time_series_range_query_for_values(range_query, NULL);
 * diffusion_time_series_range_query_from_start(range_query, NULL);
 * diffusion_time_series_range_query_until_last(range_query, 0, NULL);
 * diffusion_time_series_range_query_limit(range_query, LONG_MAX, NULL);
 * diffusion_time_series_range_query_as(range_query, NULL, NULL);
 * </pre>
 *
 * @return a `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the default settings
 */
DIFFUSION_TIME_SERIES_RANGE_QUERY_T *diffusion_time_series_range_query(void);

/**
 * @ingroup PublicAPI_TimeSeries
 *
 * @brief Free a memory allocated `DIFFUSION_TIME_SERIES_RANGE_QUERY_T`
 *
 * @param range_query The `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` to be freed.
 */
void diffusion_time_series_range_query_free(DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query);

#endif
