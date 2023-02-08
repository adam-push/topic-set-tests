#ifndef _diffusion_topic_specification_h_
#define _diffusion_topic_specification_h_ 1

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
 * @file topic-specification.h
 *
 * Functions used for working with topic specifications (TOPIC_SPECIFICATION_T).
 *
 * Topic specifications provide the information required to create a topic.
 * Topics can be created from a topic specification using `add_topic_from_specification`.
 *
 * Topic specifications allow an application to introspect the type and
 * capabilities of a topic. Topic specifications are provided to
 * <em>value streams</em> and <em>topic notification listeners</em>.
 *
 * A topic specification has a <em>topic type</em> and a map of
 * property settings which define the behavior of the topic. A default topic
 * specification for a topic type can be created using
 * `topic_specification_init`. Topic specifications with
 * different properties can be derived from a default instance using the
 * `topic_specification_init_with_properties` builder method.
 *
 * <h3>Topic Properties</h3>
 * 
 * Depending on the topic type, some properties must be included in the
 * specification when creating a topic and some properties have no effect. The
 * required and optional properties for each the topic type are set out in the
 * following table. Properties unsupported by the topic type are ignored.
 *
 * <table>
 *  <tr>
 *    <th></th>
 *    <th style="text-align:center;">Default when optional</th>
 *    <th></th>
 *    <th style="text-align:center;">STRING<br>
 *                                   JSON<br>
 *                                   BINARY<br>
 *    </th>
 *    <th style="text-align:center;">DOUBLE<br>
 *                                   INT64</th>
 *    <th style="text-align:center;">RECORD_V2</th>
 *    <th style="text-align:center;">TIME_SERIES</th>
 *    <th style="text-align:center;">ROUTING</th>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">COMPRESSION</th>
 *    <td style="text-align:center;">`low`</td>
 *    <td></td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">CONFLATION</th>
 *    <td style="text-align:center;">`conflate`</td>
 *    <td></td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional <sup>†</sup></td>
 *    <td style="text-align:center;">Optional</td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">DONT_RETAIN_VALUE</th>
 *    <td style="text-align:center;">`false`</td>
 *    <td></td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">— <sup>‡</sup></td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">OWNER</th>
 *    <td style="text-align:center;"></td>
 *    <td></td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">PERSISTENT</th>
 *    <td style="text-align:center;">`true`</td>
 *    <td></td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">PRIORITY</th>
 *    <td style="text-align:center;">`default`</td>
 *    <td></td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">PUBLISH_VALUES_ONLY</th>
 *    <td style="text-align:center;">`false`</td>
 *    <td></td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">— <sup>‡</sup></td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">REMOVAL</th>
 *    <td style="text-align:center;"></td>
 *    <td></td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">SCHEMA</th>
 *    <td style="text-align:center;"></td>
 *    <td></td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">—</td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">TIDY_ON_UNSUBSCRIBE</th>
 *    <td style="text-align:center;">`false`</td>
 *    <td></td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">TIME_SERIES_EVENT_VALUE_TYPE</th>
 *    <td style="text-align:center;"></td>
 *    <td></td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">Required</td>
 *    <td style="text-align:center;">—</td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">TIME_SERIES_RETAINED_RANGE</th>
 *    <td style="text-align:center;">`limit 10`</td>
 *    <td></td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">—</td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">TIME_SERIES_SUBSCRIPTION_RANGE</th>
 *    <td style="text-align:center;"><em>As documented</em></td>
 *    <td></td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">—</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">—</td>
 *  </tr>
 *  <tr>
 *    <th style="text-align:left;">VALIDATE_VALUES</th>
 *    <td style="text-align:center;">`false`</td>
 *    <td></td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">Optional</td>
 *    <td style="text-align:center;">— <sup>‡</sup>
 *  </tr>
 * </table>
 *
 * <dl>
 *  <dt>†
 *  <dd>`TIME_SERIES` topics have restricted values for the
 *      `CONFLATION` property. They are only allowed to have the values
 *      `off` or `unsubscribe`.
 *  <dt>‡
 *  <dd>`ROUTING` topics (deprecated) are references to other topics, and have no value
 *      of their own. Instead, they reflect the value of the appropriate source
 *      topic. Observed behavior depends on the values of the
 *      `DONT_RETAIN_VALUE`, `PUBLISH_VALUES_ONLY`, and
 *      `VALIDATE_VALUES` properties that are set on the source topic.
 * </dl>
 */


#include "hash.h"

/**
 * Opaque topic specification struct
 */
typedef struct TOPIC_SPECIFICATION_T TOPIC_SPECIFICATION_T;

