/**
 * @file buf.h
 *
 * Defines structures and functions for working with byte arrays of arbitrary
 * length. These are mostly used in the serialisation and deserialisation of
 * messages between the client and Diffusion, but are useful in a range of
 * other situations.
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
#ifndef _diffusion_buf_h_
#define _diffusion_buf_h_ 1

#include <limits.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

#include "list.h"

/**
 * A buffer for holding arbitrarily terminated byte arrays. Functions are
 * available (see (@ref buf.h)) to manipulate the byte array.
 */
typedef struct buf_s {
        /// Bytes contained in this buffer.
        char *data;
        /// Length in bytes used in this buffer.
        size_t len;
        /// Length in bytes of total space in this buffer
        size_t available;
} BUF_T;

/**
 * Allocate memory for a new buffer.
 *
 * @retval "BUF_T *"	A pointer to a new buffer.
 * @retval NULL		If the buffer cannot be created.
 */
BUF_T *buf_create(void);

/**
 * Free memory in the buffer, and the buffer itself.
 *
 * @param buf		The buffer to be freed.
 */
void buf_free(BUF_T *buf);

/**
 * A utility function for displaying the contents of a buffer to stdout, in
 * hexadecimal format.
 *
 * @param buf		The buffer to display.
 */
void hexdump_buf(BUF_T *buf);

/**
 * Creates a deep copy of an existing buffer. `buf_free` should be called
 * on the pointer when no longer needed.
 *
 * @param src		The buffer to copy.
 * @retval "BUF_t *"	A newly allocated buffer.
 * @retval NULL		If the new buffer cannot be created.
 */
BUF_T *buf_dup(const BUF_T *src);

/**
 * Appends an unsigned 64-bit integer to the buffer in Diffusion's packeder
 * integer format.
 *
 * @param buf		The buffer to write to.
 * @param val		A 64-bit unsigned integer.
 */
void buf_write_uint64_enc(BUF_T *buf, const uint64_t val);

/**
 * Appends an unsigned 32-bit integer to the buffer in Diffusion's packed
 * integer format.
 *
 * @param buf		The buffer to write to.
 * @param val		A 32-bit unsigned integer.
 */
void buf_write_uint32_enc(BUF_T *buf, const uint32_t val);

/**
 * Appends a float (IEEE-754 encoded) to the buffer.
 *
 * @param buf		The buffer to write to.
 * @param val		The float to be appended to the buffer.
 */
void buf_write_float(BUF_T *buf, const float val);

/**
 * Append a NULL-terminated string to the buffer.
 *
 * @param buf		The buffer to write to.
 * @param str		The NULL-terminated string to be appended to the buffer.
 */
void buf_write_string(BUF_T *buf, const char *str);

/**
 * Append a length-encoded string to the buffer, where the string is NULL-
 * terminated.
 *
 * @param buf		The buffer to write to.
 * @param str		The NULL-terminated string to be appended to the buffer.
 */
void buf_write_string_enc(BUF_T *buf, const char *str);

/**
 * Append a length-encoded string to the buffer.
 *
 * @param buf		The buffer to write to.
 * @param str		The string to be appended to the buffer.
 * @param len		The length of the string to write.
 */
void buf_write_string_length_enc(BUF_T *buf, const char *str, const uint64_t len);

/**
 * Appends a single byte to the buffer.
 *
 * @param buf		The buffer to write to.
 * @param b		The byte to write.
 */
void buf_write_byte(BUF_T *buf, const unsigned char b);

/**
 * Appends an array of bytes to the buffer.
 *
 * @param buf		The buffer to write to.
 * @param bytes		The bytes to be appended to the buffer.
 * @param len		The length of the byte array to be written.
 */
void buf_write_bytes(BUF_T *buf, const void *bytes, const size_t len);

/**
 * Appends a list to the buffer.
 *
 * @param buf           The buffer to write to.
 * @param list          The list to be appended to the buffer.
 * @param serialise_fn  The serialiser function for the list elements.
 */
void buf_write_list(BUF_T *buf, const LIST_T *list, void(*serialise_fn)(BUF_T *buf, void *));

/**
 * Concatenates two buffers.
 *
 * @param dst		The buffer to append to.
 * @param src		The buffer containing data to append.
 */
void buf_write_buf(BUF_T *dst, const BUF_T *src);

/**
 * Write a boolean value into a `BUF_T`
 *
 * @param buf The buffer to write into
 * @param val The boolean value to write
 */
void buf_write_bool(BUF_T *buf, bool val);

/**
 * Safely write sprintf-style to a buffer.
 *
 * @param dst		The buffer to append to.
 * @param format	The printf format string.
 * @param ...		Arguments.
 * @return		The number of bytes written, or < 0 on error.
 */
int buf_sprintf(BUF_T *dst, const char *format, ...);

