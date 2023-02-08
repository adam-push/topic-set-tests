#ifndef _diffusion_range_query_h_
#define _diffusion_range_query_h_ 1

/*
 * Copyright © 2019, 2021 Push Technology Ltd., All Rights Reserved.
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
 * @file range-query.h
 *
 * Builder for queries that select a range of events from a time series.
 *
 * <ul>
 * <li>value range queries,
 * <li>latest edits edit range queries, and
 * <li>all edits edit range queries.
 * </ul>
 *
 * <p>
 * `diffusion_time_series_range_query` returns a default range query. Further
 * queries with different parameters can be configured using the functions of
 * this header file. Function calls can be chained together in a fluent manner to
 * create a query. For example:
 *
 * <pre>
 * // A value range query that selects up to 100 original events from the
 * // start of a time series.
 * DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query = diffusion_time_series_range_query();
 * diffusion_time_series_range_query_for_values(range_query, NULL);
 * diffusion_time_series_range_query_from_start(range_query, NULL);
 * diffusion_time_series_range_query_next(range_query, 100, NULL);
 * </pre>
 *
 * <h2>Creating value range queries</h2>
 *
 * <p>
 * A value range query returns a merged view of part of a time series. This
 * is the most common time series query and appropriate for most
 * applications.
 *
 * <p>
 * A value range query begins with the `diffusion_time_series_range_query_for_values`
 * operator, followed by the <em>view range</em>. The view range determines the range
 * of original events the time series that are of interest.
 * <p>
 * The events returned by the query are constrained by an optional <em>edit
 * range</em>, introduced by the `diffusion_time_series_range_query_edit_range`
 * operator. An event will only be included in the result if it is in the edit range.
 * Let's consider some examples to see how the view range and the edit range
 * interact.
 *
 * <table>
 * <tr>
 * <th>Query</th>
 * <th>Meaning</th>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_for_values(range_query, NULL);}</td>
 * <td>For each original event in the time series, either return the latest
 * edit event or if it has no edit events, return the original event.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_for_values(range_query, NULL);
 *  diffusion_time_series_range_query_from(range_query, 100, NULL);
 *  diffusion_time_series_range_query_to(range_query, 150, NULL);}</td>
 * <td>For each original event with a sequence number between 100 and 150
 * (inclusive), either return the latest edit event or if it has no edit
 * events, return the original event.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_for_values(range_query, NULL);
 *  diffusion_time_series_range_query_from(range_query, 100, NULL);
 *  diffusion_time_series_range_query_to(range_query, 150, NULL);
 *  diffusion_time_series_range_query_edit_range(range_query, NULL);
 *  diffusion_time_series_range_query_from(range_query, 400, NULL);}</td>
 * <td>For each original event with a sequence number between 100 and 150
 * (inclusive), return the latest edit event with a sequence number greater
 * than or equal to 400.
 * <p>
 * The result of this query will not include any original events because
 * there is no overlap between the view range and the edit range.</td>
 * </tr>
 * </table>
 *
 * <p>
 * Value range queries can be further refined using the
 * `diffusion_time_series_range_query_limit` and `diffusion_time_series_range_query_as`
 * operators.
 *
 * <h2>Creating edit range queries</h2>
 *
 * <p>
 * An edit range query returns an unmerged view of a time series than can
 * include both original events and the edit events that replace them. Edit
 * range queries are rarely needed &ndash; value range queries satisfy most
 * use cases.
 *
 * <p>
 * An edit range query begins with the `diffusion_time_series_range_query_for_edits`
 * operator, followed by the <em>view range</em>. The view range determines the range
 * of original events the time series that are of interest. The result will
 * only contain original events that are in the view range, and edit events
 * for original events in the view range.
 * <p>
 * The events returned by the query are constrained by an optional <em>edit
 * range</em>, introduced by the `diffusion_time_series_range_query_latest_edits` or
 * `diffusion_time_series_range_query_all_edits` operators. An event will only be
 * included in the result if it is in the edit range. Let's consider some example
 * edit range queries.
 *
 * <table>
 * <tr>
 * <th>Query</th>
 * <th>Meaning</th>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_for_edits(range_query, NULL);}</td>
 * <td>Return all events in a time series.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_for_edits(range_query, NULL);
 *  diffusion_time_series_range_query_from(range_query, 100, NULL);
 *  diffusion_time_series_range_query_to(range_query, 150, NULL);}</td>
 * <td>Return the original events with a sequence number between 100 and 150
 * (inclusive) and all edit events in the time series that refer to the
 * original events.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_for_edits(range_query, NULL);
 *  diffusion_time_series_range_query_from(range_query, 100, NULL);
 *  diffusion_time_series_range_query_to(range_query, 150, NULL);
 *  diffusion_time_series_range_query_latest_edits(range_query, NULL);}</td>
 * <td>Return the original events with a sequence number between 100 and 150
 * (inclusive) and the latest edit events in the time series that refer to
 * the original events.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_for_edits(range_query, NULL);
 *  diffusion_time_series_range_query_from(range_query, 100, NULL);
 *  diffusion_time_series_range_query_to(range_query, 150, NULL);
 *  diffusion_time_series_range_query_all_edits(range_query, NULL);
 *  diffusion_time_series_range_query_from(range_query, 400, NULL);}
 * </td>
 * <td>For each original event with a sequence number between 100 and 150,
 * (inclusive) return all edit events with a sequence number greater than or
 * equal to 400.
 * <p>
 * The result of this query will not include any original events because
 * there is no overlap between the view range and the edit range.</td>
 * </tr>
 * </table>
 *
 * <p>
 * Edit range queries can be further refined using the `diffusion_time_series_range_query_limit`
 * and `diffusion_time_series_range_query_as` operators.
 *
 * <h2>Range expressions</h2>
 * <p>
 * Range expressions are used to specify the view and edit ranges in value
 * range and edit range queries. Each range expression has an
 * <em>anchor</em> that determines where to start, and a <em>span</em> that
 * determines where the range ends. Both anchor and span are
 * <em>inclusive</em> &ndash; if an anchor or span falls on an event, the
 * event is included in the result.
 *
 * Both anchor and the span are optional. If the anchor is unspecified, the
 * range begins at the start of the time series. If the span is unspecified,
 * the range continues until the end of the time series.
 *
 * <h3>Anchors</h3>
 *
 * <table>
 * <tr>
 * <th>Anchor</th>
 * <th>Meaning</th>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_from}</td>
 * <td>Sets the anchor at an absolute sequence number.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_from_start}</td>
 * <td>Sets the anchor at the start of the time series.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_from_time}</td>
 * <td>Sets the anchor at an absolute time.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_from_last}</td>
 * <td>Sets the anchor at a relative offset before the end of the time
 * series. For value range queries, `count` is the number of original
 * events. For edit range queries, `count` is the number of events of
 * any type.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_from_last_millis}</td>
 * <td>Sets the anchor at a relative time before the timestamp of the last
 * event of the time series.</td>
 * </tr>
 * </table>
 * <p>
 * An anchor point can be before the start or after the end of the time
 * series.
 *
 * <h3>Spans</h3>
 *
 * <table>
 * <tr>
 * <th>Span</th>
 * <th>Meaning</th>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_to}</td>
 * <td>The range ends at an absolute sequence number. The `sequence`
 * argument may be before or after the anchor.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_to_start}</td>
 * <td>The range ends at the start of the time series.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_to_time}</td>
 * <td>The range ends at an absolute time. The `instant` argument may
 * be before or after the anchor.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_next}</td>
 * <td>The range ends at an event that is a relative number of events after
 * the anchor. For value range queries, `count` is the number of
 * original events. For edit range queries, `count` is the number of
 * events of any type.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_next_millis}</td>
 * <td>The range ends at an event that is a relative time after the
 * anchor.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_previous}</td>
 * <td>The range ends at an event that is a relative number of events before
 * the anchor. For value range queries, `count` is the number of
 * original events. For edit range queries, `count` is the number of
 * events of any type.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_previous_millis}</td>
 * <td>The range ends at an event that is a relative time before the
 * anchor.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_until_last}</td>
 * <td>The range ends at an event that is a relative number of events before
 * the end of the time series. For value range queries, `count` is the
 * number of original events. For edit range queries, `count` is the
 * number of events of any type.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_until_last_millis}</td>
 * <td>The range ends at an event that is a relative time before the
 * timestamp of the last event of the time series.</td>
 * </tr>
 * </table>
 *
 * <p>
 * A span can specify an end point that is before the start or after the end
 * of the time series.
 * <p>
 * If the span specifies an end point after the anchor, the range includes
 * the first event at or following the anchor and ends at the last event at
 * or preceding the end point. If the span specifies an end point before the
 * anchor, the range includes the first event at or preceding the anchor and
 * ends at the last event at or after the end point.
 *
 * <h2>Using the builder functions</h2>
 *
 * <p>
 * Although the natural order of operators in a query is as shown in the
 * syntax diagrams above, range query builder functions &ndash; those that
 * return another range query &ndash; can be applied in any order with the
 * following exceptions:
 * <ul>
 * <li>`diffusion_time_series_range_query_edit_range` only applies to value
 * range queries, so cannot follow `diffusion_time_series_range_query_for_edits`
 * without an intervening `diffusion_time_series_range_query_for_values`;
 * <li>`diffusion_time_series_range_query_latest_edits` and
 * `diffusion_time_series_range_query_all_edits` only apply to edit range queries,
 * so cannot follow `diffusion_time_series_range_query_for_values` without
 * an intervening `diffusion_time_series_range_query_for_edits`.
 * </ul>
 *
 * <p>
 * Each function overrides some configuration of the range query to which it is
 * applied, as summarized in the following table.
 *
 * <table>
 * <tr>
 * <th>Builder function</th>
 * <th>Operator type</th>
 * <th>Overridden configuration</th>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_for_values}</td>
 *
 * <td>Value range</td>
 * <td>Overrides the existing query type to create a new value range query.
 * Overrides the existing view range with a new view range that selects the
 * entire time series. The existing edit range is copied unchanged.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_for_edits}</td>
 * <td>Value range</td>
 * <td>Overrides the existing query type to create a new edit range query
 * that includes all edits. Overrides the existing view range with a new
 * view range that selects the entire time series. The existing edit range
 * is copied unchanged.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_edit_range}</td>
 * <td>Edit range</td>
 * <td>Overrides the existing edit range with a new edit range that selects
 * the entire time series. The existing view range is copied unchanged.<br>
 * Returns `false` and populates `error` if this is not a value range
 * query.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_latest_edits}<br>
 * {@code diffusion_time_series_range_query_all_edits}</td>
 * <td>Edit range</td>
 * <td>Overrides the existing edit range with a new edit range that selects
 * the entire time series. The existing view range is copied unchanged.
 * <br>
 * Returns `false` and populates `error` if this is not an edit range
 * query.</td>
 * </tr>
 *
 * <tr>
 * <td>{@code diffusion_time_series_range_query_from}<br>
 * {@code diffusion_time_series_range_query_from_start}<br>
 * {@code diffusion_time_series_range_query_from_last}</td>
 * <td>Anchor</td>
 * <td>Overrides the anchor of the current range.</td>
 * </tr>
 *
 * <tr>
 * <td>{@code diffusion_time_series_range_query_to}<br>
 * {@code diffusion_time_series_range_query_to_start}<br>
 * {@code diffusion_time_series_range_query_next}<br>
 * {@code diffusion_time_series_range_query_previous}<br>
 * {@code diffusion_time_series_range_query_until_last}</td>
 * <td>Span</td>
 * <td>Overrides the span of the current range.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_limit}</td>
 * <td>Limit</td>
 *
 * <td>Overrides the limit.</td>
 * </tr>
 * <tr>
 * <td>{@code diffusion_time_series_range_query_as}</td>
 * <td>Query value type</td>
 *
 * <td>Overrides the query value type.</td>
 * </tr>
 * </table>
 */