/**
 * Key of the topic property that specifies whether a topic should publish
 * only values.
 *
 * By default, a topic that supports delta streams will publish the difference
 * between two values (a delta) when doing so is more efficient than to
 * publishing the complete new value. Subscribing sessions can use a value
 * stream to automatically apply the delta to a local copy of the topic value to
 * calculate the new value.
 *
 * Setting this proeprty to `true` disables this behavior so that deltas are
 * never published. Doing so is usually not recommended because it will result
 * in more data being transmitted, less efficient use of network resources, and
 * increased transmission latency. On the other hand, calculating deltas can
 * require significant CPU from the server or, if update streams are used, from
 * the updating client. The CPU cost will be higher if there are many
 * differences between successive values, in which case delta streams confer
 * fewer benefits. If successive values are unrelated to each other, consider
 * setting this property to `true`. Also consider setting this property to
 * `true` if the network capacity is high and the bandwidth savings of deltas
 * are not required.
 *
 * See also `DIFFUSION_DONT_RETAIN_VALUE`.
 */
extern const char *const DIFFUSION_PUBLISH_VALUES_ONLY;

/**
 * Key of the topic property that specifies a schema which constrains topic
 * values.
 *
 * This property is only used by `TOPIC_TYPE_RECORDV2` topics.
 */
extern const char *const DIFFUSION_SCHEMA;

/**
 * Key of the topic property that specifies the 'tidy on unsubscribe' option
 * for a topic.
 *
 * By default, if a session unsubscribes from a topic, it will receive any
 * updates for that topic that were previously queued but not sent.
 *
 * If this property is set to "true", when a session unsubscribes from the
 * topic, any updates for the topic that are still queued for the session
 * are removed. There is a performance overhead to using this option as the
 * client queue must be scanned to find topic updates to remove, however it
 * may prove useful for preventing unwanted data being sent to sessions.
 *
 * This property is `false` by default.
 */
extern const char *const DIFFUSION_TIDY_ON_UNSUBSCRIBE;

/**
 * Key of the topic property indicating whether a topic should validate
 * inbound values.
 *
 * By default, the server does not validate received values before sending
 * them on to client sessions. Invalid or corrupt values will be stored in
 * the topic and passed on to sessions. If this property is set to `true`,
 * the server will perform additional validation on values to check that
 * they are valid instances of the data type, and if it is not then it will
 * return an error to the updater and not update the topic.
 * 
 * If this value is not set (or set to something other than `true`), no
 * server validation of inbound values is performed. This is the recommended
 * setting as there is a performance overhead to validation and a session
 * using <em>topic update</em> cannot send invalid values anyway.
 *
 * This property is `false` by default.
 */
extern const char *const DIFFUSION_VALIDATE_VALUES;

/**
 * Key of the topic property that specifies a topic should not retain its
 * last value.
 *
 * By default, a topic (other than a `ROUTING` topic) will retain
 * its latest value. The latest value will be sent to new subscribers. Setting
 * this property to `true` disables this behavior. New subscribers will not be
 * sent an initial value. No value will be returned for fetch operations that
 * select the topic. This is useful for data streams where the values are only
 * transiently valid.
 *
 * Setting this property to `true` also disables delta streams, regardless
 * of the `DIFFUSION_PUBLISH_VALUES_ONLY` value. If subsequent values are likely
 * to be related, delta streams usually provide performance benefits (see
 * `DIFFUSION_PUBLISH_VALUES_ONLY`). Consider leaving this property set to
 * `false` to benefit from delta streams, even if there is no other requirement
 * to retain the last value.
 *
 * Bearing in mind the performance trade-offs of disabling delta streams, there
 * are two reasons to consider setting this property to `true`. First, it stops
 * the server and each subscribed client from keeping a copy of the value,
 * reducing their memory requirements. Second, when a topic has a high update
 * rate and is replicated across a cluster, it can significantly improve
 * throughput because the values need not be persisted to the cluster.

 * Time series topics ignore this property and always retain the latest
 * value.
 */
extern const char *const DIFFUSION_DONT_RETAIN_VALUE;

/**
 * Key of the topic property that can be used to prevent a topic from being
 * persisted when the server is configured to enable persistence.
 *
 * By default, a topic will be persisted if persistence is enabled at the
 * server and the topic type supports persistence
 * 
 * Setting `PERSISTENT` to `false` will prevent the topic from being persisted.
 * This property is `true` by default.
 */
extern const char *const DIFFUSION_PERSISTENT;

