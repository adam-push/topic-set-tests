#ifndef _diffusion_fetch_request_h_
#define _diffusion_fetch_request_h_ 1

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
 * @file fetch-request.h
 *
 * A fetch request.
 *
 * This defines a fetch request to be made to the server. A new request can
 * be created using the `diffusion_fetch_request_init` function and
 * modified to specify a range of topics and/or various levels of detail.
 */

#include <stdlib.h>
#include <stdbool.h>

#include "session.h"
#include "set.h"
#include "list.h"
#include "diffusion-api-error.h"
#include "datatypes.h"
#include "topic-specification.h"

/**
 * An opaque fetch request struct.
 */
typedef struct DIFFUSION_FETCH_REQUEST_T DIFFUSION_FETCH_REQUEST_T;

/**
 * An opaque fetch result struct.
 */
typedef struct DIFFUSION_FETCH_RESULT_T DIFFUSION_FETCH_RESULT_T;

/**
 * An opaque topic result struct.
 */
typedef struct DIFFUSION_TOPIC_RESULT_T DIFFUSION_TOPIC_RESULT_T;

/**
 * @brief Returns a results `LIST_T` of `DIFFUSION_TOPIC_RESULT_T`s from the fetch
 * operation.
 *
 * The `LIST_T` returned from this function is a copy, and as such should be freed with
 * `list_free(topic_results_list, (void (*)(void *))diffusion_topic_result_free)`.
 *
 * Results are always returned in path order.
 *
 * @param fetch_result Fetch result to retrieve the topic results from
 *
 * @return a list of topic results, each representing a result single
 *         topic selected by the fetch operation. NULL, if the fetch
 *         result is NULL.
 */
LIST_T *diffusion_fetch_result_get_topic_results(const DIFFUSION_FETCH_RESULT_T *fetch_result);

/**
 * @brief Indicates whether the fetch could have returned more results if it
 * had not been constrained by the `diffusion_fetch_request_first` or
 * `diffusion_fetch_request_last` limits.
 *
 * @param fetch_result Fetch result to query
 *
 * @return true if a limit was specified and more results could have
 *         been returned, otherwise false
 */
bool diffusion_fetch_result_has_more(const DIFFUSION_FETCH_RESULT_T *fetch_result);

/**
 * @brief The number of elements in the fetch result.
 *
 * @param fetch_result Fetch result to query
 *
 * @return the size of the fetch result list. -1 is returned if the fetch result is `NULL`.
 */
int diffusion_fetch_result_size(const DIFFUSION_FETCH_RESULT_T *fetch_result);

/**
 * @brief Returns `1` (true) if the result contains zero elements.
 *
 * @param fetch_result Fetch result to query
 *
 * @return 1 if fetch result list is empty, 0 otherwise. -1 is returned if the fetch result is `NULL`.
 */
int diffusion_fetch_result_is_empty(const DIFFUSION_FETCH_RESULT_T *fetch_result);

/**
 * @brief Returns the topic path.
 *
 * @param topic_result topic result to retrieve the topic path from
 *
 * @return the topic path. NULL, if `topic_result` is NULL.
 */
char *diffusion_topic_result_get_path(const DIFFUSION_TOPIC_RESULT_T *topic_result);

/**
 * @brief Returns the topic type.
 *
 * @param topic_result topic result to retrieve the topic type from
 *
 * @return the topic type. -1, if `topic_result` is NULL.
 */
TOPIC_TYPE_T diffusion_topic_result_get_topic_type(const DIFFUSION_TOPIC_RESULT_T *topic_result);

/**
 * @brief Returns the topic value.
 *
 * This will only return a value if the fetch request specified
 * `diffusion_fetch_request_with_values` and the topic actually
 * had a value. For topics that have no value this will return null.
 *
 * @param topic_result topic result to retrieve the value from
 *
 * @return the topic value or NULL if none available. If `topic_result` is NULL, NULL will also
 *         be returned.
 */
DIFFUSION_VALUE_T *diffusion_topic_result_get_value(const DIFFUSION_TOPIC_RESULT_T *topic_result);

/**
 * @brief Returns the topic specification.
 *
 * If the request specified `diffusion_fetch_request_with_properties`,
 * the result reflect the topic's specification and
 * can be used to create an identical topic. If the request did not
 * specify `diffusion_fetch_request_with_properties`, the
 * specification's property map will be empty.
 *
 * @param topic_result the topic result to retrieve the topic specification from
 *
 * @return the topic specification. NULL, if `topic_result` is NULL.
 */
TOPIC_SPECIFICATION_T *diffusion_topic_result_get_specification(const DIFFUSION_TOPIC_RESULT_T *topic_result);

