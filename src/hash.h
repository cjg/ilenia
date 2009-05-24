/* hash.h */

/* ilenia -- A package manager for CRUX
 *
 * Copyright (C) 2006 - 2007
 *     Giuseppe Coviello <cjg@cruxppc.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _HASH_H
#define _HASH_H

#include "dict.h"

typedef struct {
	unsigned length;
	dict_t **elements;
	unsigned size;
} hash_t;

typedef struct {
	hash_t *hash;
	unsigned current_dict;
	unsigned current_position;
	short first;
} hashiterator_t;

/**
 * hash_new: 
 * 
 * Creates a new hash with default size.
 *
 * Returns: a pointer the the new hash.
 */
hash_t *hash_new(void);

hash_t *hash_new_with_size(unsigned size);

/**
 * dict_free:
 * @self: the dictionary to delete.
 * @data_free: the pointer to the function that should be used to free the
 * memory used by the content of the dictionary, it could be a %NULL pointer.
 *
 * Deletes a dictionary freeing the memory used by the dictionary itself.
 */
#define hash_free(self, data_free) hash_free((self), \
					     (void (*)(void *)) (data_free))

/**
 * dict_add:
 * @self: the dictionary where to insert the entry.
 * @key: the key associated to the entry.
 * @data: the entry data to insert.
 *
 * Inserts the entry @data with the key @key in the dictionary @self. The
 * key must be not %NULL.
 *
 * Returns: the dictionary self.
 */
hash_t *hash_add(hash_t * self, char *key, void *data);

/**
 * dict_get:
 * @self: the dictionary where is stored the entry.
 * @key: the key associated to the entry.
 *
 * Gets an entry from a dictionary.
 *
 * Returns: a pointer to the entry, or a %NULL pointer if the key isn't found
 * in the dictionary.
 */
void *hash_get(hash_t * self, char *key);

/**
 * dict_remove:
 * @self: the dictionary where is stored the entry.
 * @key: the key associated to the entry.
 * @data_free: the pointer to the function that should be used
 * to free the memory used by the content of the dictionary, it could be a
 * %NULL pointer.
 *
 * Removes an entry from a dictionary.
 *
 * Returns: the dictionary self.
 */
hash_t *hash_remove(hash_t * self, char *key, void data_free(void *));

/**
 * dict_dump:
 * @self: the dictionary to dump.
 * @data_dump: the pointer to the function used to print the %data
 * field.
 *
 * Prints to the stdout the content of a dictionary.
 */
#define hash_dump(self, data_dump) hash_dump((self), \
                                             (void (*)(void *)) (data_dump))

hashiterator_t *hashiterator_new(hash_t *hash);
void *hashiterator_get(hashiterator_t *self);
int hashiterator_next(hashiterator_t * self);
#define hashiterator_free(self) free((self))

#endif