/**
 * Key of the topic property that specifies a removal policy for automatic
 * removal of the topic (and/or other topics).
 * 
 * This property is specified as an expression which defines one or more
 * conditions that are to be satisfied before automatic removal occurs.
 * 
 * The expression takes the form:
 * 
 * <code>
 *  when conditions [remove "selector"]
 * </code>
 * 
 * At least one condition must be supplied. If more than one is supplied,
 * they must be separated by logical operators (`and` or `or`).
 * The natural evaluation order of the operators may be changed by
 * surrounding with parentheses (e.g. (<em>condition</em> `and`
 * <em>condition</em>)).
 * 
 * The `remove` clause is optional. It provides a `Topic Selector`
 * expression representing the topics to be removed.
 * If a `remove` clause is specified, the topic with the removal
 * policy will only be removed if its path matches the selector expression.
 * The selector must be surrounded by either double or single quotes.
 * 
 * When many topics have the same removal policy, it is better to
 * set this property for one of them, using a `remove`
 * clause that selects all of the topics. This is more efficient because it
 * allows the server to avoid evaluating the same condition many times.
 * 
 * The permissions that are applied at the time of removal are those defined
 * by the roles of the principal that created the topic at the time of
 * creation. The roles of that principal may therefore change before the
 * removal with no effect, but if the permissions given to the roles change
 * it may have an effect upon the final removal.
 * 
 * Only one occurrence of each of the following condition types may be
 * included within the expression:
 * <table>
 *  <tr>
 *    <th style="text-align:left;">Condition&nbsp;Type</th>
 *    <th style="text-align:left;">Format</th>
 *    <th style="text-align:left;">Usage</th>
 *  </tr>
 *  <tr valign="top">
 *    <th style="text-align:left;"><b>time&nbsp;after</b></th>
 *    <td><code>time&nbsp;after&nbsp;<i>absoluteTime</i></code></td>
 *    <td>Removal should occur after a specified absolute time. Absolute time
 *        may be specified as a number of milliseconds since the epoch (00:00:00 on
 *        1 January 1970) <b>or</b> as a quoted date and time formatted in <a href=
 *        "https://docs.oracle.com/javase/8/docs/api/java/time/format/DateTimeFormatter.html#RFC_1123_DATE_TIME">RFC_1123
 *        date time format</a>. Either single or double quotes may be used.</td>
 *  </tr>
 *  <tr valign="top">
 *    <th style="text-align:left;"><b>subscriptions&nbsp;less&nbsp;than</b></th>
 *    <td><code>[local]&nbsp;subscriptions&nbsp;&lt;&nbsp;<i>n</i>&nbsp;for&nbsp;<i>forPeriod</i>&nbsp;[after&nbsp;<i>afterPeriod</i>]</code></td>
 *    <td>Removal should occur when the topic has had less than the specified
 *        number (<i>n</i>) of subscriptions for a given period (<i>forPeriod</i>)
 *        of time. Optionally, an initial period (<i>afterPeriod</i>) may be
 *        specified by which to delay the initial checking of this condition. See
 *        below for period formats.
 * 
 *        The optional <code>local</code> keyword restricts evaluation to only count
 *        subscriptions from sessions belonging to the local server or cluster,
 *        ignoring subscriptions from sessions belonging to downstream remote servers that
 *        host fanout replicas of the topic.</td>
 *  </tr>
 *  <tr valign="top">
 *    <th style="text-align:left;"><b>no&nbsp;updates&nbsp;for</b></th>
 *    <td><code>no&nbsp;updates&nbsp;for&nbsp;<i>forPeriod</i>&nbsp;[after&nbsp;<i>afterPeriod</i>]</code></td>
 *    <td>Removal should occur when the topic has had no updates for a given
 *        period (<i>forPeriod</i>) of time. Optionally, an initial period
 *        (<i>afterPeriod</i>) may be specified by which to delay the initial
 *        checking of this condition. See below for period formats.</td>
 *  </tr>
 * </table>
 * 
 * Multiple occurrences of the following condition types may be included
 * within the expression:
 * <table>
 *  <tr>
 *    <th style="text-align:left;">Condition&nbsp;Type</th>
 *    <th style="text-align:left;">Format</th>
 *    <th style="text-align:left;">Usage</th>
 *  </tr>
 *  <tr valign="top">
 *    <th style="text-align:left;"><b>no&nbsp;session&nbsp;has</b></th>
 *    <td><code>no&nbsp;[local]&nbsp;session&nbsp;has&nbsp;<i>"criteria"</i>&nbsp;[for&nbsp;<i>forPeriod</i>]&nbsp;[after&nbsp;<i>afterPeriod</i>]</code></td>
 *    <td>Removal should occur when there are no sessions satisfying certain
 *        <i>criteria</i>. Optionally the criteria can be required to be satisfied
 *        for a period of time (<i>forPeriod</i>). Optionally, an initial period
 *        (<i>afterPeriod</i>) can be specified to delay the initial check of the
 *        criteria. Session selection criteria are specified as defined in
 *        `session filters` and must be surrounded by single or
 *        double quotes. See below for period formats.
 *        
 *        The optional <code>local</code> keyword restricts evaluation to sessions
 *        belonging to the local server or cluster, ignoring sessions belonging to
 *        downstream remote servers that host fanout replicas of the topic.</td>
 *  </tr>
 *  <tr valign="top">
 *    <th></th>
 *    <td><code>this&nbsp;session&nbsp;closes</code></td>
 *    <td>This is a shorthand form of `no local session has` that may be
 *        used to indicate that the topic is to be removed when the session that
 *        created it closes.</td>
 *  </tr>
 * </table>
 *
 * Time periods are specified as a number followed (with no intermediate
 * space) by a single letter representing the time unit. The time unit may
 * be `s` (seconds), `m` (minutes), `h` (hours) or
 * `d` (days). For example, 10 minutes would be specified as
 * `10m`.
 * 
 * If quotes or backslashes (`\`) are required within quoted values
 * such as selectors or session criteria then they may be escaped by
 * preceding with `\`. The convenience method
 * `diffusion_escape()` is provided to escape such characters in
 * a value. The expression is validated only by the server and therefore if
 * an invalid expression is specified it will be reported as an
 * `DIFF_ERR_INVALID_TOPIC_SPECIFICATION`.
 * 
 * <b>Examples:</b>
 * 
 * <code>
 *  when time after 1518780068112
 * </code><br>
 *
 * The topic will be removed when the date and time indicated by the
 * specified number of milliseconds since the epoch has passed.
 * 
 * <code>
 *  when time after "Tue, 3 Jun 2018 11:05:30 GMT"
 * </code><br>
 * 
 * The topic will be removed when the specified date and time has passed.
 * 
 * <code>
 *  when time after "Tue, 3 Jun 2018 11:05:30 GMT" remove "*alpha/beta//"
 * </code><br>
 * 
 * The topic alpha/beta and all topics subordinate to it will be removed
 * when the specified date and time has passed.
 * 
 * <code>
 *  when subscriptions &lt; 1 for 20m
 * </code><br>
 * 
 * The topic will be removed when it has had no subscriptions for a
 * continuous period of 20 minutes.
 * 
 * <code>
 *  when subscriptions &lt; 2 for 20m after 1h
 * </code><br>
 * 
 * The topic will be removed when it has had less than 2 subscriptions for a
 * continuous period of 20 minutes after one hour has passed since its
 * creation.
 * 
 * <code>
 *  when no updates for 3h
 * </code><br>
 * 
 * The topic will be removed when it has had no updates for a continuous
 * period of 3 hours.
 * 
 * <code>
 *  when no updates for 15m after 1d
 * </code><br>
 * 
 * The topic will be removed when it has had no updates for a continuous
 * period of 15 minutes after one day has passed since its creation.
 * 
 * <code>
 *  when this session closes
 * </code><br>
 * 
 * The topic will be removed when the session creating it closes.
 * 
 * <code>
 *  when no session has '$Principal is "Alice"'
 * </code><br>
 * 
 * The topic will be removed when there are no sessions with the principal
 * 'Alice'.
 * 
 * <code>
 *  when no session has '$Principal is "Alice"' for 10m
 * </code><br>
 * 
 * The topic will be removed when there are no sessions with the principal
 * 'Alice' for a continuous period of 10 minutes.
 * 
 * <code>
 *  when no session has 'Department is "Accounts"' for 30m after 2h
 * </code><br>
 * 
 * The topic will be removed when there have been no sessions from the
 * Accounts department for a continuous period of 30 minutes after 2 hours
 * have passed since its creation.
 * 
 * <code>
 *  when time after "Tue, 3 Jun 2018 11:05:30 GMT" and subscriptions &lt; 1 for 30m
 * </code><br>
 * 
 * The topic will be removed when the specified date and time has passed and
 * the topic has had no subscriptions for a continuous period of 30 minutes
 * after that time.
 * 
 * <code>
 *  when time after "Tue, 3 Jun 2018 11:05:30 GMT" and subscriptions &lt; 2 for 10m after 1h
 * </code><br>
 * 
 * The topic will be removed when the specified date and time has passed and
 * the topic has had less than 2 subscriptions for a continuous period of 10
 * minutes after that time plus one hour.
 * 
 * <code>
 *  when time after "Tue, 3 Jun 2018 11:05:30 GMT" or subscriptions &lt; 2 for 10m after 1h
 * </code><br>
 * 
 * The topic will be removed when the specified date and time has passed or
 * the topic has had less than 2 subscriptions for a continuous period of 10
 * minutes after one hour from its creation.
 * 
 * <code>
 *  when time after "Tue, 3 Jun 2018 11:05:30 GMT" and (subscriptions &lt; 2 for 10m after 1h or no updates for 20m)
 * </code><br>
 * 
 * The topic will be removed when the specified date and time has passed and
 * either the topic has had less than 2 subscriptions for a continuous
 * period of 10 minutes after that time plus one hour or it has had no
 * updates for a continuous period of 20 minutes. Note that the parentheses
 * are significant here as without them the topic would be removed if it had
 * had no updates for 20 minutes regardless of the time and subscriptions
 * clause.
 * 
 * <b>Notes and restrictions on use</b>
 * 
 * The `after` time periods refer to the period since the topic was
 * created or restored from persistence store after a server is restarted.
 * They are designed as a 'grace' period after the topic comes into
 * existence before the related condition starts to be evaluated. When not
 * specified the conditions start to be evaluated as soon as the topic is
 * created or restored.
 * 
 * The server will evaluate conditions on a periodic basis (every few
 * seconds) so the exact removal time will not be precise for low periodic
 * granularity.
 * 
 * The meaning of the `for` period in a `no session has`
 * condition is subtly different from its use in other conditions. It does
 * not guarantee that there has been no session satisfying the condition at
 * some point between evaluations, only that when evaluated the given period
 * of time has passed since it was last evaluated and found to have no
 * matching sessions.
 * 
 * Subscriptions is the number of subscriptions to a topic, including those
 * that occur through routing topics.
 * 
 * Automatic topic removal is supported for a topic that is replicated
 * across the local cluster, and for a topic with with fanout replicas on
 * downstream remote servers. A `subscriptions less than` condition
 * will be evaluated against the total number of subscriptions across the
 * cluster and on all fanout replicas on downstream remote servers. A
 * `no session has` condition will consider all sessions hosted across
 * the cluster and all sessions hosted by downstream remote servers that
 * have a fanout replica of the topic. The `local` keyword can be used
 * to restrict evaluation to the local cluster, ignoring fanout replicas.
 */