/**
 * @brief Free a memory allocated `DIFFUSION_TOPIC_RESULT_T`.
 *
 * @param topic_result topic result to be freed
 */
void diffusion_topic_result_free(DIFFUSION_TOPIC_RESULT_T *topic_result);

/**
 * @brief Callback when a fetch request has returned a result.
 *
 * @param fetch_result The result of the fetch request
 * @param context      User supplied context passed in from `DIFFUSION_FETCH_REQUEST_PARAMS_T`
 *
 * @return HANDLERS_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_fetch_result_cb)(const DIFFUSION_FETCH_RESULT_T *fetch_result, void *context);

/**
 * @brief Structure describing a fetch request.
 */
typedef struct diffusion_fetch_request_params_s {
        /// Specifies a topic selector which
        /// selects the topics to be fetched.
        const char *topic_selector;
        /// Fetch request
        DIFFUSION_FETCH_REQUEST_T *fetch_request;
        /// Callback when a fetch request has returned
        /// a result.
        on_fetch_result_cb on_fetch_result;
        /// Callback on error
        ERROR_HANDLER_T on_error;
        /// Callback on discard
        DISCARD_HANDLER_T on_discard;
        /// User supplied context
        void *context;
} DIFFUSION_FETCH_REQUEST_PARAMS_T;

