#ifndef _diffusion_session_trees_h_
#define _diffusion_session_trees_h_ 1

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
 * @file session-trees.h
 *
 * Allows a client session to configure session trees.
 *
 * A session tree is a virtual view of the topic tree presented to a session by
 * fetch and subscription operations. Custom session trees for different
 * sessions can be configured using declarative rules maintained by the server
 * to meet data security, data optimisation, or personalisation and localisation
 * requirements. Each session can be presented with a unique session tree based
 * on its session properties.
 *
 * A session tree is produced by applying <em>branch mappings</em> to the topic
 * tree. Branch mappings are organised into <em>branch mapping tables</em>. Each
 * branch mapping table is assigned to a unique path – the <em>session tree
 * branch</em>.
 *
 * A session tree is composed of <em>session paths</em>. Each session path is
 * mapped via the branch mapping tables to a unique <em>topic path</em>.
 *
 * A branch mapping table is an ordered list of (session filter, topic tree branch)
 * pairs. For example, the branch mapping table for the session tree branch
 * <code>market/prices</code> might be:
 *
 * <pre>
 * Session filter                           Topic tree branch
 * ==============                           =================
 * USER_TIER is '1' or $Country is 'DE'     backend/discounted_prices
 * USER_TIER is '2'                         backend/standard_prices
 * $Principal is ''                         backend/delayed_prices
 * </pre>
 *
 * <p>
 * With this configuration, if an unauthenticated session (one that matches the
 * <code>$Principal is ''</code> session filter) subscribes to the session path
 * <code>market/prices/X</code>, and there is a topic bound to the topic path
 * <code>backend/delayed_prices/X</code>, the subscription will complete. The session
 * will receive a subscription notification under the session path
 * <code>market/prices/X</code>, together with the topic properties and the value of
 * the topic. The session is unaware that the data originates from a topic bound
 * to a different topic path. If no topic is bound to
 * <code>backend/delayed_prices/X</code>, the subscription will not resolve and the
 * session will receive no data, even if there is a topic bound to
 * <code>market/prices/X</code>.
 *
 * Session trees complement the data transformation capabilities of
 * {@link topic-views.h topic views}. In our example, the time delayed time feed at
 * <code>backend/delayed_prices</code> could be maintained by a topic view using the
 * <em>delay by</em> clause.
 *
 * Branch mappings are persisted by the server and shared across a cluster, in a
 * similar manner to topic views, security stores, and metric collectors. Branch
 * mappings are editable using this feature, and via the management console.
 *
 * For a given session and session path, at most one branch mapping applies. The
 * applicable branch mapping is chosen as follows:
 * <ul>
 *  <li>Each branch mapping table with session tree branch that is a prefix of the
 *      session path is considered. For a given table, the first branch mapping with
 *      a session filter that matches the session's properties is the one that applies. A
 *      branch mapping table may have no applicable branch mappings for a session.
 *  <li>If there are several such branch mapping tables with a branch mapping
 *      that for the session, the one with the longest prefix of the session path
 *      applies.
 *  <li>If no branch mapping table has a branch mapping for the session, the
 *      session path is translated to the identical topic path.
 * </ul>
 *
 * <h3>Access control</h3>
 * To subscribe to or fetch from a session path, a session must be granted the
 * appropriate path permission to the session path for the operation
 * <code>PATH_PERMISSION_SELECT_TOPIC</code>, or <code>PATH_PERMISSION_READ_TOPIC</code>).
 * The session doesn't require any permissions to the topic path of the topic
 * providing the data.
 *
 * <p>
 * To create or replace branch mappings, a session needs the
 * <code>PATH_PERMISSION_MODIFY_TOPIC</code> path permission for the
 * session tree branch of the branch mapping table,
 * <code>PATH_PERMISSION_EXPOSE_BRANCH</code> path permission for the
 * topic tree branch of each branch mapping, and (if an existing table with the same
 * session tree branch is being replaced) <code>PATH_PERMISSION_EXPOSE_BRANCH</code> permission
 * for each branch mapping of existing table.
 *
 * To retrieve a branch mapping table, a session needs the
 * <code>PATH_PERMISSION_READ_TOPIC</code> path permission for its session tree branch.
 *
 * @since 6.7
 */

#include "session.h"
#include "misc/deprecate.h"
#include "diffusion-api-error.h"


