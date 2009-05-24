/* hash.c */

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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include "memory.h"
#include "hash.h"

#undef hash_dump
#undef hash_free

#define DEFAULT_SIZE 127

static unsigned universal_hash(char *v, unsigned M) {
	int h, a = 31415, b = 27183;
	for(h = 0; *v != 0; v++, a = a * b % (M - 1))
		h = (a * h + *v) % M;
	return (unsigned) ((h < 0) ? (int)(h + M) : h);
}

hash_t *hash_new(void)
{
	return hash_new_with_size(DEFAULT_SIZE);
}

hash_t *hash_new_with_size(unsigned size)
{
	hash_t *self;
	self = xmalloc(sizeof(hash_t));
	self->length = 0;
	self->size = size;
	self->elements = xmalloc(self->size * sizeof(dict_t *));
	memset(self->elements, 0, self->size * sizeof(dict_t *));
	return self;
}

void hash_free(hash_t * self, void data_free(void *))
{
	unsigned i;

	assert(self != NULL);

	for (i = 0; i < self->size; i++)
		if(self->elements[i] != NULL)
			dict_free(self->elements[i], data_free);

	free(self->elements);
	free(self);
}

hash_t *hash_add(hash_t * self, char *key, void *data)
{
	unsigned position;

	assert(self != NULL && key != NULL);

	position = universal_hash(key, self->size);
	
	if(self->elements[position] == NULL)
		self->elements[position] = dict_new();

	dict_add(self->elements[position], key, data);

	return self;
}

void *hash_get(hash_t * self, char *key)
{
	unsigned position;

	assert(self != NULL && key != NULL);

	position = universal_hash(key, self->size);

	if(self->elements[position] != NULL)
		return dict_get(self->elements[position], key);

	return NULL;
}

hash_t *hash_remove(hash_t * self, char *key, void data_free(void *))
{
	unsigned position;

	assert(self != NULL && key != NULL);

	position = universal_hash(key, self->size);

	if(self->elements[position] != NULL)
		dict_remove(self->elements[position], key, data_free);

	return self;
}

void hash_dump(hash_t * self, void data_dump(void *))
{
	unsigned i;
	assert(self != NULL && data_dump != NULL);
	for (i = 0; i < self->size; i++) {
		printf("dict[%u]: ", i);
		if(self->elements[i] == NULL)
			printf("NULL\n");
		else
			dict_dump(self->elements[i], data_dump);
	}
}

static inline int get_first(hash_t *hash, unsigned offset_dict, 
			    unsigned offset_position, unsigned *current_dict, 
			    unsigned *current_position)
{
	for(*current_dict = offset_dict; *current_dict < hash->size;
	    (*current_dict)++) {
		*current_position = offset_position;
		if(hash->elements[*current_dict] != NULL
		   && *current_position < hash->elements[*current_dict]->length)
			return 1;
		offset_position = 0;
	}

	return 0;
}

hashiterator_t *hashiterator_new(hash_t *hash)
{
	hashiterator_t *self;

	assert(hash != NULL);
	self = xmalloc(sizeof(hashiterator_t));
	self->hash = hash;
/* 	get_first(self->hash, 0, 0, &self->current_dict,  */
/* 		  &self->current_position); */
	self->first = -1;
	self->current_dict = 0;
	self->current_position = 0;
	return self;
}

void *hashiterator_get(hashiterator_t *self)
{
	if(self->hash->elements[self->current_dict] != NULL
	   && self->current_position <
	   self->hash->elements[self->current_dict]->length)
		return self->hash->elements[self->current_dict]->elements[self->current_position]->value;
	return NULL;
}

int hashiterator_next(hashiterator_t * self) 
{
	int status; 
	
	status = get_first(self->hash, self->current_dict, 
			   self->current_position + 1 + self->first, 
			   &self->current_dict, &self->current_position);

	self->first = 0;

	return status;
}
