/**
 * @file content_types.h
 *
 * Structures representing content types (e.g., used for updating
 * topics).
 *
 * Copyright © 2014, 2021 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
         */
#ifndef _diffusion_content_types_h_
#define _diffusion_content_types_h_ 1

#include "buf.h"
#include "misc/deprecate.h"

/**
 * The content encoding. Currently only NONE is supported.
 */
typedef enum {
	CONTENT_ENCODING_NONE = 0,
	CONTENT_ENCODING_ENCRYPTED,
	CONTENT_ENCODING_COMPRESSED
} CONTENT_ENCODING_T;

/**
 * Content wrapper for encoding and data.
 */
typedef struct content_s {
        /// The content encoding.
	CONTENT_ENCODING_T encoding;
        /// The content itself.
	BUF_T *data;
} CONTENT_T;

/**
 * @deprecated This function is deprecated from version 6.1 onwards, and may
 * be removed.
 *
 * Free all memory associated with a CONTENT_T.
 *
 * @param content	The CONTENT_T to be freed.
 */
DEPRECATED(void content_free(CONTENT_T *content))

#endif
