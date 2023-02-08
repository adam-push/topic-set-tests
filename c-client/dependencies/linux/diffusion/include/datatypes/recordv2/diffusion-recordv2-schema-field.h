#ifndef _diffusion_recordv2_schema_field_h_
#define _diffusion_recordv2_schema_field_h_ 1

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
 * @file diffusion-recordv2-schema-field.h
 *
 * Schema Field for RecordV2.
 */

#include <stdlib.h>
#include <stdbool.h>

#include "diffusion-api-error.h"

typedef enum {
        FIELD_TYPE_STRING = 1,
        FIELD_TYPE_INTEGER = 2,
        FIELD_TYPE_DECIMAL = 3
} DIFFUSION_RECORDV2_FIELD_TYPE_T;

/**
 * Opaque recordv2 schema field data type.
 */
typedef struct DIFFUSION_RECORDV2_SCHEMA_FIELD_T DIFFUSION_RECORDV2_SCHEMA_FIELD_T;

/**
 * @brief Retrieves the field type.
 *
 * @param schema_field the schema field
 * @param field_type   populated with the field's type
 * @param error        populated if an error occurs. Can be NULL.
 *
 * @return true if the type was successfuly retrieved from the schema_field.
 *         False, otherwise.
 */
bool diffusion_recordv2_schema_field_get_type(const DIFFUSION_RECORDV2_SCHEMA_FIELD_T *schema_field, DIFFUSION_RECORDV2_FIELD_TYPE_T *field_type, DIFFUSION_API_ERROR *error);

/**
 * @brief Retrieves the scale of a `DIFFUSION_RECORDV2_FIELD_TYPE_T` decimal field or 0 for
 * other types.
 *
 * The scale represents the number of places to the right of the decimal
 * point in a decimal number.
 *
 * @param schema_field the schema field
 * @param scale        populated with the field's scale
 * @param error        populated if an error occurs. Can be NULL.
 *
 * @return true if the scale was successfully retrieved from the schema_field.
 *         False, otherwise.
 */
bool diffusion_recordv2_schema_field_get_scale(const DIFFUSION_RECORDV2_SCHEMA_FIELD_T *schema_field, int *scale, DIFFUSION_API_ERROR *error);

/**
 * @brief Retrieves the field name.
 *
 * @param schema_field the schema field
 * @param field_name   populated with a pointer to the field's name. Must be freed after use.
 * @param error        populated if an error occurs. Can be NULL.
 *
 * @return true if the name of the field was successfully retrieved from the schema_field.
 *         False, otherwise.
 */
bool diffusion_recordv2_schema_field_get_name(const DIFFUSION_RECORDV2_SCHEMA_FIELD_T *schema_field, char **field_name, DIFFUSION_API_ERROR *error);

/**
 * @brief Retrieves minimum number of occurrences.
 *
 * This may be 0 for a variable multiplicity field; otherwise, it will
 * be a positive value.
 *
 * @param schema_field the schema field
 * @param min          populated with the field's minimum number of occurrences.
 * @param error        populated if an error occurs. Can be NULL.
 *
 * @return true if the minimum was successfully retrieved from the schema_field.
 *         False, otherwise.
 */
bool diffusion_recordv2_schema_field_get_min(const DIFFUSION_RECORDV2_SCHEMA_FIELD_T *schema_field, int *min, DIFFUSION_API_ERROR *error);

/**
 * @brief Retrieves the maximum number of occurrences.
 *
 * This will be a positive value greater than or equal to the
 * "min" value, or -1 to indicate an unlimited number.
 *
 * @param schema_field the schema field
 * @param max          populated with the field's maximum number of occurrences.
 * @param error        populated if an error occurs. Can be NULL.
 *
 * @return true if the maximum was successfully retrieved from the schema_field.
 *         False, otherwise.
 */
bool diffusion_recordv2_schema_field_get_max(const DIFFUSION_RECORDV2_SCHEMA_FIELD_T *schema_field, int *max, DIFFUSION_API_ERROR *error);

/**
 * @brief Retrieves the field's variable multiplicity. True if the field has variable multiplicity.
 * i.e. min != max.
 *
 * @param schema_field the schema field
 * @param is_variable  populated with a bool stating if the field has variable multiplicity.
 * @param error        populated if an error occurs. Can be NULL.
 *
 * @return true if the field's variable multiplicity was successfully retrieved from the schema_field
 *         False, otherwise.
 */
bool diffusion_recordv2_schema_field_is_variable(const DIFFUSION_RECORDV2_SCHEMA_FIELD_T *schema_field, bool *is_variable, DIFFUSION_API_ERROR *error);

/**
 * @brief Free a schema field.
 *
 * @param schema_field schema field to be freed
 */
void diffusion_recordv2_schema_field_free(DIFFUSION_RECORDV2_SCHEMA_FIELD_T *schema_field);

#endif