extern const char *const DIFFUSION_REMOVAL;

/**
 * Key of the topic property that allows the creator of a topic to extend
 * `READ_TOPIC`, `MODIFY_TOPIC`, and `UPDATE_TOPIC` permissions to a specific
 * principal, in addition to the permissions granted by the authorisation
 * rules in the security store.
 *
 * A session that has authenticated using the principal can update and
 * remove the topic, so the principal can be considered the topic owner. To
 * fetch or subscribe to the topic, the principal must also be granted
 * the `SELECT_TOPIC` permission by the security store rules.
 *
 * This may be used in the following cases:
 * 1) A session creates a topic and makes its own principal the owner.
 * 2) A session creates a topic and makes another principal the owner.
 *
 * The format of the property value is:
 *
 * `$Principal is "name"`
 *
 * where 'name' is the name of the principal. Single quotes may be used
 * instead of double quotes.
 *
 * The purpose of this property is to allow a client to create topics on
 * behalf of other users. This can be used in conjunction with the
 * `DIFFUSION_REMOVAL` property so that such topics are removed when there are
 * no longer any sessions for the named principal.
 */
extern const char *const DIFFUSION_OWNER;

/**
 * Key of the topic property that describes the conflation policy of the
 * topic. The policy specifies how the server manages queued topic updates.
 * Conflation is applied individually to each session queue.
 *
 * Conflation is the process of merging or discarding topic updates queued
 * for a session to reduce the server memory footprint and network data. The
 * server will conflate sessions that have a large number of queued messages
 * to meet configured queue size targets. The sessions with the largest
 * queues are typically slow consumers or have been disconnected – both will
 * benefit from conflation. This property allows conflation behaviour to be
 * tuned on a topic-by-topic basis.
 *
 * Supported policies are:
 *
 * - `off`
 * - `conflate`
 * - `unsubscribe`
 * - `always`
 *
 * The default policy used when the property is not specified and the topic
 * type is not time series is `conflate`. The default policy used when the
 * property is not specified and the topic type is time series is `off`.
 *
 * The policy `off` disables conflation for the topic. This policy disables
 * all conflation for the topic, so topic updates will never be merged or
 * discarded.
 *
 * The policy `conflate` automatically conflates topic updates when back
 * pressure is detected by the server.
 *
 * The policy `unsubscribe` automatically unsubscribes the topic when back
 * pressure is detected by the server. The unsubscription is not persisted
 * to the cluster, if a session fails over to a different server it will be
 * resubscribed to the topic.
 *
 * The policy `always` automatically conflates topic updates as they are
 * queued for the session. This is an eager policy that ensures only the
 * latest update is queued for the topic, minimising the server memory and
 * network bandwidth used by the session.
 *
 * The `conflate` and `unsubscribe` policies are applied when the server
 * detects back pressure for a session. The server configuration places
 * limits on the data queued for each session. If these limits are breached,
 * the server will conflate the session queue to attempt to reduce its size.
 * If the session queue still exceeds the limits after conflation, the
 * session will be terminated.
 *
 * Conflation of stateless, single value and record topics is configured
 * using server-side configuration. This configuration describes how topic
 * updates should be merged. Like the `always` policy conflation specified
 * this way is applied when queuing a topic update. The policy `off`
 * prevents this conflation being applied. All other policies allow
 * conflation specified this way to happen. The `unsubscribe` policy will
 * still unsubscribe topics that use conflation specified this way.
 *
 * The policies `conflate` and `always` are not supported for
 * time series topics as they would cause missing events. Attempts to enable
 * these policies with time series topics will cause the creation of the
 * topic to fail, reporting that the specification is invalid.
 */
