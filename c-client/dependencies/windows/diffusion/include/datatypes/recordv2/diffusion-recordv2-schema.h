#ifndef _diffusion_recordv2_schema_h_
#define _diffusion_recordv2_schema_h_ 1

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
 * @file diffusion-recordv2-schema.h
 *
 * Schema for RecordV2.
 */

#include <stdlib.h>

#include "list.h"
#include "datatypes/recordv2/diffusion-recordv2-mutable-record-model.h"
#include "datatypes/recordv2/diffusion-recordv2-schema-record.h"
#include "datatypes/recordv2/diffusion-recordv2-schema-field.h"

/**
 * Opaque recordv2 schema data type.
 */
typedef struct DIFFUSION_RECORDV2_SCHEMA_T DIFFUSION_RECORDV2_SCHEMA_T;

/**
 * @brief Create a mutable model based upon the schema.
 *
 * The model will be created with all mandatory record occurrences and all mandatory field occurrences initialized to default values.
 * Such a model may be mutated and used to generate updated RecordV2 occurrences for updating purposes. `diffusion_recordv2_mutable_record_model_free`
 * should be called on this pointer when no longer needed.
 *
 * @param recordv2_schema the recordv2 schema
 *
 * @return a mutable recordv2 model. NULL if recordv2_schema is NULL.
 */
DIFFUSION_RECORDV2_MUTABLE_RECORD_MODEL_T *diffusion_recordv2_schema_create_mutable_model(const DIFFUSION_RECORDV2_SCHEMA_T *recordv2_schema);

/**
 * @brief Returns an immutable, ordered list of record definitions.
 *
 * There will be at least one.
 *
 * @param recordv2_schema the recordv2 schema
 *
 * @return a list of DIFFUSION_RECORDV2_SCHEMA_RECORD_Ts in the schema. NULL if the
 *         recordv2_schema is NULL.
 */
LIST_T *diffusion_recordv2_schema_get_records(const DIFFUSION_RECORDV2_SCHEMA_T *recordv2_schema);

/**
 * @brief Returns the schema in a JSON format.
 *
 * @param recordv2_schema the recordv2 schema.
 *
 * @return schema in JSON string format - must be freed after use. NULL if the recordv2_schema is NULL.
 */
char *diffusion_recordv2_schema_as_json_string(const DIFFUSION_RECORDV2_SCHEMA_T *recordv2_schema);

/**
 * @brief Free a recordv2 schema.
 *
 * @param recordv2_schema recordv2 schema to be freed
 */
void diffusion_recordv2_schema_free(DIFFUSION_RECORDV2_SCHEMA_T *recordv2_schema);

#endif
