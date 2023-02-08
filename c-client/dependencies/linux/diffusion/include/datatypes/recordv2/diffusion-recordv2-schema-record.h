#ifndef _diffusion_recordv2_schema_record_h_
#define _diffusion_recordv2_schema_record_h_ 1

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
 * @file diffusion-recordv2-schema-record.h
 *
 * Schema Record for RecordV2.
 */

#include <stdlib.h>
#include <stdbool.h>

#include "list.h"
#include "diffusion-api-error.h"

/**
 * Opaque recordv2 schema record data type.
 */
typedef struct DIFFUSION_RECORDV2_SCHEMA_RECORD_T DIFFUSION_RECORDV2_SCHEMA_RECORD_T;

/**
 * @brief Retrieves a copy of the ordered list of field definitions in the schema.
 *
 * There will be at least one.
 *
 * @param schema_record the schema record
 * @param fields        populated with a memory allocated `LIST_T` containing the records fields
 *                      as `DIFFUSION_RECORDV2_SCHEMA_FIELD_T`s. Must be freed after use.
 * @param error         populated if an error occurs. Can be NULL.
 *
 * @return true if the fields were successfully retrieved from the schema_record
 *         false, otherwise.
 */
bool diffusion_recordv2_schema_record_get_fields(const DIFFUSION_RECORDV2_SCHEMA_RECORD_T *schema_record, LIST_T **fields, DIFFUSION_API_ERROR *error);

/**
 * @brief Retrieves a copy of the node name.
 *
 * @param schema_record the schema record
 * @param record_name   populated with a pointer to the record's name. Must be freed after use.
 * @param error         populated if an error occurs. Can be NULL.
 *
 * @return true if the name of the field was successfully retrieved from the schema_record
 *         false, otherwise.
 */
bool diffusion_recordv2_schema_record_get_name(const DIFFUSION_RECORDV2_SCHEMA_RECORD_T *schema_record, char **record_name, DIFFUSION_API_ERROR *error);

/**
 * @brief Retrieves minimum number of occurrences.
 *
 * This may be 0 for a variable multiplicity field; otherwise, it will
 * be a positive value.
 *
 * @param schema_record the schema record
 * @param min           populated with the record's minimum number of occurrences.
 * @param error         populated if an error occurs. Can be NULL.
 *
 * @return true if the minimum was successfully retrieved from the schema_record
 *         false, otherwise.
 */
bool diffusion_recordv2_schema_record_get_min(const DIFFUSION_RECORDV2_SCHEMA_RECORD_T *schema_record, int *min, DIFFUSION_API_ERROR *error);

/**
 * @brief Retrieves maximum number of occurrences.
 *
 * This will be a positive value greater than or equal to the
 * "min" value, or -1 to indicate an unlimited number.
 *
 * @param schema_record the schema record
 * @param max           populated with the record's maximum number of occurrences.
 * @param error         populated if an error occurs. Can be NULL.
 *
 * @return true if the maximum was successfully retrieved from the schema_record.
 *         false, otherwise.
 */
bool diffusion_recordv2_schema_record_get_max(const DIFFUSION_RECORDV2_SCHEMA_RECORD_T *schema_record, int *max, DIFFUSION_API_ERROR *error);

/**
 * @brief Retrieves true if the node has variable multiplicity.
 * i.e. min != max.
 *
 * @param schema_record the schema record
 * @param is_variable   populated with a bool stating if the record has variable multiplicity.
 * @param error         populated if an error occurs. Can be NULL.
 *
 * @return true if the records's variable multiplicity was successfully retrieved from the schema_record
 *         false, otherwise.
 */
bool diffusion_recordv2_schema_record_is_variable(const DIFFUSION_RECORDV2_SCHEMA_RECORD_T *schema_record, bool *is_variable, DIFFUSION_API_ERROR *error);

/**
 * @brief Free a schema record.
 *
 * @param schema_record schema record to be freed
 */
void diffusion_recordv2_schema_record_free(DIFFUSION_RECORDV2_SCHEMA_RECORD_T *schema_record);

#endif
