/**
 * @file cbor.h
 *
 * Utilities for handling CBOR-encoded data.
 *
 * Copyright © 2017, 2021 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */

#ifndef _cbor_h_
#define _cbor_h_ 1

#include <stddef.h>
#include <stdint.h>

typedef struct cbor_parser_s {
        const char *data;
        size_t len;
        char *ptr;
        int in_indefinite_block;
} CBOR_PARSER_T;

/**
 * CBOR major types
 */
typedef enum {
        CBOR_TYPE_UNSIGNED_INT = 0,
        CBOR_TYPE_NEGATIVE_INT = 1,
        CBOR_TYPE_BYTE_STRING = 2,
        CBOR_TYPE_TEXT_STRING = 3,
        CBOR_TYPE_ARRAY = 4,
        CBOR_TYPE_MAP = 5,
        CBOR_TYPE_SEMANTIC_TAG = 6,
        CBOR_TYPE_FLOAT = 7
} CBOR_TYPE_T;

typedef union {
        /// Value of a CBOR_TYPE_UNSIGNED_INT
        uint64_t as_uint;
        /// Value of a CBOR_TYPE_NEGATIVE_INT
        int64_t as_negint;
        /// Pointer to bytes for a CBOR_TYPE_BYTE_STRING or
        /// CBOR_TYPE_TEXT_STRING type.
        char *as_bytes;
        /// Pointer to data relevant to a CBOR_TYPE_SEMANTIC_TAG
        void *as_semantic_tag;
        /// Value of a CBOR_TYPE_FLOAT
        double as_float;
} CBOR_VALUE_UNION_T;

/**
 * A CBOR value/token as returned by cbor_next_val().
 */
typedef struct cbor_val_s {
        /// The initial byte of CBOR encoding; contains the major type
        /// (extracted into the field "type") and possibly other information;
        /// useful for comparing known CBOR values such as CBOR_VAL_FALSE.
        unsigned char initial_byte;
        /// The major type of the CBOR token.
        CBOR_TYPE_T type;
        /// Union, value depends on type.
        CBOR_VALUE_UNION_T value;
        /// Length in bytes of CBOR_TYPE_BYTE_STRING and CBOR_TYPE_TEXT_STRING
        /// types, or the number of items in a CBOR_TYPE_ARRAY type, or the
        /// number of item pairs ina CBOR_TYPE_MAP.
        int64_t size;
} CBOR_VAL_T;

/**
 * @brief Create a CBOR parser.
 *
 * @param data	A pointer to CBOR-encoded data. The data is not copied and
 *              must be available during the lifetime of the parser.
 * @param len   Length of the CBOR-encoded data.
 * @returns     A pointer to a CBOR_PARSER_T, or NULL if data is NULL.
 */
CBOR_PARSER_T *cbor_parser_create(const char *data, size_t len);

/**
 * @brief Free memory associated with a CBOR parser.
 *
 * @param parser A pointer to a CBOR_PARSER_T. If NULL, this function
 *               has no effect.
 */
void cbor_parser_free(CBOR_PARSER_T *parser);

int cbor_parser_available_bytes(CBOR_PARSER_T *parser);

/**
 * @brief Get the next available token from a CBOR parser.
 *
 * @param parser A pointer to a CBOR parser.
 * @returns A pointer to a CBOR_VAL_T containing details about the next token,
 *          or NULL if there are no more tokens to read.
 */
CBOR_VAL_T *cbor_next_val(CBOR_PARSER_T *parser);

/**
 * @brief Free memory associated with a CBOR_VAL_T.
 *
 * @param val A pointer to a CBOR_VAL_T structure, or NULL.
 */
void cbor_val_free(CBOR_VAL_T *val);

typedef int(*cbor_jump_fn)(CBOR_PARSER_T *, CBOR_VAL_T *);

#define DECL_PARSER_JUMP_FN(name) int name(CBOR_PARSER_T *parser, CBOR_VAL_T *val);

DECL_PARSER_JUMP_FN(cbor_parse_unimplemented)
DECL_PARSER_JUMP_FN(cbor_parse_reserved)

DECL_PARSER_JUMP_FN(cbor_parse_break)

DECL_PARSER_JUMP_FN(cbor_parse_uint_small)
DECL_PARSER_JUMP_FN(cbor_parse_uint8)
DECL_PARSER_JUMP_FN(cbor_parse_uint16)
DECL_PARSER_JUMP_FN(cbor_parse_uint32)
DECL_PARSER_JUMP_FN(cbor_parse_uint64)