/**
 * Opaque Branch Mapping Table Builder struct
 */
typedef struct DIFFUSION_BRANCH_MAPPING_TABLE_BUILDER_T DIFFUSION_BRANCH_MAPPING_TABLE_BUILDER_T;


/**
 * Opaque Branch Mapping Table struct
 */
typedef struct DIFFUSION_BRANCH_MAPPING_TABLE_T DIFFUSION_BRANCH_MAPPING_TABLE_T;


/**
 * Opaque Branch Mapping struct
 */
typedef struct DIFFUSION_BRANCH_MAPPING_T DIFFUSION_BRANCH_MAPPING_T;


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Initialise a new branch mapping table builder.
 * <code>diffusion_branch_mapping_table_builder_free</code> should be called on this pointer
 * when no longer needed.
 *
 * @return the branch mapping table builder
 */
DIFFUSION_BRANCH_MAPPING_TABLE_BUILDER_T *diffusion_branch_mapping_table_builder_init();


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Reset the stored branch mappings in the branch mapping table builder.
 *
 * @param builder           the branch mapping table builder.
 *
 * @return  the branch mapping table builder
 */
DIFFUSION_BRANCH_MAPPING_TABLE_BUILDER_T *diffusion_branch_mapping_table_builder_reset(
        DIFFUSION_BRANCH_MAPPING_TABLE_BUILDER_T *builder);


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Adds a new branch mapping with the specified values.
 *
 * @param builder           the branch mapping table builder.
 * @param session_filter    the session filter.
 * @param topic_tree_branch     the topic tree branch.
 *
 * @return  the branch mapping table builder
 */
DIFFUSION_BRANCH_MAPPING_TABLE_BUILDER_T *diffusion_branch_mapping_table_builder_add_branch_mapping(
        DIFFUSION_BRANCH_MAPPING_TABLE_BUILDER_T *builder,
        char *session_filter,
        char *topic_tree_branch);


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Creates a branch mapping table for the specified session tree branch
 *
 * @param session_tree_branch    the session filter.
 * @param topic_tree_branch     the .
 * @param builder           the branch mapping table builder.
 *
 * @return  the branch mapping table builder
 */
DIFFUSION_BRANCH_MAPPING_TABLE_T *diffusion_branch_mapping_table_builder_create_table(
        DIFFUSION_BRANCH_MAPPING_TABLE_BUILDER_T *builder,
        char *session_tree_branch);


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Free a branch mapping table builder.
 *
 * @param builder     The branch mapping table builder.
 */
void diffusion_branch_mapping_table_builder_free(
        DIFFUSION_BRANCH_MAPPING_TABLE_BUILDER_T *builder);


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Free a branch mapping table.
 *
 * @param table     The branch mapping table.
 */
void diffusion_branch_mapping_table_free(DIFFUSION_BRANCH_MAPPING_TABLE_T *table);


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Get the session tree branch from a branch mapping table
 *
 * @param table     The branch mapping table.
 *
 * @return the session tree branch if present, NULL otherwise.
 */
char *diffusion_branch_mapping_table_get_session_tree_branch(DIFFUSION_BRANCH_MAPPING_TABLE_T *table);


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Get the branch mappings from a branch mapping table
 *
 * @param table     The branch mapping table
 *
 * @return  a list containing <code>DIFFUSION_BRANCH_MAPPING_T</code> elements if valid or
 *          NULL otherwise.
 */
LIST_T *diffusion_branch_mapping_table_get_branch_mappings(DIFFUSION_BRANCH_MAPPING_TABLE_T *table);


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Free a list containing branch mappings
 *
 * @param branch_mappings       The list containing the branch mappings
 */
void diffusion_branch_mapping_table_free_branch_mappings(LIST_T *branch_mappings);


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Get the session filter from a branch mapping.
 *
 * @param table     The branch mapping
 *
 * @return  the session filter of the branch mapping if valid or
 *          NULL otherwise.
 */
char *diffusion_branch_mapping_get_session_filter(DIFFUSION_BRANCH_MAPPING_T *mapping);


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Get the topic tree branch from a branch mapping.
 *
 * @param table     The branch mapping
 *
 * @return  the topic tree branch of the branch mapping if valid or
 *          NULL otherwise.
 */
char *diffusion_branch_mapping_get_topic_tree_branch(DIFFUSION_BRANCH_MAPPING_T *mapping);


