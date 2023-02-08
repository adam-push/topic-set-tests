#ifndef _diffusion_metrics_h_
#define _diffusion_metrics_h_ 1

/*
 * Copyright © 2021 - 2022 Push Technology Ltd., All Rights Reserved.
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
 * @file metrics.h
 *
 * Allows a client to configure metric collectors.
 *
 * Diffusion servers provide metrics which are made available in several ways:-
 * <ul>
 *  <li>Java Management Extensions (JMX) MBeans.
 *  <li>Through the Diffusion Management Console.
 *  <li>As endpoints for Prometheus.
 * </ul>
 *
 * Metric collectors allow custom aggregation of metrics that are relevant to
 * your application. There are no default metric collectors, only the ones that
 * you create.
 *
 * There are two types of metric collector: Session Metric Collectors and Topic
 * Metric Collectors.
 *
 * For full details regarding the configuration and operation of metric
 * collectors see the user manual.
 *
 * ### Session Metric Collectors
 *
 * These can be configured to record metric data for a subset of all sessions,
 * specified with a session filter.
 *
 * The set of metrics recorded by each session metric collector is the same as
 * those recorded for the whole server. For full details of session metrics, see
 * the table in the user manual.
 *
 * If the session filters of two different session metric collectors select the
 * same session, both will record metrics for that session. It is only valid to
 * add the metrics of different session metric collectors if their session
 * filters select distinct sets of sessions.
 *
 * You can optionally group the sessions within a collector by session
 * properties.
 *
 * ### Topic Metric Collectors
 *
 * These can be configured to record metric data for a subset of all topics,
 * specified with a topic selector.
 *
 * You can optionally group the topics within a collector by topic type.
 *
 * The set of metrics recorded by each topic metric collector is the same as
 * those recorded for the whole server. For full details of topic metrics, see
 * the table in the user manual.
 *
 * If the topic selectors of two different topic metric collectors select the
 * same topic, both will record metrics for that topic. It is only valid to add
 * the metrics of different topic metric collectors if their topic selectors
 * select distinct sets of topics.
 *
 * ### Access control
 *
 * The following access control restrictions are applied:
 * <ul>
 *  <li>To {@link #diffusion_metrics_put_session_metric_collector put} or
 *      {@link #diffusion_metrics_remove_session_metric_collector remove} a
 *      session metric collector, a session needs the {@link GLOBAL_PERMISSION_CONTROL_SERVER
 *      CONTROL_SERVER} global permission.
 *  <li>To {@link #diffusion_metrics_put_topic_metric_collector put} or
 *      {@link #diffusion_metrics_remove_topic_metric_collector remove} a topic metric
 *      collector, a session needs the {@link GLOBAL_PERMISSION_CONTROL_SERVER CONTROL_SERVER}
 *      global permission.
 *  <li>To {@link #diffusion_metrics_list_session_metric_collectors list} session metric
 *      collectors or {@link #diffusion_metrics_list_topic_metric_collectors topic metric collectors}, a session needs
 *      the {@link GLOBAL_PERMISSION_VIEW_SERVER VIEW_SERVER} global permission.
 * </ul>
 *
 * @since 6.7
 */

#include "session.h"
#include "misc/deprecate.h"
#include "diffusion-api-error.h"


/**
 * @ingroup PublicAPI_Metrics
 *
 * Opaque Session Metric Collector Builder struct
 */
typedef struct DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T;


/**
 * @ingroup PublicAPI_Metrics
 *
 * Opaque Session Metric Collector struct
 */
typedef struct DIFFUSION_SESSION_METRIC_COLLECTOR_T DIFFUSION_SESSION_METRIC_COLLECTOR_T;


/**
 * @ingroup PublicAPI_Metrics
 *
 * Opaque Topic Metric Collector Builder struct
 */
typedef struct DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T;


/**
 * @ingroup PublicAPI_Metrics
 *
 * Opaque Topic Metric Collector struct
 */
typedef struct DIFFUSION_TOPIC_METRIC_COLLECTOR_T DIFFUSION_TOPIC_METRIC_COLLECTOR_T;