/**
 * Read a byte from a char array.
 *
 * @param data		The source char array.
 * @param val		The location in which to store the byte value.
 * @return		The address in the source char array following the
 *			byte which has been read.
 */
char *buf_read_byte(const char *data, unsigned char *val);


/**
 * Read an unencoded uint32_t from a char array.
 *
 * @param data		The source char array.
 * @param val		The location in which to store the int32.
 * @return		The address in the source char array following the
 *			int32 which has been read.
 */
char *buf_read_uint32(const char *data, uint32_t *val);

/**
 * Read an unencoded uint64_t from a char array.
 *
 * @param data		The source char array.
 * @param val		The location in which to store the int64.
 * @return		The address in the source char array following the
 *			int64 which has been read.
 */
char *buf_read_uint64(const char *data, uint64_t *val);

/**
 * Read a float (IEEE754 encoded) from a char array.
 *
 * @param data		The source char array.
 * @param val		The location in which to store the float.
 * @return		The address in the source char array following the
 *			float which has been read.
 */
char *buf_read_float(const char *data, float *val);

/**
 * Reads a int32 encoded in Diffusion's packed integer format from the char
 * array.
 *
 * @param data		The source char array.
 * @param val		The location in which to store the int32 value.
                Is left unset if `data` is NULL or empty.
 * @return		The address in the source char array following the
 *			int32 which has been read.
 */
char *buf_read_uint32_enc(const char *data, uint32_t *val);

/**
 * Reads a int64 encoded in Diffusion's packed integer format from the char
 * array.
 *
 * @param data		The source char array.
 * @param val		The location in which to store the int64 value.
 * @return		The address in the source char array following the
 *			int64 which has been read.
 */
char *buf_read_uint64_enc(const char *data, uint64_t *val);

/**
 * Reads a length-encoded string from the char array.
 *
 * @param data		The source char array.
 * @param dst_str	A pointer to a location in which to the string is
 *			stored once it has been read. This memory is allocated
 *			by the API, but the user should free() it once it is
 *			no longer required.
 * @param len		The length of the string which has been read. Can be NULL.
 * @return		The address in the source char array following the
 *			string which has been read.
 */
char *buf_read_string_length_enc(const char *data, char **dst_str, size_t *len);

/**
 * Read a boolean value
 *
 * @param data pointer to read from
 * @param val  boolean pointer to set
 */
char *buf_read_bool(char *data, bool *val);

/**
 * @brief Returns the contents of the buffer as a NULL terminated string.
 *
 * Be aware that if the buffer contains NULL characters, then the
 * returned string will too.
 *
 * It is the caller's responsibility to free() the memory returned by
 * this function.
 *
 * @param buf           The buffer.
 * @return              A pointer to a NULL-terminated string.
 */
char *buf_as_string(const BUF_T *buf);

/**
 * @brief Returns the contents of the buffer as a NULL terminated string of
 * hex digits.
 *
 * It is the caller's responsibility to free() the memory returned by this
 * function.
 *
 * @param buf           The buffer.
 * @return              A pointer to a NULL-terminated string.
 */
char *buf_as_hex(const BUF_T *buf);

/**
 * @brief Returns a pointer to the underlying bytes of the BUF_T.
 *
 * If the requested range of bytes is outside those contained within the
 * buffer, NULL is returned. No memory is copied in this function; it is
 * incorrect to free() the returned pointer.
 *
 * @param buf           The buffer.
 * @param offset        The offset into the buffer.
 * @param length        The length of the requested substr, or -1 for all
 *                      remaining bytes.
 * @return              A pointer to the offset within the underlying
 *                      bytes, or NULL on error. It is not guaranteed to
 *                      be a NULL-terminated series of bytes.
 */
char *buf_substr(const BUF_T *buf, int offset, int length);


/**
 * @brief Compare two buffers for equivalence.
 *
 * This function considers a NULL buffer to be equivalent to an empty buffer
 * (size 0).
 *
 * @param a             A pointer to a buffer
 * @param b             A pointer to a buffer
 * @retval 0 if both buffers have the same length and contents.
 * @retval -2 if the buffers differ in length
 * @retval -1 if the buffers are the same length, but a is lexicographically
 * less than b.
 * @retval 1 if the buffers are the same length, but a is lexicographically
 * greater than b.
 *
 */
int buf_cmp(const BUF_T *a, const BUF_T *b);

/**
 * @brief Discard bytes from the front of a buffer.
 *
 * Removes bytes_to_remove bytes from the front of a buffer and
 * discards them. The buffer is shrunk and holds the bytes after
 * those which were discarded.
 *
 * @param buf           The buffer.
 * @param bytes_to_remove The number of bytes to be discarded.
 */
void buf_discard_front(BUF_T *buf, const int bytes_to_remove);

#endif