extern const char *const DIFFUSION_CONFLATION;

/**
 * Key of the topic property that allows the compression policy to be set
 * on a per-topic basis.
 *
 * Compression reduces the bandwidth required to broadcast topic updates to
 * subscribed sessions, at the cost of increased server CPU.
 *
 * Changes to a topic's value are published to each subscribed session as a
 * sequence of topic messages. A topic message can carry the latest value or
 * the difference between the latest value and the previous value (a delta).
 * The compression policy determines if and how published topic messages
 * are compressed. Topic messages are not exposed through the client API;
 * the client library handles decompression and decodes deltas
 * automatically, passing reconstructed values to the application.
 *
 * The compression policy for a topic is specified by setting this property
 * to one of several values:
 * <ul>
 *  <li>`off`</li>
 *  <li>`low`</li>
 *  <li>`medium`</li>
 *  <li>`high`</li>
 * </ul>
 *
 * The policies are listed in the order of increasing compression and
 * increasing CPU cost. `off` disables compression completely for the
 * topic and requires no additional CPU; `high` compresses the topic
 * messages to the smallest number of bytes, but has the highest CPU cost.
 * Generally some compression is beneficial, so the default value for this
 * property is `low.
 *
 * Prior to version 6.4, only two values were allowed: `true`
 * (equivalent to `medium`, and the previous default policy) and
 * `false` (equivalent to `off`). These values are still
 * supported.
 *
 * This property is only one factor that determines whether a topic message
 * will be compressed. Other factors include:
 * <ul>
 *  <li>Compression must be enabled in the server configuration.
 *  <li>The client library must support the server's compression scheme. In
 *      this release, the server supports zlib compression, and also allows
 *      compression to be disabled on a per-connector basis. From 6.4, all client
 *      libraries are capable of zlib compression. A JavaScript client may or may
 *      not support zlib compression, depending on whether the zlib library can
 *      be loaded. The zlib library is packaged separately to reduce the download
 *      size of the core library.
 * </ul>
 */