/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector_Builder
 *
 * @brief Initialize a new session metric collector builder.
 * <code>diffusion_session_metric_collector_builder_free</code> should be called on this pointer
 * when no longer needed.
 *
 * @return the session metric collector builder
 *
 * @since 6.7
 */
DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *diffusion_session_metric_collector_builder_init();


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector_Builder
 *
 * @brief Reset the session metric collector builder's internal values.
 *
 * @param builder           the session metric collector builder
 *
 * @return  the session metric collector builder
 *
 * @since 6.7
 */
DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *diffusion_session_metric_collector_builder_reset(
        DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *builder);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector_Builder
 *
 * @brief Specifies whether the metric collector should export metrics to
 * Prometheus or not.
 *
 * The default is that metrics are not exported to Prometheus.
 *
 * @param builder                   the session metric collector builder
 * @param export_to_prometheus      true to export metrics to Prometheus
 *
 * @return  the session metric collector builder
 *
 * @since 6.7
 */
DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *diffusion_session_metric_collector_builder_export_to_prometheus(
        DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *builder,
        bool export_to_prometheus);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector_Builder
 *
 * @brief Set the maximum number of groups maintained by the metric collector.
 *
 * By default, the number of groups is not limited.
 *
 * @param builder           the session metric collector builder
 * @param limit             a positive integer
 *
 * @return  the session metric collector builder
 *
 * @since 6.8
 */
DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *diffusion_session_metric_collector_builder_maximum_groups(
        DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *builder,
        int limit);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector_Builder
 *
 * @brief Limit the number of groups maintained by this metric collector.
 *
 * Session metric collectors can {@link diffusion_session_metric_collector_get_group_by_properties
 * group metrics by properties}.
 * This property places an upper limit on the number of groups that will be maintained for the
 * metric collector.
 *
 * For example, if a session metric collector groups by `$SessionId` and `maximum_groups` is 10,
 * then metrics will only be collected for the first 10 sessions.
 *
 * @param builder                   the session metric collector builder
 * @param export_to_prometheus      true to export metrics to Prometheus
 *
 * @return  the session metric collector builder
 *
 * @since 6.7
 */
DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *diffusion_session_metric_collector_builder_export_to_prometheus(
        DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *builder,
        bool export_to_prometheus);

/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector_Builder
 *
 * @brief Adds the name of a session property to group by to the list known
 * to this builder.
 *
 * By default a builder will initially have no session properties to
 * group by set.
 *
 * @param builder           the session metric collector builder
 * @param property_name     the name of the session property. See
 *                          {@link session.h} for details of session properties
 *
 * @return  the session metric collector builder
 *
 * @since 6.7
 */
DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *diffusion_session_metric_collector_builder_group_by_property(
        DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *builder,
        char *property_name);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector_Builder
 *
 * @brief Specifies a list of session property names to group by, replacing
 * any current list known to this builder.
 *
 * @param builder           the session metric collector builder
 * @param property_names    a list of session property names. See
 *                          {@link session.h} for details of session properties
 *
 * @return  the session metric collector builder
 *
 * @since 6.7
 */
DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *diffusion_session_metric_collector_builder_group_by_properties(
        DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *builder,
        LIST_T *property_names);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector_Builder
 *
 * @brief Specifies whether the metric collector should remove any metrics
 * that have no matches.
 *
 * The default is that the metric collector will not remove metrics
 * with no matches.
 *
 * @param builder           the session metric collector builder
 * @param remove            true to indicate that metrics with no matches
 *                          should be removed
 *
 * @return  the session metric collector builder
 *
 * @since 6.7
 */
DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *diffusion_session_metric_collector_builder_remove_metrics_with_no_matches(
        DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *builder,
        bool remove);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector_Builder
 *
 * @brief Create a new session metric collector using the values currently known by the builder.
 *
 * @param builder           the session metric collector builder
 * @param name              the name of the session metric collector
 * @param session_filter    the session filter indicating the sessions this collector should
 *                          apply to. The format of a session properties filter is documented
 *                          in {@file session.h}
 *
 * @return  a new session metric collector if the operation was successful, NULL otherwise.
 *
 * @since 6.7
 *
 * @deprecated since 6.8 and will be removed in a future release
 *             use `diffusion_session_metric_collector_builder_create` instead
 */