DECL_PARSER_JUMP_FN(cbor_parse_negint_small)
DECL_PARSER_JUMP_FN(cbor_parse_negint8)
DECL_PARSER_JUMP_FN(cbor_parse_negint16)
DECL_PARSER_JUMP_FN(cbor_parse_negint32)
DECL_PARSER_JUMP_FN(cbor_parse_negint64)

DECL_PARSER_JUMP_FN(cbor_parse_byte_string_small)
DECL_PARSER_JUMP_FN(cbor_parse_byte_string_uint8)
DECL_PARSER_JUMP_FN(cbor_parse_byte_string_uint16)
DECL_PARSER_JUMP_FN(cbor_parse_byte_string_uint32)
DECL_PARSER_JUMP_FN(cbor_parse_byte_string_uint64)
DECL_PARSER_JUMP_FN(cbor_parse_byte_string_indefinite)

DECL_PARSER_JUMP_FN(cbor_parse_text_string_small)
DECL_PARSER_JUMP_FN(cbor_parse_text_string_uint8)
DECL_PARSER_JUMP_FN(cbor_parse_text_string_uint16)
DECL_PARSER_JUMP_FN(cbor_parse_text_string_uint32)
DECL_PARSER_JUMP_FN(cbor_parse_text_string_uint64)
DECL_PARSER_JUMP_FN(cbor_parse_text_string_indefinite)

DECL_PARSER_JUMP_FN(cbor_parse_array_small)
DECL_PARSER_JUMP_FN(cbor_parse_array_uint8)
DECL_PARSER_JUMP_FN(cbor_parse_array_uint16)
DECL_PARSER_JUMP_FN(cbor_parse_array_uint32)
DECL_PARSER_JUMP_FN(cbor_parse_array_uint64)
DECL_PARSER_JUMP_FN(cbor_parse_array_indefinite)

DECL_PARSER_JUMP_FN(cbor_parse_map_small)
DECL_PARSER_JUMP_FN(cbor_parse_map_uint8)
DECL_PARSER_JUMP_FN(cbor_parse_map_uint16)
DECL_PARSER_JUMP_FN(cbor_parse_map_uint32)
DECL_PARSER_JUMP_FN(cbor_parse_map_uint64)
DECL_PARSER_JUMP_FN(cbor_parse_map_indefinite)

DECL_PARSER_JUMP_FN(cbor_parse_semantic_tag)

DECL_PARSER_JUMP_FN(cbor_parse_float16)
DECL_PARSER_JUMP_FN(cbor_parse_float32)
DECL_PARSER_JUMP_FN(cbor_parse_float64)

DECL_PARSER_JUMP_FN(cbor_parse_true)
DECL_PARSER_JUMP_FN(cbor_parse_false)
DECL_PARSER_JUMP_FN(cbor_parse_null)
DECL_PARSER_JUMP_FN(cbor_parse_undefined)

/**
 * A CBOR generator is a wrapper for encoded data and the length of that
 * data. Once all items have been written to the generator, the encoded data
 * can be accessed directly from the structure.
 */
typedef struct cbor_generator_s {
        char *data;
        size_t len;
} CBOR_GENERATOR_T;

#define CBOR_VAL_SMALL 23
#define CBOR_VAL8 24
#define CBOR_VAL16 25
#define CBOR_VAL32 26
#define CBOR_VAL64 27

/// Initial byte which maps to a JSON "false" value.
#define CBOR_VAL_FALSE 0xf4
/// Initial byte which maps to a JSON "true" value.
#define CBOR_VAL_TRUE 0xf5
/// Initial byte which maps to a JSON "null" value.
#define CBOR_VAL_NULL 0xf6
/// Initial byte which maps to a JSON "undefined" value.
#define CBOR_VAL_UNDEFINED 0xf7
/// Initial byte indicating the termination of a indefinte-length collection.
#define CBOR_VAL_BREAK 0xff

/**
 * @brief Creates a generator to be used for encoding CBOR data.
 *
 * @retval A new CBOR generator, or NULL on error.
 */
CBOR_GENERATOR_T *cbor_generator_create();

/**
 * @brief Free memory associated with a CBOR generator.
 *
 * @param generator The CBOR generator to be freed.
 */
void cbor_generator_free(CBOR_GENERATOR_T *generator);

/**
 * @brief Encode an unsigned integer value into the smallest possible CBOR
 * encoding.
 *
 * @param generator	The CBOR generator to encode into.
 * @param val           The value to be encoded.
 * @returns 0 on success or -1 on error.
 */