extern const char *const DIFFUSION_COMPRESSION;

/**
 * Key of the topic property that specifies the event data type for a time
 * series topic.
 *
 * The value is the `DIFFUSION_DATATYPE` name of a data type.
 */
extern const char *const DIFFUSION_TIME_SERIES_EVENT_VALUE_TYPE;

/**
 * Key of the topic property that specifies the range of events retained by
 * a time series topic.
 *
 * When a new event is added to the time series, older events that fall
 * outside of the range are discarded.
 *
 * If the property is not specified, a time series topic will retain the ten
 * most recent events.
 *
 * Time series range expressions
 *
 * The property value is a time series `range expression` string
 * composed of one or more constraint clauses. Constraints are combined to
 * provide a range of events from the end of the time series.
 *
 * <dl>
 *  <dt>limit constraint</dt>
 *  <dd>A limit constraint specifies the maximum number of events from the
 *      end of the time series.
 *  <dt>last clause
 *  <dd>A last constraint specifies the maximum duration of events from the
 *      end of the time series. The duration is expressed as an integer followed
 *      by one of the following time units:
 *      - `MS` - milliseconds;
 *      - `S` - seconds;
 *      - `H` - hours.
 *      If a range expression contains multiple constraints, the constraint that
 *      selects the smallest range is used.
 * </dl>
 *
 * <b>Examples:</b>
 * <table>
 *  <tr>
 *    <td>`limit 5`</td>
 *    <td>The five most recent events</td>
 *  </tr>
 *  <tr>
 *    <td>`last 10s`</td>
 *    <td>All events that are no more than ten seconds older than the latest
 *        event</td>
 *  </tr>
 *  <tr>
 *    <td>`last 10s limit 5`</td>
 *    <td>The five most recent events that are no more than ten seconds older
 *      than the latest event</td>
 *  </tr>
 * </table>
 * 
 * Range expressions are not case sensitive: `limit 5 last 10s` is
 * equivalent to `LIMIT 5 LAST 10S`.
 */
extern const char *const DIFFUSION_TIME_SERIES_RETAINED_RANGE;

/**
 * Key of the topic property that specifies the range of time series topic
 * events to send to new subscribers.
 *
 * The property value is a time series range expression, following the
 * format used for `DIFFUSION_TIME_SERIES_RETAINED_RANGE`.
 *
 * If the property is not specified, new subscribers will be sent the latest
 * event if delta streams are enabled and no events if delta streams are
 * disabled. See the description of Subscription range in the
 * time series feature documentation.
 */
extern const char *const DIFFUSION_TIME_SERIES_SUBSCRIPTION_RANGE;

/**
 * Key of the topic property that specifies the topic delivery priority.
 *
 * The supported delivery priorities are:
 * - `low`
 * - `default`
 * - `high`
 *
 * The delivery priority affects the order of topic updates sent to a
 * subscribed client session. When there are multiple topic updates for
 * topics with different priorities in a session's outbound queue, updates
 * for `high` priority topics will be delivered first, followed by
 * updates for `default` priority topics, followed by updates for
 * `low` priority topics. Topic subscription and unsubscription
 * notifications are also delivered according to the topic delivery
 * priority.
 *
 * Using different delivery priorities is most beneficial when there is a
 * large backlog of queued updates to deliver to a client session. On
 * lightly loaded systems, updates typically remain in the outbound queue
 * for a few milliseconds and so there is a lower chance of topic updates
 * being reordered based on their priority. The backlog will be larger if
 * the topic update rate is higher; the server or the client are more
 * heavily loaded; the client session becomes temporarily disconnected; or
 * if there is poor network connectivity between the server and the client.
 *
 * Messages from the server to the client that are not topic updates, for
 * example ping requests and responses, are queued with the
 * `default` delivery priority.
 */
