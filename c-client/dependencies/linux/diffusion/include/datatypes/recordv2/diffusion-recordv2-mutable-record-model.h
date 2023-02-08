#ifndef _diffusion_recordv2_mutable_record_model_h_
#define _diffusion_recordv2_mutable_record_model_h_ 1

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
 * @file diffusion-recordv2-mutable-record-model.h
 *
 * Mutable data model for RecordV2.
 *
 * This is a mutable data model of RecordV2 `DIFFUSION_VALUE_T` data based upon a
 * `DIFFUSION_RECORDV2_SCHEMA_T`.
 *
 * An initial version of such a model can be created from a schema using the
 * `diffusion_recordv2_schema_create_mutable_model` function. A model created in this way will
 * have all mandatory fields set to default values.
 *
 * The model may then be updated as required and then at any time a pointer
 * `DIFFUSION_VALUE_T` struct can be generated from the current state using
 * the `diffusion_recordv2_mutable_record_model_as_value` function.
 * The `DIFFUSION_VALUE_T` pointer may then be used to update a
 * topic, by retrieving its bytes values using `diffusion_value_get_raw_bytes`.
 * The supplied bytes can then be written into a `BUF_T` with `write_diffusion_recordv2_value`
 * ready to be used in an update.
 */

#include <stdlib.h>
#include <stdbool.h>

#include "diffusion-api-error.h"
#include "datatypes/diffusion-value.h"

/**
 * Opaque recordv2 mutable record model data type.
 */
typedef struct DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T;

