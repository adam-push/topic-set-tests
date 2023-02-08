/**
 * @file hash_num.h
 *
 * A simple hash with an numerical key implementation.
 *
 * Copyright © 2020, 2021 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */

#ifndef _diffusion_hash_num_h_
#define _diffusion_hash_num_h_ 1

#include <apr_portable.h>

/**
 * This structure represents an entry within a hash table.
 */
typedef struct hash_num_entry_s {
        /// The key for this hash entry.
        unsigned long key;

        /// The value associated with this hash entry.
        void *val;

        /// If multiple keys have the same hash, this is a pointer to
        /// the next possible matching entry.
        struct hash_num_entry_s *next;
} HASH_NUM_ENTRY_T;

/**
 * This represents a hash table.
 */
typedef struct hash_num_s {
        /// The maximum number of available slots in this hash table.
        unsigned long slots;

        /// An array of entries assigned to slots.
        HASH_NUM_ENTRY_T **entries;

        /// The number of items held by this hash table.
        unsigned long size;

        /// Mutex protecting hash modification/access.
        apr_pool_t *pool;
        apr_thread_mutex_t *mutex;
} HASH_NUM_T;

/**
 * Create a new hash with numerical keys
 *
 * @param slots         The number of slots available in the hashmap. Keys hash to a slot,
 *                      and if a slot already contains a key which yields the same hash,
 *                      it is chained to other entries in the bucket.
 *
 * @retval "HASH_NUM_T *" Returns a pointer to a HASH_NUM_T structure.
 * @retval NULL                 If the hash cannot be created.
 */
HASH_NUM_T *hash_num_new(const unsigned long slots);


/**
 * Create a new unsynchronized hash with numerical keys
 *
 * @param slots         The number of slots available in the hashmap. Keys hash to a slot,
 *                      and if a slot already contains a key which yields the same hash,
 *                      it is chained to other entries in the bucket.
 *
 * @retval "HASH_NUM_T *" Returns a pointer to a HASH_NUM_T structure.
 * @retval NULL                 If the hash cannot be created.
 */
HASH_NUM_T *unsync_hash_num_new(const unsigned long slots);


/**
 * @brief Clears all keys and values from a hash.
 *
 * Frees all the keys and values in a HASH_NUM_T, but does not free
 * the hash itself.
 *
 * @param hash    The hash to be freed.
 * @param val_free_fn   A function to be used to free memory associated with the value, or
 *                      NULL if the values should not be freed.
 */
void hash_num_clear(HASH_NUM_T *hash, void(*val_free_fn)(void *));

/**
 * @brief Frees memory associated with a hash.
 *
 * This function can free all memory associated with a hash.
 *
 * @param hash          The hash to be freed.
 * @param val_free_fn   A function to be used to free memory associated with the value, or
 *                      NULL if the values should not be freed.
 */
void hash_num_free(HASH_NUM_T *hash, void(*val_free_fn)(void *));

/**
 * Add a value to a hash with the given key.
 *
 * If the hash already contains the key, the value is replaced and
 * the old value returned. In this case, the key in the hash is
 * reused; you may need to free() the key that was passed.
 *
 * @param hash    The hash to which the key/value pair will be added.
 * @param key           The key under which to store the value.
 * @param val           The value stored under the key.
 * @retval "void *"     If the key already exists in the hash the previous value is returned.
 * @retval NULL         If the key cannot be found in the hash.
 */
void *hash_num_add(HASH_NUM_T *hash, unsigned long key, const void *val);

/**
 * Remove a value from the hashmap
 *
 * @param hash    The hash from which the key/value pair will be removed.
 * @param key           The key for the entry which is to be removed.
 * @retval "void *"     The value which was removed.
 * @retval NULL         If the key was not found.
 */
void *hash_num_del(HASH_NUM_T *hash, unsigned long key);

/**
 * Get a value from the hashmap
 *
 * @param hash    The hash to be searched for the key.
 * @param key           The key for which the value is to be returned.
 * @retval "void *"     The value in the hashmap associated with the key, or NULL if not found.
 * @retval NULL         If the key was not found.
 */
void *hash_num_get(const HASH_NUM_T *hash, unsigned long key);

/**
 * Obtains all keys currently in the hashmap.
 *
 * @param hash    The hash to be inspected.
 *
 * @return              A array of all keys in the hashmap.
 *                      Call free() on the array when it is no longer required;
 */
unsigned long *hash_num_keys(const HASH_NUM_T *hash);

/**
 * @brief Create a deep copy of a hash
 * `hash_num_free` should be called on the pointer when no longer needed.
 *
 * @param src          The hash to copy.
 * @param fn           The function used to copy the hash value.
 *
 * @return             A copy of the hashmap.
 */
HASH_NUM_T *hash_num_dup(const HASH_NUM_T *src, void *(*fn)());

/**
 * Create a deep copy of a hash, assuming that the values are NULL-terminated strings.
 *
 * @param src           The hash to copy.
 * @return              A copy of the hashmap.
 */
HASH_NUM_T *hash_num_dup_strval(const HASH_NUM_T *src);

#endif