DEPRECATED(DIFFUSION_SESSION_METRIC_COLLECTOR_T *diffusion_session_metric_collector_builder_create_collector(
        DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *builder,
        char *name,
        char *session_filter))


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector_Builder
 *
 * @brief Create a new session metric collector using the values currently known by the builder.
 *
 * @param builder           the session metric collector builder
 * @param name              the name of the session metric collector
 * @param session_filter    the session filter indicating the sessions this collector should
 *                          apply to. The format of a session properties filter is documented
 *                          in {@file session.h}
 * @param api_error         populated on API error. Can be NULL.
 *
 * @return  a new session metric collector if the operation was successful, NULL otherwise.
 *          In this case, if a non-NULL <code>api_error</code> pointer has been provided,
 *          this will be populated with the error information and should be freed with
 *          <code>diffusion_api_error_free</code>.
 *
 * @since 6.8
 */
DIFFUSION_SESSION_METRIC_COLLECTOR_T *diffusion_session_metric_collector_builder_create(
        DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *builder,
        char *name,
        char *session_filter,
        DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector_Builder
 *
 * @brief Free a session metric collector builder.
 *
 * @param builder           the session metric collector builder
 *
 * @since 6.7
 */
void diffusion_session_metric_collector_builder_free(
        DIFFUSION_SESSION_METRIC_COLLECTOR_BUILDER_T *builder);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector
 *
 * @brief Free a session metric collector.
 *
 * @param collector         the session metric collector
 *
 * @since 6.7
 */
void diffusion_session_metric_collector_free(DIFFUSION_SESSION_METRIC_COLLECTOR_T *collector);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector
 *
 * @brief Returns the session filter of the session metric collector.
 *
 * @param collector         the session metric collector
 * @param session_filter    the variable that will receive the session filter if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.7
 */
bool diffusion_session_metric_collector_get_session_filter(
        DIFFUSION_SESSION_METRIC_COLLECTOR_T *collector,
        char **session_filter);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector
 *
 * @brief Returns the name of the session metric collector.
 *
 * @param collector         the session metric collector
 * @param name              the variable that will receive the name if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.7
 */
bool diffusion_session_metric_collector_get_name(
        DIFFUSION_SESSION_METRIC_COLLECTOR_T *collector,
        char **name);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector
 *
 * @brief Indicates whether metrics with no matches should be removed.
 *
 * @param collector         the session metric collector
 * @param removes           the variable that will receive the bool value if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.7
 */
bool diffusion_session_metric_collector_removes_metrics_with_no_matches(
        DIFFUSION_SESSION_METRIC_COLLECTOR_T *collector,
        bool *removes);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector
 *
 * @brief Indicates whether the metric collector exports to Prometheus.
 *
 * @param collector                 the session metric collector
 * @param exports_to_prometheus     the variable that will receive the bool value if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.7
 */
bool diffusion_session_metric_collector_exports_to_prometheus(
        DIFFUSION_SESSION_METRIC_COLLECTOR_T *collector,
        bool *exports_to_prometheus);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector
 *
 * @brief Limit the number of groups maintained by this metric collector.
 *
 * Session metric collectors can {@link diffusion_session_metric_collector_get_group_by_properties
 * group metrics by properties}.
 * This property places an upper limit on the number of groups that will be maintained for the
 * metric collector.
 *
 * For example, if a session metric collector groups by `$SessionId` and `maximum_groups` is 10,
 * then metrics will only be collected for the first 10 sessions.
 *
 * @param collector         the session metric collector
 * @param maximum_groups    the variable that will receive the `int` value if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.8
 */
bool diffusion_session_metric_collector_maximum_groups(
        DIFFUSION_SESSION_METRIC_COLLECTOR_T *collector,
        int *maximum_groups);

/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector
 *
 * @brief Returns the list of properties to group by.
 *
 * @param collector         the session metric collector
 * @param group_by          the variable that will receive the list value if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.7
 */
bool diffusion_session_metric_collector_get_group_by_properties(
        DIFFUSION_SESSION_METRIC_COLLECTOR_T *collector,
        LIST_T **group_by);


/**
 * @ingroup PublicAPI_Metrics_Session_Metric_Collector
 *
 * @brief Compares two session metric collectors
 *
 * @param collector_1   a session metric collector
 * @param collector_2   another session metric collector
 *
 * @return true if both session metric collectors are identical.
 *
 * @since 6.7
 */
bool diffusion_session_metric_collector_compare(
        DIFFUSION_SESSION_METRIC_COLLECTOR_T *collector_1,
        DIFFUSION_SESSION_METRIC_COLLECTOR_T *collector_2);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector_Builder
 *
 * @brief Initialize a new topic metric collector builder.
 * <code>diffusion_session_topic_collector_builder_free</code> should be called on this pointer
 * when no longer needed.
 *
 * @return the topic metric collector builder
 *
 * @since 6.7
 */
DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *diffusion_topic_metric_collector_builder_init();


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector_Builder
 *
 * @brief Reset the topic metric collector builder's internal values.
 *
 * @param builder           the topic metric collector builder
 *
 * @return  the topic metric collector builder
 *
 * @since 6.7
 */
DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *diffusion_topic_metric_collector_builder_reset(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *builder);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector_Builder
 *
 * @brief Specifies whether the metric collector should export metrics to
 * Prometheus or not.
 *
 * The default is that metrics are not exported to Prometheus.
 *
 * @param builder                   the topic metric collector builder
 * @param export_to_prometheus      true to export metrics to Prometheus
 *
 * @return  the topic metric collector builder
 *
 * @since 6.7
 */
DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *diffusion_topic_metric_collector_builder_export_to_prometheus(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *builder,
        bool export_to_prometheus);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector_Builder
 *
 * @brief Set the maximum number of groups maintained by the metric collector.
 *
 * By default, the number of groups is not limited.
 *
 * @param builder           the session metric collector builder
 * @param limit             a positive integer
 *
 * @return  the topic metric collector builder
 *
 * @since 6.8
 */
DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *diffusion_topic_metric_collector_builder_maximum_groups(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *builder,
        int limit);

/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector_Builder
 *
 * @brief Specifies whether the metric collector should group by topic type.
 *
 * By default a topic metric collector does not group by topic type.
 *
 * @param builder               the topic metric collector builder
 * @param group_by_topic_type   true to indicate that the collector
 *                              should group by topic type
 *
 * @return  the topic metric collector builder
 *
 * @since 6.7
 */
DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *diffusion_topic_metric_collector_builder_group_by_topic_type(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *builder,
        bool group_by_topic_type);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector_Builder
 *
 * @brief Specifies whether the metric collector should group by topic view.
 *
 * By default a topic metric collector does not group by topic view.
 *
 * @param builder               the topic metric collector builder
 * @param group_by_topic_view   true to indicate that the collector
 *                              should group by topic view
 *
 * @return  the topic metric collector builder
 *
 * @since 6.9
 */
DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *diffusion_topic_metric_collector_builder_group_by_topic_view(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *builder,
        bool group_by_topic_view);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector_Builder
 *
 * @brief Specifies the number of leading parts of the topic path the
 *        metric collector should use to group results.
 *
 * By default a topic metric collector does not group by the topic
 * path prefix. If a positive number of parts is specified, it will enable grouping.
 *
 * @param builder               the topic metric collector builder
 * @param parts                 the number of leading parts of the topic path to
 *                              group by; set to 0 to disable grouping by path
 *
 * @return  the topic metric collector builder
 *
 * @since 6.8
 */
DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *diffusion_topic_metric_collector_builder_group_by_path_prefix_parts(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *builder,
        int parts);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector_Builder
 *
 * @brief Create a new topic metric collector using the values currently known by the builder.
 *
 * @param builder           the topic metric collector builder
 * @param name              the name of the topic metric collector
 * @param topic_selector    the selector pattern that specifies the topics for which
 *                          metrics are to be collected
 *
 * @return  a new topic metric collector if the operation was successful, NULL otherwise.
 *
 * @since 6.7
 *
 * @deprecated since 6.8 and will be removed in a future release
 *             use `diffusion_topic_metric_collector_builder_create` instead
 */
DEPRECATED(DIFFUSION_TOPIC_METRIC_COLLECTOR_T *diffusion_topic_metric_collector_builder_create_collector(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *builder,
        char *name,
        char *topic_selector))


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector_Builder
 *
 * @brief Create a new topic metric collector using the values currently known by the builder.
 *
 * @param builder           the topic metric collector builder
 * @param name              the name of the topic metric collector
 * @param topic_selector    the selector pattern that specifies the topics for which
 *                          metrics are to be collected
 * @param api_error         populated on API error. Can be NULL.
 *
 * @return  a new topic metric collector if the operation was successful, NULL otherwise.
 *          In this case, if a non-NULL <code>api_error</code> pointer has been provided,
 *          this will be populated with the error information and should be freed with
 *          <code>diffusion_api_error_free</code>.
 *
 * @since 6.7
 */
DIFFUSION_TOPIC_METRIC_COLLECTOR_T *diffusion_topic_metric_collector_builder_create(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *builder,
        char *name,
        char *topic_selector,
        DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector_Builder
 *
 * @brief Free a topic metric collector builder.
 *
 * @param builder           the topic metric collector builder
 *
 * @since 6.7
 */
void diffusion_topic_metric_collector_builder_free(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_BUILDER_T *builder);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector
 *
 * @brief Free a topic metric collector.
 *
 * @param collector         the topic metric collector
 *
 * @since 6.7
 */
void diffusion_topic_metric_collector_free(DIFFUSION_TOPIC_METRIC_COLLECTOR_T *collector);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector
 *
 * @brief Returns the topic selector of the topic metric collector.
 *
 * @param collector         the topic metric collector
 * @param topic_selector    the variable that will receive the topic selector if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.7
 */
bool diffusion_topic_metric_collector_get_topic_selector(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_T *collector,
        char **topic_selector);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector
 *
 * @brief Returns the name of the topic metric collector.
 *
 * @param collector         the topic metric collector
 * @param name              the variable that will receive the name if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.7
 */
bool diffusion_topic_metric_collector_get_name(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_T *collector,
        char **name);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector
 *
 * @brief Specifies whether the metric collector should group by topic type.
 *
 * @param collector             the topic metric collector
 * @param groups_by_topic_type  the variable that will receive the `bool` value if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.7
 */
bool diffusion_topic_metric_collector_groups_by_topic_type(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_T *collector,
        bool *groups_by_topic_type);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector
 *
 * @brief Indicates whether the metric collector groups by topic view.
 *
 * @param collector                 the topic metric collector
 * @param groups_by_topic_view      the variable that will receive the `bool` value if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.9
 */
bool diffusion_topic_metric_collector_groups_by_topic_view(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_T *collector,
        bool *groups_by_topic_view);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector
 *
 * @brief Specifies the number of leading parts of the topic path to group by,
 *        or 0, if the collector does not group by path prefix
 *
 * @param collector                     the topic metric collector
 * @param group_by_path_prefix_parts    the variable that will receive the `int` value if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.8
 */
bool diffusion_topic_metric_collector_group_by_path_prefix_parts(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_T *collector,
        int *group_by_path_prefix_parts);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector
 *
 * @brief Indicates whether the metric collector exports to Prometheus.
 *
 * @param collector                 the topic metric collector
 * @param exports_to_prometheus     the variable that will receive the `bool` value if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.7
 */
bool diffusion_topic_metric_collector_exports_to_prometheus(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_T *collector,
        bool *exports_to_prometheus);


/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector
 *
 * @brief Limit the number of groups maintained by this metric collector.
 *
 * Topic metric collectors can {@link diffusion_topic_metric_collector_groups_by_topic_type
 * group metrics by properties}.
 * This property places an upper limit on the number of groups that will be maintained for the
 * metric collector.
 *
 * @param collector         the topic metric collector
 * @param maximum_groups    the variable that will receive the `int` value if present
 *
 * @return true if the collector is valid and extracted the value
 *
 * @since 6.8
 */
bool diffusion_topic_metric_collector_maximum_groups(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_T *collector,
        int *maximum_groups);

/**
 * @ingroup PublicAPI_Metrics_Topic_Metric_Collector
 *
 * @brief Compares two topic metric collectors
 *
 * @param collector_1   a topic metric collector
 * @param collector_2   another topic metric collector
 *
 * @return true if both topic metric collectors are identical.
 *
 * @since 6.7
 */
bool diffusion_topic_metric_collector_compare(
        DIFFUSION_TOPIC_METRIC_COLLECTOR_T *collector_1,
        DIFFUSION_TOPIC_METRIC_COLLECTOR_T *collector_2);


/**
 * @ingroup PublicAPI_Metrics
 *
 * @brief callback when a session metric collector has been set on the server.
 *
 * @param context           User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*session_metric_collector_put_cb)(void *context);


/**
 * @ingroup PublicAPI_Metrics
 *
 * @brief callback when a session metric collector has been removed from the server.
 *
 * @param context           User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*session_metric_collector_remove_cb)(void *context);


/**
 * @ingroup PublicAPI_Metrics
 *
 * @brief callback when the list of session metric collectors has been retrieved from the server.
 *
 * @param context           User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*session_metric_collector_list_cb)(const LIST_T *session_metric_collectors, void *context);


/**
 * @ingroup PublicAPI_Metrics
 *
 * @brief callback when a topic metric collector has been set on the server.
 *
 * @param context           User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*topic_metric_collector_put_cb)(void *context);


/**
 * @ingroup PublicAPI_Metrics
 *
 * @brief callback when a topic metric collector has been removed from the server.
 *
 * @param context           User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*topic_metric_collector_remove_cb)(void *context);


/**
 * @ingroup PublicAPI_Metrics
 *
 * @brief callback when the list of topic metric collectors has been retrieved from the server.
 *
 * @param context           User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*topic_metric_collector_list_cb)(const LIST_T *topic_metric_collectors, void *context);


/**
 * @ingroup PublicAPI_Metrics
 *
 * Structure supplied when issuing a <code>diffusion_metrics_put_session_metric_collector</code> call.
 */
typedef struct diffusion_metrics_put_session_metric_collector_params_s {
        /// The session metric collector that will be placed in the server
        DIFFUSION_SESSION_METRIC_COLLECTOR_T *collector;
        /// Callback fired when the session metric collector is set
        session_metric_collector_put_cb on_collector_set;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_METRICS_PUT_SESSION_METRIC_COLLECTOR_PARAMS_T;


/**
 * @ingroup PublicAPI_Metrics
 *
 * Structure supplied when issuing a <code>diffusion_metrics_put_topic_metric_collector</code> call.
 */
typedef struct diffusion_metrics_put_topic_metric_collector_params_s {
        /// The topic metric collector that will be placed in the server
        DIFFUSION_TOPIC_METRIC_COLLECTOR_T *collector;
        /// Callback fired when the topic metric collector is set
        topic_metric_collector_put_cb on_collector_set;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_METRICS_PUT_TOPIC_METRIC_COLLECTOR_PARAMS_T;


/**
 * @ingroup PublicAPI_Metrics
 *
 * Structure supplied when issuing a <code>diffusion_metrics_remove_session_metric_collector</code> call.
 */
typedef struct diffusion_metrics_remove_session_metric_collector_params_s {
        /// The name of the session metric collector that will be removed from the server
        char *collector_name;
        /// Callback fired when the session metric collector is removed
        session_metric_collector_remove_cb on_collector_removed;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_METRICS_REMOVE_SESSION_METRIC_COLLECTOR_PARAMS_T;


/**
 * @ingroup PublicAPI_Metrics
 *
 * Structure supplied when issuing a <code>diffusion_metrics_remove_topic_metric_collector</code> call.
 */
typedef struct diffusion_metrics_remove_topic_metric_collector_params_s {
        /// The name of the topic metric collector that will be removed from the server
        char *collector_name;
        /// Callback fired when the topic metric collector is removed
        topic_metric_collector_remove_cb on_collector_removed;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_METRICS_REMOVE_TOPIC_METRIC_COLLECTOR_PARAMS_T;


/**
 * @ingroup PublicAPI_Metrics
 *
 * Structure supplied when issuing a <code>diffusion_metrics_list_session_metric_collectors</code> call.
 */
typedef struct diffusion_metrics_list_session_metric_collectors_params_s {
        /// Callback fired when the session metric collectors are retrieved from the server
        session_metric_collector_list_cb on_collectors_received;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_METRICS_LIST_SESSION_METRIC_COLLECTORS_PARAMS_T;


/**
 * @ingroup PublicAPI_Metrics
 *
 * Structure supplied when issuing a <code>diffusion_metrics_list_topic_metric_collectors</code> call.
 */
typedef struct diffusion_metrics_list_topic_metric_collectors_params_s {
        /// Callback fired when the topic metric collectors are retrieved from the server
        topic_metric_collector_list_cb on_collectors_received;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_METRICS_LIST_TOPIC_METRIC_COLLECTORS_PARAMS_T;


/**
 * @ingroup PublicAPI_Metrics
 *
 * @brief Add a session metric collector, replacing any with the same name.

 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing the session metric collector and
 *                      the callbacks to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL <code>api_error</code> pointer has been provided, this will be populated
 *         with the error information and should be freed with <code>diffusion_api_error_free</code>.
 */
bool diffusion_metrics_put_session_metric_collector(
        SESSION_T *session,
        const DIFFUSION_METRICS_PUT_SESSION_METRIC_COLLECTOR_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_Metrics
 *
 * @brief Retrieves the current session metric collectors.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing the callbacks to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL <code>api_error</code> pointer has been provided, this will be populated
 *         with the error information and should be freed with <code>diffusion_api_error_free</code>.
 */
bool diffusion_metrics_list_session_metric_collectors(
        SESSION_T *session,
        const DIFFUSION_METRICS_LIST_SESSION_METRIC_COLLECTORS_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_Metrics
 *
 * @brief Removes any session metric collector with the given name, if it exists.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing the session metric collector name and the
 *                      callbacks to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL <code>api_error</code> pointer has been provided, this will be populated
 *         with the error information and should be freed with <code>diffusion_api_error_free</code>.
 */
bool diffusion_metrics_remove_session_metric_collector(
        SESSION_T *session,
        const DIFFUSION_METRICS_REMOVE_SESSION_METRIC_COLLECTOR_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);



/**
 * @ingroup PublicAPI_Metrics
 *
 * @brief Add a topic metric collector, replacing any with the same name.

 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing the topic metric collector and
 *                      the callbacks to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL <code>api_error</code> pointer has been provided, this will be populated
 *         with the error information and should be freed with <code>diffusion_api_error_free</code>.
 */
bool diffusion_metrics_put_topic_metric_collector(
        SESSION_T *session,
        const DIFFUSION_METRICS_PUT_TOPIC_METRIC_COLLECTOR_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_Metrics
 *
 * @brief Retrieves the current topic metric collectors.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing the callbacks to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL <code>api_error</code> pointer has been provided, this will be populated
 *         with the error information and should be freed with <code>diffusion_api_error_free</code>.
 */
bool diffusion_metrics_list_topic_metric_collectors(
        SESSION_T *session,
        const DIFFUSION_METRICS_LIST_TOPIC_METRIC_COLLECTORS_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_Metrics
 *
 * @brief Removes any topic metric collector with the given name, if it exists.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing the topic metric collector name and the
 *                      callbacks to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL <code>api_error</code> pointer has been provided, this will be populated
 *         with the error information and should be freed with <code>diffusion_api_error_free</code>.
 */
bool diffusion_metrics_remove_topic_metric_collector(
        SESSION_T *session,
        const DIFFUSION_METRICS_REMOVE_TOPIC_METRIC_COLLECTOR_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);

#endif