/**
 * @brief Creates an immutable `DIFFUSION_VALUE_T` generated from the model.
 *
 * @param mutable_record_model the mutable record model to generate the value from
 * @param value                pointer to a pointer which will have its value set to the `DIFFUSION_VALUE_T`
 * @param error                populated if an error occurs. Can be NULL.
 *
  * @return true if the mutable record model was transformed into a `DIFFUSION_VALUE_T`.
 *         False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_as_value(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, DIFFUSION_VALUE_T **value, DIFFUSION_API_ERROR *error);

/**
 * @brief Returns the actual number of occurrences of a named field within a specified record occurrences.
 *
 * For all but variable fields this simply returns the schema defined number of occurrences of the field.
 *
 * @param mutable_record_model the mutable record model to retrieve the field count from
 * @param record_name          the record's name
 * @param record_index         the record index
 * @param field_name           the field name
 * @param field_count          pointer to a variable which will have its value set to the field count
 * @param error                populated if an error occurs. Can be NULL.
 *
 * @return true if the mutable record model's field count was successfully retrieved.
 *         False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_field_count(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, const char *record_name, int record_index, const char *field_name, int *field_count, DIFFUSION_API_ERROR *error);

/**
 * @brief Get a field value.
 *
 * This allows an item to be addressed using a key of the form `recordName(recordIndex).fieldName(fieldIndex)`.
 * Indexes may be omitted in which case 0 is assumed. The record part may also be omitted in which case the first occurrence of the first record is assumed.
 *
 * @param mutable_record_model the mutable record model to retrieve the field value from
 * @param key                  the key
 * @param field_value          pointer to the field_value variable which will have its value set to the field value.
 * @param error                populated if an error occurs. Can be NULL.
 *
 * @return true if the mutable record model's field value was successfully retrieved.
 *         False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_get_field_value_with_key(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, const char *key, char **field_value, DIFFUSION_API_ERROR *error);

/**
 * @brief Get a field value.
 *
 * @param mutable_record_model the mutable record model
 * @param record_name          the record name
 * @param record_index         the record index
 * @param field_name           the field name
 * @param field_index          the field index
 * @param field_value          pointer to a variable which will have its value set to the
 *                             field value. Must be freed after use.
 * @param error                populated if an error occurs. Can be NULL.
 *
 * @return true if the mutable record model's field value was successfully retrieved.
 *         False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_get_field_value(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, const char *record_name, int record_index, const char *field_name, int field_index, char **field_value, DIFFUSION_API_ERROR *error);

/**
 * @brief Returns the actual number of occurrences of a named record.
 *
 * If the record is not variable, this is the same as the defined number of occurrences in the schema.
 *
 * @param mutable_record_model the mutable record model
 * @param record_name          the record name
 * @param record_count         pointer to a variable which will have its value set to the
 *                             record count.
 * @param error                populated if an error occurs. Can be NULL.
 *
 * @return true if the mutable record model's record count was successfully retrieved.
 *         False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_record_count(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, const char *record_name, int *record_count, DIFFUSION_API_ERROR *error);

/**
 * @brief Adds new values to the end of a variable length field list.
 *
 * This is a convenience method for adding to the end of the last record and is therefore useful when there is only one record type.
 *
 * @param mutable_record_model the mutable record model
 * @param values               a NULL terminated array of values to be added
 * @param error                populated if an error occurs. Can be NULL.
 *
* @return true if the values were successfully added to the mutable record model.
 *        False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_add_values(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, char **values, DIFFUSION_API_ERROR *error);

/**
 * @brief Adds new values to the end of a variable length field list.
 *
 * This can only be used for a variable multiplicity field which can only be the last field in a record and therefore the field does not need to be named.
 *
 * @param mutable_record_model the mutable record model
 * @param record_name          the name of the record to add values to
 * @param record_index         the record's index
 * @param values               a NULL terminated array of values to be added
 * @param error                populated if an error occurs. Can be NULL.
 *
* @return true if the values were successfully added to the record within the mutable record model.
 *        False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_add_values_to_record(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, const char *record_name, int record_index, char **values, DIFFUSION_API_ERROR *error);

/**
 * @brief Adds a new initialized record occurrence to the end of a variable multiplicity record list.
 *
 * As the only variable multiplicity record can be the last one there is no need to name the record.
 * This method will add to the list of occurrences of the last defined record. The record will be initialized
 * with default values appropriate to the schema definition and may then have individual field items set separately.
 *
 * @param mutable_record_model the mutable record model
 * @param error                populated if an error occurs. Can be NULL.
 *
 * @return true if the record was successfully added to the mutable record model.
 *        False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_add_record(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, DIFFUSION_API_ERROR *error);

/**
 * @brief Remove all optional instances of a variable multiplicity field.
 *
 * As a variable repeating field can only be the last or only field within a record then the field name does not need to be specified.
 * This will only remove field occurrences down to the minimum number of occurrences specified by the schema.
 * If the last or only field within the record is not defined as variable multiplicity, this would have no effect.
 *
 * @param mutable_record_model the mutable record model
 * @param record_name          the name of the record
 * @param record_index         the record's index
 * @param error                populated if an error occurs. Can be NULL.
 *
 * @return true if the variable fields in the mutable record model were cleared.
 *        False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_clear_variable_fields(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, const char *record_name, int record_index, DIFFUSION_API_ERROR *error);

/**
 * @brief Removes all optional instances of a variable multiplicity record.
 *
 * As a variable repeating record can only be the last or only record then the record name does not need to be specified.
 * This will only remove record occurrences down to the minimum number of occurrences specified by the schema.
 * If the last or only record is not defined as variable multiplicity, this would have no effect.
 *
 * @param mutable_record_model the mutable record model
 * @param error                populated if an error occurs. Can be NULL.
 *
 * @return true if the variable records in the mutable record model were cleared.
 *        False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_clear_variable_records(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, DIFFUSION_API_ERROR *error);

/**
 * @brief Removes the specified occurrence of a variable multiplicity field.
 *
 * A variable multiplicity field must be the last or only field within a record and therefore the field name is not required.
 *
 * @param mutable_record_model the mutable record model
 * @param record_name          the name of the record
 * @param record_index         the record's index
 * @param field_index          the field's index
 * @param error                populated if an error occurs. Can be NULL.
 *
 * @return true if the field within the record in the mutable record model was removed.
 *        False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_remove_field(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, const char *record_name, int record_index, int field_index, DIFFUSION_API_ERROR *error);

/**
 * @brief Removes the specified occurrence of a variable multiplicity record.
 *
 * A variable multiplicity record must be the last or only record within a schema and therefore the record name is not required.
 *
 * @param mutable_record_model the mutable record model
 * @param index                the record's index
 * @param error                populated if an error occurs. Can be NULL.
 *
 * @return true if the record in the mutable record model were cleared.
 *        False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_remove_record(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, int index, DIFFUSION_API_ERROR *error);

/**
 * @brief Sets a specified field value.
 *
 * @param mutable_record_model the mutable record model
 * @param record_name          the name of the record
 * @param record_index         the record's index
 * @param field_name           the name of the field
 * @param field_index          the field's index
 * @param value                the value to set the field to
 * @param error                populated if an error occurs. Can be NULL.
 *
 * @return true if the field in the mutable record model has been set.
 *        False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_set_field_value(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, const char *record_name, int record_index, const char *field_name, int field_index, const char *value, DIFFUSION_API_ERROR *error);

/**
 * @brief Sets a specified field value.
 *
 * This allows an item to be addressed using a key of the form `recordName(recordIndex).fieldName(fieldIndex)`. Indexes may be omitted in which case 0 is assumed.
 * The record part may also be omitted in which case the first occurrence of the first record is assumed.
 *
 * @param mutable_record_model the mutable record model
 * @param key
 * @param value                the value to set the field to
 * @param error                populated if an error occurs. Can be NULL.
 *
 * @return true if the field in the mutable record model has been set.
 *        False otherwise.
 */
bool diffusion_recordv2_mutable_record_model_set_field_value_with_key(const DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model, const char *key, const char *value, DIFFUSION_API_ERROR *error);

/**
 * @brief Free a mutable recordv2 model
 *
 * @param mutable_record_model mutable record model to be freed
 */
void diffusion_recordv2_mutable_record_model_free(DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *mutable_record_model);

#endif