extern const char *const DIFFUSION_PRIORITY;

/**
 * The topic type determines the type of the data values a topic publishes to
 * subscribers.
 *
 * <h3>Source Topics</h3>
 * Most topics are source topics. The characteristics of each type of source
 * topic are summarized in the following table.
 *
 * | Topic type              | State                         | Data type                                         |
 * | ------------------------| ----------------------------- | ------------------------------------------------- |
 * | #TOPIC_TYPE_STRING      | Single scalar value.          | String.                                           |
 * | #TOPIC_TYPE_INT64       | Single scalar value.          | 64-bit integer.                                   |
 * | #TOPIC_TYPE_DOUBLE      | Single scalar value.          | Double precision floating point number.           |
 * | #TOPIC_TYPE_BINARY      | Single scalar value.          | Arbitrary binary data.                            |
 * | #TOPIC_TYPE_RECORDV2    | Single composite value.       | #DATATYPE_RECORDV2 &ndash; Diffusion-specific data type. A list of records, each composed of field values, with an optional schema. |
 * | #TOPIC_TYPE_JSON        | Single composite value.       | #DATATYPE_JSON, backed by CBOR-format binary.   |
 * | #TOPIC_TYPE_TIME_SERIES | Append-only log of events.    | #DIFFUSION_TIME_SERIES_EVENT_T containing a value of a \link DIFFUSION_DATATYPE well-known data type\endlink. |
 *
 *
 * ###Routing Topics (deprecated)
 *
 * A {@link TOPIC_TYPE_ROUTING ROUTING} topic can have a different source topic for each
 * subscription.
 * 
 * Routing topics republish values from source topics. The data type is
 * inherited from the source topic.
 * 
 * Routing topics are deprecated in favor of {@link session-trees.h Session Trees}.
 */
typedef enum {
        /**
         * Routing Topic.
         *
         * A topic that can reference different source topics for different sessions.
         *
         * Each subscription to a routing topic is routed to a source topic. Updates
         * to the source topic are routed back to the subscriber and appear to come
         * from the routing topic.
         *
         * The result is that a session may subscribe to a topic which is in reality
         * supported by another topic and the mapping of the routing topic to the
         * actual topic can be different for each session.
         *
         * As an example, you may wish for all sessions to simply subscribe to a
         * topic called "Prices" but depending upon the client type the actual topic
         * could differ (Prices/Discount, Prices/Standard etc).
         *
         * An instance of this topic may map any number of sessions to any number of
         * different source topics.
         *
         * From the point of view of a session subscribing to such a topic, a
         * routing topic appears to be a normal topic but it has no state of its own
         * and cannot be updated.
         *
         * The mapping of sessions to source topics is performed by a control client
         * session using the Subscription Control feature. When a session subscribes
         * to the routing topic the control client is requested to provide the topic
         * that the client is to be subscribed to. If there is no control client
         * available to handle subscriptions at the time a session subscribed, the
         * session will not be subscribed to the topic.
         * 
         * Alternatively, the routing can be determined by a user-written Java class
         * (deployed on the server) which will be invoked to define the mapping of
         * the topic to another data topic when a session subscribes.
         *
         * When a source topic is removed that is mapped to a routing topic then
         * any session that were mapped to that source topic will be unsubscribed from
         * the routing topic.
         *
         * @since 5.7
         *
         * @deprecated since 6.7 Routing topics are deprecated. The more powerful
         * {@link session-trees.h Session Trees} feature should be used in their place.
         */
        TOPIC_TYPE_ROUTING = 12,
        
        /**
         * Topic that stores and publishes binary values. Based on the
         * {@link DATATYPE_BINARY BINARY} data type.
         *
         * Supports delta-streams.
         * 
         * @since 5.7
         */
        TOPIC_TYPE_BINARY = 14,
        
        /**
         * Topic that stores and publishes JSON (JavaScript Object Notation) values.
         * Based on the {@link DATATYPE_JSON JSON} data type.
         *
         * Supports delta-streams.
         *
         * @since 5.7
         */
        TOPIC_TYPE_JSON = 15,
        
        /**
         * A time series is a sequence of events. Each event contains a value and has
         * server-assigned metadata comprised of a sequence number, timestamp, and
         * author.
         *
         * A time series topic allows sessions to access a time series that is
         * maintained by the server. A time series topic has an associated event data
         * type that determines the type of value associated with each event.
         *
         * <h3>Retained range</h3>
         *
         * The {@link #DIFFUSION_TIME_SERIES_RETAINED_RANGE} property
         * configures the range of historic events retained by a time series topic.
         * If the property is not specified, a time series topic will retain the ten
         * most recent events.
         *
         * <h3>Subscription range</h3>
         *
         * The {@link #DIFFUSION_TIME_SERIES_SUBSCRIPTION_RANGE} property
         * configures a time series topic to send a range of historic events from the
         * end of the time series to new subscribers. This is a convenient way to
         * synchronize new subscribers without requiring the use of a range query.
         *
         * By default, new subscribers will be sent the latest event if delta streams
         * are enabled and no events if delta streams are disabled.
         *
         * <h3>Mandatory properties</h3>
         *
         * The {@link #DIFFUSION_TIME_SERIES_EVENT_VALUE_TYPE} property must
         * be provided when creating a time series topic.
         *
         * @since 6.0
         */
        TOPIC_TYPE_TIME_SERIES = 16,
        
        /**
         * Topic that stores and publishes string values. Based on the string data
         * type.
         *
         * Supports `nil` values.
         *
         * Supports delta-streams.
         *
         * @since 6.0
         */
        TOPIC_TYPE_STRING = 17,
        
        /**
         * Topic that stores and publishes 64-bit integer values. Based on the int64
         * data type.
         *
         * Supports `nil` values.
         *
         * The topic does not support delta-streams — only complete values are
         * transmitted.
         * 
         * @since 6.0
         */
        TOPIC_TYPE_INT64 = 18,
        
        /**
         * Topic that stores and publishes IEEE 754 double-precision floating point
         * numbers. Based on the double data type.
         *
         * Supports `nil` values.
         *
         * The topic does not support delta-streams — only complete values are
         * transmitted.
         *
         * @since 6.0
         */
        TOPIC_TYPE_DOUBLE = 19,
        
        /**
         * Topic that stores and publishes data in the form of records and fields.
         *
         * Supports delta-streams.
         *
         * @since 6.0
         */
        TOPIC_TYPE_RECORDV2 = 20,
        
        /**
         * A topic type that is unsupported by the session.
         *
         * @since 6.1
         */
        TOPIC_TYPE_UNKNOWN = 21
} TOPIC_TYPE_T;

