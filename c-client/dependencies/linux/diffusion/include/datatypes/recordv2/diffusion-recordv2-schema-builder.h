#ifndef _diffusion_recordv2_schema_builder_h_
#define _diffusion_recordv2_schema_builder_h_ 1

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
 * @file diffusion-recordv2-schema-builder.h
 *
 * Schema Builder for RecordV2.
 */

#include <stdlib.h>
#include <stdbool.h>

#include "datatypes/recordv2/diffusion-recordv2-schema.h"
#include "diffusion-api-error.h"

/**
 * Opaque recordv2 schema builder data type.
 */
typedef struct DIFFUSION_RECORDV2_SCHEMA_BUILDER_T DIFFUSION_RECORDV2_SCHEMA_BUILDER_T;

/**
 * @brief Initialise a new recordv2 schema builder. `diffusion_recordv2_schema_builder_free` should
 * be called on this pointer when no longer needed.
 *
 * @return a recordv2 schema builder
 */
DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *diffusion_recordv2_schema_builder_init();

/**
 * @brief Add a new single occurrence record to the schema.
 *
 * This is the equivalent to calling:
 * `recordv2_schema_builder_record_with_occurs(builder, name, 1)`.
 *
 * @param recordv2_schema_builder the schema builder
 * @param record_name             the record name
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return true if the record was successfully added to the schema.
 *         False otherwise.
 */
bool diffusion_recordv2_schema_builder_record(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, const char *record_name, DIFFUSION_API_ERROR *error);

/**
 * @brief Add a new single occurrence record to the schema.
 *
 * This is the equivalent to calling:
 * `recordv2_schema_builder_record_with_min_max(builder, name, occurs, occurs)`.
 *
 * @param recordv2_schema_builder the schema builder
 * @param record_name             the record name
 * @param occurs                  the number of times the record is to occur.
 *                                This must be apositive value
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return true if the record was successfully added to the schema.
 *         False otherwise.
 */
bool diffusion_recordv2_schema_builder_record_with_occurs(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, const char *record_name, int occurs, DIFFUSION_API_ERROR *error);

/**
 * @brief Add a new single occurrence record to the schema.
 *
 * A field may not be added after a field that has variable multiplicity
 * (min != max).
 *
 * @param recordv2_schema_builder the schema builder
 * @param record_name             the record name
 * @param min                     the minimum number of occurrences of the record. This must not
 *                                be negative
 * @param max                     the maximum number of occurrences of the record. This must
 *                                either be -1 to indicate an unlimited number or it must be a
 *                                positive number greater than or equal to "min"
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return true if the record was successfully added to the schema.
 *         False otherwise.
 */
bool diffusion_recordv2_schema_builder_record_with_min_max(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, const char *record_name, int min, int max, DIFFUSION_API_ERROR *error);

/**
 * @brief Add a single occurrence string field to the current record.
 *
 * This is the equivalent of calling:
 * `recordv2_schema_builder_string_with_occurs(builder, name, 1)`.
 *
 * @param recordv2_schema_builder the schema builder
 * @param field_name              the field name
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return true if the string field was successfully added to the schema.
 *         False otherwise.
 */
bool diffusion_recordv2_schema_builder_string(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, const char *field_name, DIFFUSION_API_ERROR *error);

/**
 * @brief Add a single occurrence string field to the current record.
 *
 * This is the equivalent to calling:
 * `recordv2_schema_builder_string_with_min_max(builder, name, occurs, occurs)`.
 *
 * @param recordv2_schema_builder the schema builder
 * @param field_name              the field name
 * @param occurs                  the fixed number of times the field should occur within the
 *                                record. This must be positive.
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return true if the string field was successfully added to the schema.
 *         False otherwise.
 */
bool diffusion_recordv2_schema_builder_string_with_occurs(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, const char *field_name, int occurs, DIFFUSION_API_ERROR *error);

/**
 * Add a single occurrence string field to the current record.
 *
 * A field may not be added after a field that has variable multiplicity
 * (min != max).
 *
 * @param recordv2_schema_builder the schema builder
 * @param field_name              the field name
 * @param min                     the minimum number of occurrences of the record. This must not
 *                                be negative
 * @param max                     the maximum number of occurrences of the record. This must
 *                                either be -1 to indicate an unlimited number or it must be a
 *                                positive number greater than or equal to "min"
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return true if the string field was successfully added to the schema.
 *         False otherwise.
 */
bool diffusion_recordv2_schema_builder_string_with_min_max(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, const char *field_name, int min, int max, DIFFUSION_API_ERROR *error);

