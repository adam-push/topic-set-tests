#ifndef _diffusion_datatypes_h_
#define _diffusion_datatypes_h_ 1

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
 * @file datatypes.h
 *
 * Enumeration of Diffusion datatypes.
 */
#include "datatypes/diffusion-binary.h"
#include "datatypes/diffusion-json.h"
#include "datatypes/diffusion-string.h"
#include "datatypes/diffusion-int64.h"
#include "datatypes/diffusion-double.h"
#include "datatypes/diffusion-recordv2.h"
#include "datatypes/recordv2/diffusion-recordv2-builder.h"
#include "datatypes/recordv2/diffusion-recordv2-schema-builder.h"
#include "datatypes/recordv2/diffusion-recordv2-schema.h"
#include "datatypes/recordv2/diffusion-recordv2-record-model.h"
#include "datatypes/recordv2/diffusion-recordv2-mutable-record-model.h"
#include "datatypes/diffusion-value.h"

/**
 * Supported datatypes.
 */
typedef enum {
        /**
         * Binary datatype enum
         */
        DATATYPE_BINARY = 0,
        /**
         * JSON datatype enum
         */
        DATATYPE_JSON = 1,
        /**
         * String datatype enum
         */
        DATATYPE_STRING = 2,
        /**
         * Double datatype enum
         */
        DATATYPE_DOUBLE = 3,
        /**
         * Int64 datatype enum
         */
        DATATYPE_INT64 = 4,
        /**
         * RecordV2 datatype enum
         */
        DATATYPE_RECORDV2 = 5
} DIFFUSION_DATATYPE;

#endif
