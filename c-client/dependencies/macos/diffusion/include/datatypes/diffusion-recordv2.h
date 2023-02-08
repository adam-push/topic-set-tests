#ifndef _diffusion_recordv2_value_h_
#define _diffusion_recordv2_value_h_ 1

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
 * @file diffusion-recordv2.h
 *
 * RecordV2 datatype associated functions.
 */

#include <stdlib.h>

#include "buf.h"
#include "list.h"
#include "diffusion-api-error.h"
#include "diffusion-value.h"
#include "datatypes/recordv2/diffusion-recordv2-schema.h"
#include "datatypes/recordv2/diffusion-recordv2-record-model.h"

/**
 * @brief Parses the value into a model based upon a specified schema.
 *
 * This assumes that data is compatible with the schema and does not do any
 * validation. There is no need to validate the data if this has been done
 * on entry or at the server. However, if the data is invalid then issues
 * may occur when attempting to access it.
 *
 * @param value        the diffusion value to be read
 * @param schema       the schema to retrieve the model from
 * @param record_model a pointer to a pointer which has its value populated with a `DIFFUSION_RECORDV2_RECORD_MODEL_T`.
 *                     Must be freed.
 * @param error        populated if an error occurs. Can be NULL.
 *
 * @return true if the value was successfully read and the `record_model`
 *         parameter was populated with a record_model. False, otherwise.
 */
bool diffusion_recordv2_as_model(const DIFFUSION_VALUE_T *value, const DIFFUSION_RECORDV2_SCHEMA_T *schema, DIFFUSION_RECORDV2_RECORD_MODEL_T **record_model, DIFFUSION_API_ERROR *error);

/**
 * @brief Parses the value into a model based upon a specified schema.
 *
 * @param value        the diffusion value to be read
 * @param schema       the schema to be validated against and retrieve the model from
 * @param record_model a pointer to a pointer which has its value populated with a `DIFFUSION_RECORDV2_RECORD_MODEL_T`.
 *                     Must be freed.
 * @param error        populated if an error occurs. Can be NULL.
 *
 * @return true if the value was successfully read and the `record_model`
 *         parameter was populated with a record_model. False, otherwise.
 */
bool diffusion_recordv2_as_validated_model(const DIFFUSION_VALUE_T *value, const DIFFUSION_RECORDV2_SCHEMA_T *schema, DIFFUSION_RECORDV2_RECORD_MODEL_T **record_model, DIFFUSION_API_ERROR *error);

/*
 * @brief Get a list of fields from the RecordV2 diffusion value.
 *
 * This disregards record boundaries. If there is more than one record, they
 * are concatenated to produce a list of all of the fields.
 * This method would normally only be used when it is known that there is
 * only one record.
 *
 * @param value     diffusion value to be read
 * @param as_fields pointer to a `LIST_T` to be populated
 * @param error     populated if an error occurs. Can be NULL.
 *
 * @return true if the value was successfully read and the `as_fields`
 *         parameter was populated with all the fields. False, otherwise.
 */
bool diffusion_recordv2_as_fields(const DIFFUSION_VALUE_T *value, LIST_T **as_fields, DIFFUSION_API_ERROR *error);

/**
 * @brief Get a list of lists where each element represents a record within the RecordV2
 * diffusion value.
 *
 * @param value       diffusion value to be read
 * @param as_records  pointer to a `LIST_T` to be populated
 * @param error error populated if an error occurs. Can be NULL.
 *
 * @return true if the value was successfully read and `as_records` was populated with
 *         lists of all the records contained in the RecordV2 diffusion value.
 *         False otherwise.
 */
bool diffusion_recordv2_as_records(const DIFFUSION_VALUE_T *value, LIST_T **as_records, DIFFUSION_API_ERROR *error);

/**
 * @brief Get a stringified representation of the RecordV2 diffusion value.
 *
 * @param value              diffusion value to be read.
 * @param recordv2_as_string pointer to a pointer where the stringified RecordV2
 *                           data will be stored
 * @param error              populated if an error occurs. Can be NULL.
 *
 * @return true if the value was sucessfully read and `recordv2_as_string` was set to a pointer.
 *         containing the stringified representation of the RecordV2 diffusion value.
 *         False otherwise.
 */
bool diffusion_recordv2_to_string(const DIFFUSION_VALUE_T *value, char **recordv2_as_string, DIFFUSION_API_ERROR *error);

/**
 * @brief Write a recordv2 value into a `BUF_T` for an update.
 *
 * @param recordv2 recordv2 bytes to be written into the `BUF_T`
 * @param buf      `BUFT_T` to write the recordv2 value into
 *
 * @return true if the value was successfully written into the `BUF_T`.
 *         False is returned if recordv2 or buf are NULL.
 */
bool write_diffusion_recordv2_value(const void *recordv2, const BUF_T *buf);

#endif
