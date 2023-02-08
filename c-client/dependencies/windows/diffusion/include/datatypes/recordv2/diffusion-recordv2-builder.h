#ifndef _diffusion_recordv2_builder_h_
#define _diffusion_recordv2_builder_h_ 1

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
 * @file diffusion-recordv2-builder.h
 *
 * Builder for RecordV2.
 */

#include <stdlib.h>

#include "list.h"

/**
 * Opaque recordv2 value builder data type.
 */
typedef struct DIFFUSION_RECORDV2_BUILDER_T DIFFUSION_RECORDV2_BUILDER_T;

/**
 * @brief Initialise a new recordv2 builder. `diffusion_recordv2_builder_free` should be
 * called on this pointer when no longer needed.
 *
 * @return a recordv2 builder
 */
DIFFUSION_RECORDV2_BUILDER_T *diffusion_recordv2_builder_init();

/**
 * @brief Adds a new record comprising the specified field values.
 *
 * @param recordv2_builder the builder to modify
 * @param fields           the fields within the new record. If no fields are
 *                         supplied, an empty record will be added.
 */
void diffusion_recordv2_builder_add_record(const DIFFUSION_RECORDV2_BUILDER_T *recordv2_builder, char **fields);

/**
 * @brief Adds a new record comprising the specified field values.
 *
 * @param recordv2_builder the builder to modify
 * @param fields           the fields within the new record. If no fields are
 *                         supplied, an empty record will be added.
 */
void diffusion_recordv2_builder_add_record_with_list(const DIFFUSION_RECORDV2_BUILDER_T *recordv2_builder, LIST_T *fields);

/**
 * @brief Adds one or more field values.
 *
 * If there is a current record, this adds the fields to the end of the
 * current record.
 *
 * @param recordv2_builder the builder to modify
 * @param values           field values
 */
void diffusion_recordv2_builder_add_fields(const DIFFUSION_RECORDV2_BUILDER_T *recordv2_builder, char **values);

/**
 * @brief Adds one or more field values.
 *
 * If there is a current record, this adds the fields to the end of the
 * current record.
 *
 * @param recordv2_builder the builder to modify
 * @param values           field values
 */
void diffusion_recordv2_builder_add_fields_with_list(const DIFFUSION_RECORDV2_BUILDER_T *recordv2_builder, LIST_T *values);

/**
 * @brief Builds a bytes representation of the current builder state.
 *
 * @param recordv2_builder the builder to build
 *
 * @return a bytes representation of the builder.
 */
void *diffusion_recordv2_builder_build(const DIFFUSION_RECORDV2_BUILDER_T *recordv2_builder);

/**
 * @brief Clears all current values from the builder allowing it to be reused to
 * generate new data.
 *
 * @param recordv2_builder the builder to clear
 */
void diffusion_recordv2_builder_clear(DIFFUSION_RECORDV2_BUILDER_T *recordv2_builder);

/**
 * Free a recordv2 builder.
 *
 * @param recordv2_builder builder to be freed.
 */
void diffusion_recordv2_builder_free(DIFFUSION_RECORDV2_BUILDER_T *recordv2_builder);

#endif