/**
 * @brief callback when branch mapping table has been set on the server
 *
 * @param context               User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int(*branch_mapping_table_set_cb)(void *context);


/**
 * @brief callback when branch mapping table has been set on the server
 *
 * @param session_tree_branches       A list of session tree branches.
 * @param context               User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int(*get_session_tree_branches_cb)(const LIST_T *session_tree_branches, void *context);


/**
 * @brief callback when branch mapping table has been retrieved from the server
 *
 * @param table                 The branch mapping table.
 * @param context               User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int(*get_branch_mapping_table_cb)(const DIFFUSION_BRANCH_MAPPING_TABLE_T *table, void *context);


/**
 * Structure supplied when issuing a <code>diffusion_session_trees_put_branch_mapping_table</code> call.
 */
typedef struct diffusion_session_trees_put_branch_mapping_table_params_s {
        /// The branch mapping table that will be placed in the server
        DIFFUSION_BRANCH_MAPPING_TABLE_T *table;
        /// Callback fired when the branch mapping table is set
        branch_mapping_table_set_cb on_table_set;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_SESSION_TREES_PUT_BRANCH_MAPPING_TABLE_PARAMS_T;


/**
 * Structure supplied when issuing a <code>diffusion_session_trees_get_session_tree_branches</code> call.
 */
typedef struct diffusion_session_trees_get_session_tree_branches_params_s {
        /// Callback fired when the session tree branches are received
        get_session_tree_branches_cb on_session_tree_branches_received;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_SESSION_TREES_GET_SESSION_TREE_BRANCHES_PARAMS_T;


/**
 * Structure supplied when issuing a <code>diffusion_session_trees_get_branch_mapping_table</code> call.
 */
typedef struct diffusion_session_trees_get_branch_mapping_table_params_s {
        /// The session tree branch.
        char *session_tree_branch;
        /// Callback fired when the session tree branches are received
        get_branch_mapping_table_cb on_table_received;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle discards. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} DIFFUSION_SESSION_TREES_GET_BRANCH_MAPPING_TABLE_PARAMS_T;


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Create or replace a branch mapping table.
 *
 * The server ensures that a session tree branch has at most one branch mapping
 * table. Putting a new branch mapping table will replace any previous
 * branch mapping table with the same session tree branch. To remove all branch
 * mappings for a session tree branch, put an empty branch mapping table.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing the branch mapping table and
 *                      the callbacks to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL <code>api_error</code> pointer has been provided, this will be populated
 *         with the error information and should be freed with <code>diffusion_api_error_free</code>.
 */
bool diffusion_session_trees_put_branch_mapping_table(
        SESSION_T *session,
        const DIFFUSION_SESSION_TREES_PUT_BRANCH_MAPPING_TABLE_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Retrieve the session tree branches of the server's branch mapping tables.
 *
 * The results will only include the session tree branches of branch mapping tables
 * that have at least one branch mapping and for which the calling session
 * has <code>PATH_PERMISSION_READ_TOPIC</code> path permission for
 * the session tree branch.
 *
 * Individual branch mapping tables can be retrieved using
 * <code>diffusion_session_trees_get_branch_mapping_table</code>.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing the callbacks to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL <code>api_error</code> pointer has been provided, this will be populated
 *         with the error information and should be freed with <code>diffusion_api_error_free</code>.
 */
bool diffusion_session_trees_get_session_tree_branches(
        SESSION_T *session,
        const DIFFUSION_SESSION_TREES_GET_SESSION_TREE_BRANCHES_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);


/**
 * @ingroup PublicAPI_SessionTrees
 *
 * @brief Retrieve a branch mapping table from the server.
 *
 * If there is no branch mapping table at the given session tree branch, this
 * method will return an empty branch mapping table.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing the session tree branch name and the
 *                      callbacks to be invoked.
 * @param api_error     Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL <code>api_error</code> pointer has been provided, this will be populated
 *         with the error information and should be freed with <code>diffusion_api_error_free</code>.
 */
bool diffusion_session_trees_get_branch_mapping_table(
        SESSION_T *session,
        const DIFFUSION_SESSION_TREES_GET_BRANCH_MAPPING_TABLE_PARAMS_T params,
        DIFFUSION_API_ERROR *api_error);

#endif
