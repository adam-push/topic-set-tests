#ifndef _diffusion_recordv2_record_model_h_
#define _diffusion_recordv2_record_model_h_ 1

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
 * @file diffusion-recordv2-record-model.h
 *
 * `DIFFUSION_RECORDV2_RECORD_MODEL` data model.
 *
 * A read only model can be created from any RecordV2 `DIFFUSION_VALUE_T` object
 * using the `diffusion_recordv2_as_model` function. The model then provides
 * direct access to the fields within the data. Fields may be accessed
 * either by explicitly specifying the record and field occurrence or
 * by specifying a key of the form:
 *
 * `recordName(recordIndex).fieldName(fieldIndex)`
 */

#include <stdlib.h>
#include <stdbool.h>

#include "diffusion-api-error.h"
#include "datatypes/diffusion-value.h"

/**
 * Opaque recordv2 record model data type.
 */
typedef struct DIFFUSION_RECORDV2_RECORD_MODEL_T DIFFUSION_RECORDV2_RECORD_MODEL_T;

/**
 * @brief Creates an immutable `DIFFUSION_VALUE_T` generated from the model.
 * 
 * @param record_model the model to generate the value from
 * @param value        pointer to a pointer which will have its value set to the `DIFFUSION_VALUE_T`
 * @param error        populated if an error occurs. Can be NULL.
 * 
  * @return true if the record model was transformed into a DIFFUSION_VALUE_T.
 *         False otherwise.
 */
bool diffusion_recordv2_record_model_as_value(const DIFFUSION_RECORDV2_RECORD_MODEL_T * record_model, DIFFUSION_VALUE_T **value, DIFFUSION_API_ERROR *error);

/**
 * @brief Returns the actual number of occurrences of a named field within a specified record occurrences.
 *
 * For all but variable fields this simply returns the schema defined number of occurrences of the field.
 * 
 * @param record_model the model to retrieve the field count from
 * @param record_name  the record's name
 * @param record_index the record index
 * @param field_name   the field name
 * @param field_count  pointer to a variable which will have its value set to the field count
 * @param error        populated if an error occurs. Can be NULL.
 * 
 * @return true if the record model's field count was successfully retrieved.
 *         False otherwise.
 */
bool diffusion_recordv2_record_model_field_count(const DIFFUSION_RECORDV2_RECORD_MODEL_T *record_model, const char *record_name, int record_index, const char *field_name, int *field_count, DIFFUSION_API_ERROR *error);

/**
 * @brief Get a field value.
 *
 * This allows an item to be addressed using a key of the form recordName(recordIndex).fieldName(fieldIndex).
 * Indexes may be omitted in which case 0 is assumed. The record part may also be omitted in which case the first occurrence of the first record is assumed.
 * 
 * @param record_model the record model to retrieve the field value from
 * @param key
 * @param field_value  pointer to the field_value variable which will have its value set to the field value.
 * @param error        populated if an error occurs. Can be NULL.
 * 
 * @return true if the record model's field value was successfully retrieved.
 *         False otherwise.
 */
bool diffusion_recordv2_record_model_get_field_value_with_key(const DIFFUSION_RECORDV2_RECORD_MODEL_T *record_model, const char *key, char **field_value, DIFFUSION_API_ERROR *error);

/**
 * @brief Get a field value.
 * 
 * @param record_model the record model
 * @param record_name  the record name
 * @param record_index the record's index
 * @param field_name   the field name
 * @param field_index  the field's index
 * @param field_value  pointer to a variable which will have its value set to the
 *                     field value. Must be freed after use.
 * @param error        populated if an error occurs. Can be NULL.
 * 
 * @return true if the record model's field value was successfully retrieved.
 *         False otherwise.
 */
bool diffusion_recordv2_record_model_get_field_value(const DIFFUSION_RECORDV2_RECORD_MODEL_T *record_model, const char *record_name, int record_index, const char *field_name, int field_index, char **field_value, DIFFUSION_API_ERROR *error);

/**
 * @brief Returns the actual number of occurrences of a named record.
 *
 * If the record is not variable, this is the same as the defined number of occurrences in the schema.
 * 
 * @param record_model the record model
 * @param record_name  the record name
 * @param record_count pointer to a variable which will have its value set to the
 *                     record count. 
 * @param error        populated if an error occurs. Can be NULL.
 *
 * @return true if the record model's record count was successfully retrieved.
 *         False otherwise.
 */
bool diffusion_recordv2_record_model_record_count(const DIFFUSION_RECORDV2_RECORD_MODEL_T *record_model, const char *record_name, int *record_count, DIFFUSION_API_ERROR *error);

/**
 * @brief Free a recordv2 record model.
 * 
 * @param record_model record model to be freed.
 */
void diffusion_recordv2_record_model_free(DIFFUSION_RECORDV2_RECORD_MODEL_T *record_model);

#endif