/**
 * @brief Specifies a logical start point within the topic tree.
 *
 * If specified, only results for topics with a path that is lexically
 * equal to or 'after' the specified path will be returned.
 *
 * This is the inclusive equivalent of `diffusion_fetch_request_after` and if used
 * will override any previous `diffusion_fetch_request_after` or
 * `diffusion_fetch_request_from` constraint.
 *
 * @param fetch_request Fetch request to mutate
 * @param topic_path    The topic path from which results are to be returned
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_from(const DIFFUSION_FETCH_REQUEST_T *fetch_request, const char *topic_path,  DIFFUSION_API_ERROR *error);

/**
 * @brief Specifies a logical start point within the topic tree.
 *
 * If specified, only results for topics with a path that is lexically
 * 'after' the specified path will be returned.
 *
 * This is the non inclusive equivalent of `diffusion_fetch_request_from` and if
 * used will override any previous `diffusion_fetch_request_from` or
 * `diffusion_fetch_request_after` constraint.
 *
 * @param fetch_request Fetch request to mutate
 * @param topic_path    The topic path after which results are to be returned
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_after(const DIFFUSION_FETCH_REQUEST_T *fetch_request, const char *topic_path, DIFFUSION_API_ERROR *error);

/**
 * @brief Specifies a logical end point within the topic tree.
 *
 * If specified, only results for topics with a path that is lexically
 * equal to or 'before' the specified path will be returned.
 *
 * This is the inclusive equivalent of `diffusion_fetch_request_before` and if
 * used will override any previous `diffusion_fetch_request_before` or
 * `diffusion_fetch_request_to` constraint.
 *
 * @param fetch_request Fetch request to mutate
 * @param topic_path    The topic path before which results are to be returned
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_to(const DIFFUSION_FETCH_REQUEST_T *fetch_request, const char *topic_path, DIFFUSION_API_ERROR *error);

/**
 * @brief Specifies a logical end point within the topic tree.
 *
 * If specified, only results for topics with a path that is lexically
 * 'before' the specified path will be returned.
 *
 * This is the non inclusive equivalent of `diffusion_fetch_request_to` and if used
 * will override any previous `diffusion_fetch_request_to` or `diffusion_fetch_request_before`
 * constraint.
 *
 * @param fetch_request Fetch request to mutate
 * @param topic_path    The topic path before which results are to be returned
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_before(const DIFFUSION_FETCH_REQUEST_T *fetch_request, const char *topic_path, DIFFUSION_API_ERROR *error);

/**
 * @brief Specifies that only topics of the specified topic types should be
 * returned.
 *
 * If this is not specified, all types will be returned
 * (unless constrained by `diffusion_fetch_request_with_values`).
 *
 * This may be used instead to further constrain the results when using
 * `diffusion_fetch_request_with_values`. For example, you can specify
 * `DATATYPE_JSON` to `diffusion_fetch_request_with_values` then
 * specify `TOPIC_TYPE_JSON` here to ensure that only JSON
 * topics are returned and not those topics that are logically value
 * subtypes of JSON (e.g. `TOPIC_TYPE_STRING`).
 *
 * If `diffusion_fetch_request_with_values` has been specified then the types
 * specified here must be compatible with the value datatype specified.
 *
 * `TOPIC_TYPE_ROUTING` may not be specified as only target topic types
 * may be selected.
 *
 * @param fetch_request Fetch request to mutate
 * @param topic_types   Topic types to be selected
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_topic_types(const DIFFUSION_FETCH_REQUEST_T *fetch_request, SET_T *topic_types, DIFFUSION_API_ERROR *error);

/**
 * @brief Specifies that values should be returned for selected topics, and
 * constraining the selection to only those topics with a data type
 * compatible with the specified value datatype.
 *
 * If `datatype` is NULL, values will be returned for all topic types
 * (unless constrained by `diffusion_fetch_request_topic_types`).
 *
 * The specified value constrains the topic types. So, any topic types
 * specified in a previous call to `diffusion_fetch_request_topic_types` that
 * cannot be read as the specified datatype will be removed from the list
 * of topic types.
 *
 * @param fetch_request Fetch request to mutate
 * @param datatype      Pointer to the datatype of the required values
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_with_values(const DIFFUSION_FETCH_REQUEST_T *fetch_request, DIFFUSION_DATATYPE *datatype, DIFFUSION_API_ERROR *error);

/**
 * @brief Specifies that the fetch request should not return values.
 *
 * @param fetch_request fetch request to mutate
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_without_values(const DIFFUSION_FETCH_REQUEST_T *fetch_request, DIFFUSION_API_ERROR *error);

/**
 * @brief Specifies that all properties associated with each topic's
 * specification should be returned.
 *
 * @param fetch_request Fetch request to mutate
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_with_properties(const DIFFUSION_FETCH_REQUEST_T *fetch_request, DIFFUSION_API_ERROR *error);

/**
 * @brief Include the details of reference topics that are not yet published.
 *
 *
 * Topic views that use the `delay by` clause
 * create reference topics in an unpublished state. The topics are
 * published once the delay time has expired. A topic in the
 * unpublished state prevents a lower priority topic view from creating
 * a reference topic with the same path.
 *
 * A reference topic in the unpublished state which matches the query
 * will only be included in the fetch results if the session has
 * PATH_PERMISSION_READ_TOPIC permission for the
 * reference's source topic as well as PATH_PERMISSION_READ_TOPIC permission for
 * the reference topic. Requiring PATH_PERMISSION_READ_TOPIC permission for the
 * source topic ensures less privileged sessions cannot derive
 * information from the existence of the reference topic before the
 * delay time has expired.
 *
 * @param fetch_request Fetch request to mutate
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_with_unpublished_delayed_topics(const DIFFUSION_FETCH_REQUEST_T *fetch_request, DIFFUSION_API_ERROR *error);

/**
 * @brief Specifies a maximum number of topic results to be returned from the
 * start of the required range.
 *
 * If this is not specified, the number of results returned will only be
 * limited by other constraints of the request.
 *
 * This should be used to retrieve results in manageable batches and
 * prevent very large result sets.
 *
 * If there are potentially more results that would satisfy the other
 * constraints then the fetch result will indicate so via the
 * `diffusion_fetch_result_has_more` function.
 *
 * Zero can be supplied to return no results. Such a request can be used
 * together with `diffusion_fetch_result_has_more` to query whether there are
 * topics that match the selector provided to `diffusion_fetch_request_fetch`
 * without retrieving the details of any of the topics. To retrieve unlimited
 * topics use `INT_MAX` which is the default value.
 *
 * Either this or `diffusion_fetch_request_last` may be specified. This will therefore
 * override any previous `diffusion_fetch_request_last` or `diffusion_fetch_request_first` constraint.
 *
 * @param fetch_request Fetch request to mutate
 * @param number        The maximum number of results to return from the start
 *                      of the range. Must be non-negative.
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_first(const DIFFUSION_FETCH_REQUEST_T *fetch_request, int32_t number, DIFFUSION_API_ERROR *error);

/**
 * @brief Specifies a maximum number of topic results to be returned from the
 * end of the required range.
 *
 * This is similar to `diffusion_fetch_request_first` except that the specified
 * number of results are returned from the end of the range. This is
 * useful for paging backwards through a range of topics. The results
 * will be returned in topic path order (not reverse order).
 *
 * Either this or `diffusion_fetch_request_first` may be specified. This will
 * therefore override any previous `diffusion_fetch_request_first` or
 * `diffusion_fetch_request_last` constraint.
 *
 * Zero can be supplied to return no results. Such a request can be used
 * together with `diffusion_fetch_result_has_more` to query whether there are
 * topics that match the selector provided to `diffusion_fetch_request_fetch`
 * without retrieving the details of any of the topics. To retrieve unlimited
 * topics use `INT_MAX` which is the default value.
 *
 * @param fetch_request Fetch request to mutate
 * @param number        The maximum number of results to return from the end
 *                      of the range. Must be non-negative.
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_last(const DIFFUSION_FETCH_REQUEST_T *fetch_request, int32_t number, DIFFUSION_API_ERROR *error);

/**
 * @brief Specifies the maximum data size of the result set.
 * This may be used to constrain the size of the result.
 *
 * @param fetch_request Fetch request to mutate
 * @param maximum       the maximum size of the result set in bytes.
 * @param error         Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_maximum_result_size(const DIFFUSION_FETCH_REQUEST_T *fetch_request, uint32_t maximum, DIFFUSION_API_ERROR *error);

/**
 * @brief Specifies a limit on the number of results returned for each deep
 * branch.
 *
 * A deep branch has a root path that has a number of parts equal to the
 * `deep_branch_depth` parameter. The `deep_branch_limit` specifies the
 * maximum number of results for each deep branch.
 *
 * This method is particularly useful for incrementally exploring a
 * topic tree from the root, allowing a breadth-first search strategy.
 *
 * For example, given a topic tree containing the topics with the
 * following paths:
 *
 * <pre>
 * x/0
 * x/x/1
 * x/x/x/2
 * y/y/y/y/3
 * y/y/y/4
 * z/5
 * z/z/6
 * </pre>
 *
 * Then
 *
 * <pre>
 * 
 * DIFFUSION_FETCH_REQUEST_T *fetch_request = diffusion_fetch_request_init(session);
 * diffusion_fetch_request_limit_deep_branches(fetch_request, 1, 1, NULL);
 * 
 * DIFFUSION_FETCH_REQUEST_PARAMS_T params = {
 *         .topic_selector = "?.//",
 *         .fetch_request = fetch_request,
 *         .on_fetch_result = on_fetch_result
 * };
 *
 * diffusion_fetch_request_fetch(session, params);
 * 
 * </pre>
 *
 * will return results with the paths `x/0`, `y/y/y/y/3`,
 * and `z/5`. The application can then determine the roots of the
 * tree are `x`, `y`, and `z`.
 *
 * The `deep_branch_limit` parameter can usefully be set to
 * `0`. For example, given the same example topic tree,
 *
 * <pre>
 *
 * DIFFUSION_FETCH_REQUEST_T *fetch_request = diffusion_fetch_request_init(session);
 * diffusion_fetch_request_limit_deep_branches(fetch_request, 3, 0, NULL);
 * 
 * DIFFUSION_FETCH_REQUEST_PARAMS_T params = {
 *         .topic_selector = "?.//",
 *         .fetch_request = fetch_request,
 *         .on_fetch_result = on_fetch_result
 * };
 *
 * diffusion_fetch_request_fetch(session, params);
 *
 * </pre>
 *
 * will only return results having paths with fewer than three parts;
 * namely `x/0`, and `z/5`.
 *
 * The fetch result does not indicate whether this option caused some
 * results to be filtered from deep branches. It has no affect on the
 * `diffusion_fetch_result_has_more` result. If the result set
 * contains `deep_branch_limit` results for a particular deep
 * branch, some topics from that branch may have been filtered.
 *
 * @param fetch_request     Fetch request to mutate
 * @param deep_branch_depth The number of parts in the root path of a
 *                          branch for it to be considered deep
 * @param deep_branch_limit The maximum number of results to return for
 *                          each deep branch
 * @param error             Populated if an error occurs. Can be NULL.
 *
 * @return true if the fetch request was successfully mutated. False, otherwise. In this case, if
 *         a non-NULL `DIFFUSION_API_ERROR` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_fetch_request_limit_deep_branches(const DIFFUSION_FETCH_REQUEST_T *fetch_request, uint32_t deep_branch_depth, uint32_t deep_branch_limit, DIFFUSION_API_ERROR *error);

/**
 * @brief Sends a fetch request to the server.
 *
 * Results are returned for all topics matching the selector that
 * satisfy the request constraints within any range defined by
 * `diffusion_fetch_request_from`/`diffusion_fetch_request_after` and/or
 * `diffusion_fetch_request_to`/`diffusion_fetch_request_before`.
 *
 * @param session The session handle. If NULL, the function returns immediately.
 * @param params  Parameters describing the fetch request and
 *                callbacks handlers which may be invoked in response.
 */
void diffusion_fetch_request_fetch(SESSION_T *session, DIFFUSION_FETCH_REQUEST_PARAMS_T params);

#endif