#include <stdlib.h>
#include <stdbool.h>

#include "session.h"
#include "diffusion-api-error.h"
#include "datatypes.h"

/**
 * Opaque diffusion time series range query struct
 */
typedef struct DIFFUSION_TIME_SERIES_RANGE_QUERY_T DIFFUSION_TIME_SERIES_RANGE_QUERY_T;

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` to perform a value range
 * query with the view range set to the entire time series.
 *
 * Operator type: value range
 *
 * @param range_query The range query to be mutated
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_for_values(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` to perform an edit range
 * query with the view range set to the entire time series.
 *
 * Operator type: value range
 *
 * @param range_query The range query to be mutated
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_for_edits(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` to perform a value range
 * query with the edit range set to the entire time series.
 *
 * This operator can only be applied to value range queries. If this operator is applied
 * to a non-value range query, false will be returned and `error`, if non-NULL, will be
 * populated.
 *
 * The default query returned by `diffusion_time_series_range_query` is a
 * value range query. The `diffusion_time_series_range_query_for_values` function
 * can be used to create a value range query from an edit range query.
 *
 * @param range_query The range query to be mutated
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_edit_range(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` to perform an edit range
 * query with the edit range that selects all edits in the entire time
 * series.
 *
 * This operator can only be applied to edit range queries. If this operator is applied
 * to a non-edit range query, false will be returned and `error`, if non-NULL, will be
 * populated.
 *
 * The default query returned by `diffusion_time_series_range_query` is a
 * value range query. The `diffusion_time_series_range_query_for_edits` operator can
 * be used to create an edit range query from a value range query.
 *
 * @param range_query The range query to be mutated
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_all_edits(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` to perform an edit range
 * query with the edit range that selects latest edits in the entire time
 * series.
 *
 * This operator can only be applied to edit range queries. If this operator is applied
 * to a non-edit range query, false will be returned and `error`, if non-NULL, will be
 * populated.
 *
 * The default query returned by `diffusion_time_series_range_query` is a
 * value range query. The `diffusion_time_series_range_query_for_edits` operator
 * can be used to create an edit range query from a value range query.
 *
 * Operator type: edit range
 *
 * @param range_query The range query to be mutated
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_latest_edits(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the anchor of the current range
 * configured to be an absolute sequence number.
 *
 * Operator type: anchor
 *
 * @param range_query The range query to be mutated
 * @param sequence    Absolute sequence number specifying the anchor of
 *                    the returned range
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_from(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, long sequence, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the anchor of the current range
 * configured to be an absolute sequence number.
 *
 * Operator type: anchor
 *
 * @param range_query The range query to be mutated
 * @param t           Absolute time specifying the end of the range
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_from_time(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, time_t t, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the anchor of the current range
 * configured to be the start of the time series.
 *
 * There is a difference between `diffusion_time_series_range_query_from_start(range_query, error)` and
 * `diffusion_time_series_range_query_from(range_query, 0, error)` if the range also ends before the first.
 * For example:
 *
 * <pre>
 * diffusion_time_series_range_query_from_start(range_query, error);
 * diffusion_time_series_range_query_to_start(range_query, error);
 * </pre>
 *
 * is always empty, but
 *
 * <pre>
 * diffusion_time_series_range_query_from_start(range_query, error);
 * diffusion_time_series_range_query_to(range_query, 0, error);
 * </pre>
 *
 * includes the event with sequence number 0.
 *
 * @param range_query The range query to be mutated
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_from_start(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the anchor of the current range
 * configured to be a relative offset before the end of the time series.
 *
 * Operator type: anchor
 *
 * @param range_query The range query to be mutated
 * @param count       Specifies the anchor as a number of events before the
 *                    end of the time series. For value range queries, count is the
 *                    number of original events. For edit range queries, count is
 *                    the number of events of any type.
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_from_last(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, long count, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the anchor of the current range
 * configured to be a relative time from the timestamp of the last event in the time series.
 *
 * Operator type: anchor
 *
 * @param range_query The range query to be mutated
 * @param time_span   Specifies anchor as a number of milliseconds relative
 *                    to the timestamp of the latest event in the time series.
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_from_last_millis(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, long time_span, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the span of the current range
 * configured to end at an absolute sequence number.
 *
 * Operator type: span
 *
 * @param range_query The range query to be mutated
 * @param sequence    Absolute sequence number specifying the end of the
 *                    returned range
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_to(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, long sequence, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the span of the current range
 * configured to end at an absolute sequence number.
 *
 * Operator type: span
 *
 * @param range_query The range query to be mutated
 * @param t           Absolute time specifying the end of the range
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_to_time(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, time_t t, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the span of the current range
 * configured to end at the start of the time series.
 *
 * There is a difference between `diffusion_time_series_range_query_from_start(range_query, error)` and
 * `diffusion_time_series_range_query_from(range_query, 0, error)` if the range also ends before the first.
 * For example:
 *
 * <pre>
 * diffusion_time_series_range_query_from_start(range_query, error);
 * diffusion_time_series_range_query_to_start(range_query, error);
 * </pre>
 *
 * is always empty, but
 *
 * <pre>
 * diffusion_time_series_range_query_from_start(range_query, error);
 * diffusion_time_series_range_query_to(range_query, 0, error);
 * </pre>
 *
 * includes the event with sequence number 0.
 *
 * Operator type: span
 *
 * @param range_query The range query to be mutated
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_to_start(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the span of the current range
 * configured to select a range of events following the anchor.
 *
 * Operator type: span
 *
 * @param range_query The range query to be mutated
 * @param count       Specifies the end of the range of events to select
 *                    following the anchor. For value range queries, count is the
 *                    number of original events. For edit range queries, count is
 *                    the number of events of any type.
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_next(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, long count, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the span of the current range
 * configured to select a temporal range of events following the anchor.
 *
 * Operator type: span
 *
 * @param range_query The range query to be mutated
 * @param time_span   The time span in milliseconds of events following the
 *                    anchor to select
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_next_millis(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, long time_span, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the span of the current range
 * configured to select a range of events preceding the anchor.
 *
 * Operator type: span
 *
 * @param range_query The range query to be mutated
 * @param count       Specifies the end of the range of events to select
 *                    preceding the anchor. For value range queries, count is the
 *                    number of original events. For edit range queries, count is
 *                    the number of events of any type.
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_previous(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, long count, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the span of the current range
 * configured to select a temporal range of events preceding the anchor.
 *
 * Operator type: span
 *
 * @param range_query The range query to be mutated
 * @param time_span   The time span in milliseconds of events preceding the
 *                    anchor to select
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_previous_millis(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, long time_span, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the span of the current range
 * configured to end a number of events before the end of the time series.
 *
 * Operator type: span
 *
 * @param range_query The range query to be mutated
 * @param count       Specifies the end of the range of events to select as a
 *                    number of events before the end of the time series. For value
 *                    range queries, count is the number of original events. For
 *                    edit range queries, count is the number of events of any type.
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_until_last(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, long count, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with the span of the current range
 * configured to end at a relative time from the timestamp of the last event in the time series.
 *
 * Operator type: span
 *
 * @param range_query The range query to be mutated
 * @param time_span   Specifies the end of the range of events to select as
 *                    a number of milliseconds relative to the timestamp of the
 *                    latest event in the time series
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_until_last_millis(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, long time_span, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` to return at most count events.
 *
 * If the query would otherwise select more than count events, only the
 * latest count values (those with the highest sequence numbers) are
 * returned.
 *
 * `diffusion_time_series_query_result_is_complete` can be used to determine
 * whether a query has returned an incomplete result.
 *
 * Operator type: limit
 *
 * @param range_query The range query to be mutated
 * @param count       The maximum number of events to return
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_limit(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, long count, DIFFUSION_API_ERROR *error);

/**
 * @brief Mutates this `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` with a different query value type.
 *
 * A query can only be evaluated successfully against time series topics
 * with a compatible event data type. If a query function is called for a
 * time series topic with an incompatible event data type, the `on_error` callback from
 * the function call's `DIFFUSION_TIME_SERIES_RANGE_QUERY_PARAMS_T` will be invoked.
 *
 * The default range query does not have a bound datatype value which means, by default,
 * it is compatible with all time series value data types. When a value is received, the appropriate
 * datatype 'read' function should be used to extract the type specific value.
 *
 * Operator type: query value type
 *
 * @param range_query The range query to be mutated
 * @param datatype    Pointer to the datatype value. This can be `NULL` if the range
 *                    query is required to be compatible with all time series value
 *                    data types.
 * @param error       Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_range_query_as(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query, DIFFUSION_DATATYPE *datatype, DIFFUSION_API_ERROR *error);

/**
 * @brief Returns a memory allocated copy of a `DIFFUSION_TIME_SERIES_RANGE_QUERY_T`. `diffusion_time_series_range_query_free` should be
 *        called on the pointer when no longer needed.
 *
 * @param range_query The range query to be copied.
 *
 * @return a copy of a `DIFFUSION_TIME_SERIES_RANGE_QUERY_T`. NULL, if `range_query` is NULL.
 */
DIFFUSION_TIME_SERIES_RANGE_QUERY_T *diffusion_time_series_range_query_dup(const DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query);

/**
 * @brief Free a memory allocated `DIFFUSION_TIME_SERIES_RANGE_QUERY_T`
 *
 * @param range_query The `DIFFUSION_TIME_SERIES_RANGE_QUERY_T` to be freed.
 */
void diffusion_time_series_range_query_free(DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query);

/**
 * Opaque diffusion time series query result struct
 */
typedef struct DIFFUSION_TIME_SERIES_QUERY_RESULT_T DIFFUSION_TIME_SERIES_QUERY_RESULT_T;

/**
 * Describes the structural properties of a query result.
 */
typedef enum {
        /**
         * @brief The result is ordered by the original event sequence number,
         * presenting edit events instead of the original events they
         * replace.
         *
         * The <em>original event sequence number</em> of an event `e`
         * is `diffusion_time_series_event_get_sequence(diffusion_time_series_event_get_original_event(e))`.
         * It is equal to `diffusion_time_series_event_get_sequence(e)`, if and only if `e` is an original
         * event.
         *
         * The result has the following properties:
         *
         * <ul>
         * <li>The sequence of each event in the result is unique.
         * <li>The original event sequence of each event in the result is
         * unique.
         * <li>The result is ordered by original event sequence. The
         * original event sequence of each subsequent event in the result is
         * greater than its predecessor.
         * <li>If no events have been removed from the time series, the
         * original event sequence of each subsequent event is one greater
         * than its predecessor.
         * <li>If an event is an original event, the query found no
         * corresponding edit events.
         * <li>If an event is an edit event, its timestamp attribute may lie
         * outside the query range. Consequentially, the sequence and
         * timestamp attributes of the events may be non-sequential.
         * </ul>
         */
        VALUE_EVENT_STRUCTURE,

        /**
         * @brief The result is presented in time series order.
         *
         * The result has the following properties:
         *
         * <ul>
         * <li>The sequence of each event in the result is unique.
         * <li>The result is ordered by sequence. The sequence of each
         * subsequent event in the result is greater than its predecessor.
         * <li>Edit event timestamps may lie outside the query range.
         * <li>The result can have multiple edit events for the same
         * original event.
         * </ul>
         */
        EDIT_EVENT_STRUCTURE
} DIFFUSION_QUERY_RESULT_STRUCTURE_T;

/**
 * @brief Returns a list of `DIFFUSION_TIME_SERIES_EVENT_T`s selected by the query.
 *
 * @param query_result The range query result
 *
 * @return a list of time series events selected by the query. If `query_result` is `NULL`, `NULL`
 *         will be returned.
 */
LIST_T *diffusion_time_series_query_result_get_events(const DIFFUSION_TIME_SERIES_QUERY_RESULT_T *query_result);

/**
 * @brief Returns the number of events selected by the query.
 *
 * @param query_result The range query result
 *
 * @return the number of events selected by the query. If `query_result` is NULL, -1
 *         will be returned.
 */
long diffusion_time_series_query_result_get_selected_count(const DIFFUSION_TIME_SERIES_QUERY_RESULT_T *query_result);

/**
 * @brief Returns whether this result includes all events selected by the
 * query.
 *
 * Equivalent to:
 *
 * <pre>
 * return diffusion_time_series_query_result_get_selected_count(result) == list_get_size(diffusion_time_series_query_result_get_events(result));
 * </pre>
 *
 * @param query_result The range query result
 *
 * @return 1 if the query result is complete, 0 if it is not. -1 will be return if `query_result` is `NULL`.
 */
int diffusion_time_series_query_result_is_complete(const DIFFUSION_TIME_SERIES_QUERY_RESULT_T *query_result);

/**
 * @brief Returns a description of the structure of the query result.
 *
 * @param query_result The range query result
 *
 * @return a `DIFFUSION_QUERY_RESULT_STRUCTURE_T` that describes the structure of the result
 *         stream. -1 will be return if `query_result` is `NULL`.
 */
DIFFUSION_QUERY_RESULT_STRUCTURE_T diffusion_time_series_query_result_structure(const DIFFUSION_TIME_SERIES_QUERY_RESULT_T *query_result);

/**
 * @brief Merge this result with `other`, combining original events and
 * edit events, to produce a `DIFFUSION_TIME_SERIES_QUERY_RESULT_T` of type
 * `VALUE_EVENT_STRUCTURE`
 *
 * The following rules are applied to calculate the result:
 * <ul>
 * <li>If this result and `other` have an event with equal
 * sequence numbers, the event from `other` is selected.
 * <li>An edit event is selected in place of its original event.
 * <li>If there are multiple edit events of an original edit, the one
 * with the highest sequence is selected.
 * </ul>
 *
 * The returned result implements `diffusion_time_series_query_result_is_complete`
 * to return `true` and `diffusion_time_series_query_result_get_selected_count` to return
 * the count of events in the stream, regardless of whether this result is complete.
 *
 * @param query_result The query result to be merged
 * @param other        The query result to be merged
 *
 * @return a merged query result. If `query_result` or `other` is `NULL`, then `NULL` will be returned.
 */
DIFFUSION_TIME_SERIES_QUERY_RESULT_T *diffusion_time_series_query_result_merge(const DIFFUSION_TIME_SERIES_QUERY_RESULT_T *query_result, const DIFFUSION_TIME_SERIES_QUERY_RESULT_T *other);

/**
 * @brief Returns a memory allocated copy of a `DIFFUSION_TIME_SERIES_QUERY_RESULT_T`
 *
 * @param query_result The query result to be copied.
 *
 * @return a copy of a `DIFFUSION_TIME_SERIES_QUERY_RESULT_T`. NULL, if `query_result` is NULL.
 */
DIFFUSION_TIME_SERIES_QUERY_RESULT_T *diffusion_time_series_query_result_dup(const DIFFUSION_TIME_SERIES_QUERY_RESULT_T *query_result);

/**
 * @brief Free a memory allocated `DIFFUSION_TIME_SERIES_QUERY_RESULT_T`
 *
 * @param query_result The `DIFFUSION_TIME_SERIES_QUERY_RESULT_T` to be freed.
 */
void diffusion_time_series_query_result_free(DIFFUSION_TIME_SERIES_QUERY_RESULT_T *query_result);

/**
 * @brief Callback when a range query result is received.
 *
 * @param query_result The query result.
 * @param context      User supplied context.
 *
 * @return HANDLERS_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_query_result_cb)(const DIFFUSION_TIME_SERIES_QUERY_RESULT_T *query_result, void *context);

/**
 * @brief Structure supplied when issuing a `diffusion_time_series_select_from` request.
 */
typedef struct diffusion_time_series_range_query_params_s {
        /// The topic path
        const char *topic_path;
        /// The range query
        DIFFUSION_TIME_SERIES_RANGE_QUERY_T *range_query;
        /// Callback on query result
        on_query_result_cb on_query_result;
        /// Callback on error
        ERROR_HANDLER_T on_error;
        /// Callback on discard
        DISCARD_HANDLER_T on_discard;
        /// User supplied context
        void *context;
} DIFFUSION_TIME_SERIES_RANGE_QUERY_PARAMS_T;

/**
 * @brief Specifies a logical start point within the topic tree.
 *
 * @param session     The session handle. If NULL, the function returns immediately.
 * @param params      Parameters describing the range query and callback handlers
 *                    which may be invoked in response.
 * @param api_error   Populated if an error occurs. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_time_series_select_from(SESSION_T *session, const DIFFUSION_TIME_SERIES_RANGE_QUERY_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

#endif