int cbor_write_uint(CBOR_GENERATOR_T *generator, uint64_t val);

/**
 * @brief Encode an negative integer value into the smallest possible CBOR
 * encoding.
 *
 * @param generator	The CBOR generator to encode into.
 * @param val           The value to be encoded.
 * @returns 0 on success or -1 on error.
 */
int cbor_write_negint(CBOR_GENERATOR_T *generator, int64_t val);

/**
 * @brief Encode a stream of bytes as CBOR.
 *
 * A marker denoting an indefinte-length byte string can be written using this
 * function by setting len to -1. It is neccessary to call `cbor_write_break()`
 * once the data items inside a byte string of this type has been written.
 *
 * @param generator     The CBOR generator to encode into.
 * @param bytes         A pointer to the bytes to be encoded.
 * @param len           The length of the bytes to be encoded.
 * @returns 0 on success or -1 on error.
 */
int cbor_write_byte_string(CBOR_GENERATOR_T *generator, const char *bytes, int64_t len);

/**
 * @brief Encode a stream of bytes as CBOR.
 *
 * A marker denoting an indefinte-length text string can be written using this
 * function by setting len to -1. It is neccessary to call `cbor_write_break()`
 * once the data items inside a text string of this type have been written.
 *
 * @param generator     The CBOR generator to encode into.
 * @param bytes         A pointer to the bytes to be encoded.
 * @param len           The length of the bytes to be encoded.
 * @returns 0 on success or -1 on error.
 */
int cbor_write_text_string(CBOR_GENERATOR_T *generator, const char *bytes, int64_t len);

/**
 * @brief Encode an array start marker as CBOR.
 *
 * A marker denoting an indefinte-length array can be written using this
 * function by setting len to -1. It is neccessary to call `cbor_write_break()`
 * once the data items inside an array of this type have been written.
 *
 * @param generator     The CBOR generator to encode into.
 * @param size          The length of the array to be encoded.
 * @returns 0 on success or -1 on error.
 */
int cbor_write_array(CBOR_GENERATOR_T *generator, int64_t size);

/**
 * @brief Encode a map start marker as CBOR.
 *
 * The length of a map is equal to the number of key/value pairs that it
 * contains.
 *
 * A marker denoting an indefinte-length map can be written using this
 * function by setting len to -1. It is neccessary to call `cbor_write_break()`
 * once the data items inside a map of this type have been written.
 *
 * @param generator     The CBOR generator to encode into.
 * @param size          The length of the map to be encoded.
 * @returns 0 on success or -1 on error.
 */
int cbor_write_map(CBOR_GENERATOR_T *generator, int64_t size);

/**
 * @brief Encode a floating point value in CBOR format. Automatically detects
 * the smallest possible encoding to use.
 *
 * @param generator	The CBOR generator to which the encoded value is
 *                      appended.
 * @param val		The value to be encoded.
 * @returns 0 on success, or -1 on error.
 */
int cbor_write_float(CBOR_GENERATOR_T *generator, double val);

/**
 * @brief Encode a "break" value in CBOR format, for terminating
 * indefinite-length CBOR collections.
 *
 * @param generator     The CBOR generator to which the encoded value is
 *                      appended.
 * @returns 0 on success, or -1 on error.
 */
int cbor_write_break(CBOR_GENERATOR_T *generator);

/**
 * @brief Encode a "false" value in CBOR format.
 *
 * @param generator     The CBOR generator to which the encoded value is
 *                      appended.
 * @returns 0 on success, or -1 on error.
 */
int cbor_write_false(CBOR_GENERATOR_T *generator);

/**
 * @brief Encode a "true" value in CBOR format.
 *
 * @param generator     The CBOR generator to which the encoded value is
 *                      appended.
 * @returns 0 on success, or -1 on error.
 */
int cbor_write_true(CBOR_GENERATOR_T *generator);

/**
 * @brief Encode a "null" value in CBOR format.
 *
 * @param generator     The CBOR generator to which the encoded value is
 *                      appended.
 * @returns 0 on success, or -1 on error.
 */
int cbor_write_null(CBOR_GENERATOR_T *generator);

/**
 * @brief Encode an "undefined" value in CBOR format.
 *
 * @param generator     The CBOR generator to which the encoded value is
 *                      appended.
 * @returns 0 on success, or -1 on error.
 */
int cbor_write_undefined(CBOR_GENERATOR_T *generator);


#endif