/**
 * @brief Default constructor. Creates a topic specification of a particular topic type with no properties.
 *        `topic_specification_free` should be called on the pointer when no longer needed.
 *
 * @param topic_type the topic type
 *
 * @return a topic specification initialised with the provided topic type.
 */
TOPIC_SPECIFICATION_T *topic_specification_init(TOPIC_TYPE_T topic_type);

/**
 * @brief Creates a topic specification of a particular topic type and properties. `topic_specification_free` should
 *        be called on the pointer when no longer needed.
 *
 * @param topic_type the topic type
 * @param properties the specification properties
 *
 * @return a topic specification initialised with the provided topic type and
 *         properties.
 */
TOPIC_SPECIFICATION_T *topic_specification_init_with_properties(TOPIC_TYPE_T topic_type, HASH_T *properties);

/**
 * @brief Returns the `TOPIC_TYPE_T` assigned to the specification.
 *
 * @param specification the topic specification
 *
 * @return the topic type of the topic specification. -1 is returned if
 *         the specification is NULL.
 */
TOPIC_TYPE_T topic_specification_get_topic_type(const TOPIC_SPECIFICATION_T *specification);

/**
 * @brief Returns a copy `HASH_T` of the properties assigned to the specification. `hash_free`
 *        should be called on the pointer when no longer needed.
 *
 * @param specification the topic specification
 *
 * @return a copy of the topic specification's properties. NULL is returned if
 *         the specification or the specification's properties hash map is NULL.
 */
HASH_T *topic_specification_get_properties(const TOPIC_SPECIFICATION_T *specification);

/**
 * @brief Set the topic type for a topic specification.
 *
 * @param specification the topic specification
 * @param topic_type    the topic type to set the specification to
 */
void topic_specification_set_topic_type(TOPIC_SPECIFICATION_T *specification, TOPIC_TYPE_T topic_type);

/**
 * @brief Set the properties for a topic specification.
 *
 * @param specification the topic specification
 * @param properties    the properties to set the topic specification to
 */
void topic_specification_set_properties(TOPIC_SPECIFICATION_T *specification, const HASH_T *properties);

/**
 * @brief Create a duplicate (copy) of an existing `TOPIC_SPECIFICATION_T`.
 *        `topic_specification_free` should be called on the pointer when no
 *        longer needed.
 *
 * @param src the topic specification to copy
 *
 * @return a copy of the topic specification provided.
 */
TOPIC_SPECIFICATION_T *topic_specification_dup(const TOPIC_SPECIFICATION_T *src);

/**
 * @brief Free memory associated with a `TOPIC_SPECIFICATION_T`.
 *
 * @param specification the topic specification to be freed.
 */
void topic_specification_free(TOPIC_SPECIFICATION_T *specification);

#endif