/**
 * @brief Add a single occurrence integer field to the current record.
 *
 * This is the equivalent of calling:
 * `recordv2_schema_builder_integer_with_occurs(builder, name, 1)`.
 *
 * @param recordv2_schema_builder the schema builder
 * @param field_name              the field name
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return true if the integer field was successfully added to the schema.
 *         False otherwise.
 */
bool diffusion_recordv2_schema_builder_integer(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, const char *field_name, DIFFUSION_API_ERROR *error);

/**
 * @brief Add a single occurrence integer field to the current record.
 *
 * This is the equivalent to calling:
 * `recordv2_schema_builder_integer_with_min_max(builder, name, occurs, occurs)`.
 *
 * @param recordv2_schema_builder the schema builder
 * @param field_name              the field name
 * @param occurs                  the fixed number of times the field should occur within the
 *                                record. This must be positive
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return true if the integer field was successfully added to the schema.
 *         False otherwise.
 */
bool diffusion_recordv2_schema_builder_integer_with_occurs(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, const char *field_name, int occurs, DIFFUSION_API_ERROR *error);

/**
 * @brief Add a single occurrence integer field to the current record.
 *
 * A field may not be added after a field that has variable multiplicity
 * (min != max).
 *
 * @param recordv2_schema_builder the schema builder
 * @param field_name              the field name
 * @param min                     the minimum number of occurrences of the record. This must not
 *                                be negative
 * @param max                     the maximum number of occurrences of the record. This must
 *                                either be -1 to indicate an unlimited number or it must be a
 *                                positive number greater than or equal to "min"
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return true if the integer field was successfully added to the schema.
 *         False otherwise.
 */
bool diffusion_recordv2_schema_builder_integer_with_min_max(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, const char *field_name, int min, int max, DIFFUSION_API_ERROR *error);

/**
 * @brief Add a single occurrence decimal field to the current record.
 *
 * This is the equivalent of calling:
 * `recordv2_schema_builder_decimal_with_occurs(builder, name, scale, 1)`.
 *
 * @param recordv2_schema_builder the schema builder
 * @param field_name              the field name
 * @param scale                   the scale of the field (the number of decimal places). This
 *                                must be a positive value.
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return true if the decimal field was successfully added to the schema.
 *         False otherwise.
 */
bool diffusion_recordv2_schema_builder_decimal(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, const char *field_name, int scale, DIFFUSION_API_ERROR *error);

/**
 * @brief Add a single occurrence decimal field to the current record.
 *
 * This is the equivalent of calling:
 * `recordv2_schema_builder_decimal_with_min_max(builder, name, scale, occurs, occurs)`.
 *
 * @param recordv2_schema_builder the schema builder
 * @param field_name              the field name
 * @param scale                   the scale of the field (the number of decimal places). This
 *                                must be a positive value.
 * @param occurs                  the fixed number of times the field should occur within the
 *                                record. This must be positive.
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return true if the decimal field was successfully added to the schema.
 *         False otherwise.
 */
bool diffusion_recordv2_schema_builder_decimal_with_occurs(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, const char *field_name, int scale, int occurs, DIFFUSION_API_ERROR *error);

/**
 * @brief Add a single occurrence decimal field to the current record.
 *
 * A field may not be added after a field that has variable multiplicity
 * (min != max).
 *
 * @param recordv2_schema_builder the schema builder
 * @param field_name              the field name
 * @param scale                   the scale of the field (the number of decimal places). This
 *                                must be a positive value.
 * @param min                     the minimum number of occurrences of the record. This must not
 *                                be negative.
 * @param max                     the maximum number of occurrences of the record. This must
 *                                either be -1 to indicate an unlimited number or it must be a
 *                                positive number greater than or equal to "min".
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return true if the decimal field was successfully added to the schema.
 *         False otherwise.
 */
bool diffusion_recordv2_schema_builder_decimal_with_min_max(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, const char *field_name, int scale, int min, int max, DIFFUSION_API_ERROR *error);

/**
 * @brief Build an immutable Schema.
 *
 * At least one record with at least one field must have been added to the
 * builder. `diffusion_recordv2_schema_free` should be called on this pointer
 * when no longer needed.
 *
 * @param recordv2_schema_builder the schema builder
 * @param error                   populated if an error occurs. Can be NULL.
 *
 * @return a new immutable schema object representing the current state of
 *         the builder. NULL if an error occurs.
 */
DIFFUSION_RECORDV2_SCHEMA_T *diffusion_recordv2_schema_builder_build(const DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder, DIFFUSION_API_ERROR *error);

/**
 * Free a recordv2 schema builder.
 *
 * @param recordv2_schema_builder the schema builder to be freed.
 */
void diffusion_recordv2_schema_builder_free(DIFFUSION_RECORDV2_SCHEMA_BUILDER_T *recordv2_schema_builder);

#endif
