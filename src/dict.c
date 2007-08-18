/* dict.c */

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
#include "dict.h"
#include "memory.h"

#undef dict_free

#define BLOCKSIZE 512
#define ELEMENTS_PER_BLOCK (BLOCKSIZE / sizeof(element_t *))

static element_t *element_new(char *key, void *data)
{
	element_t *self;
	assert(key);
	self = xmalloc(sizeof(element_t));
	self->key = xstrdup(key);
	self->value = data;
	return self;
}

static void element_free(element_t * self, void data_free(void *))
{
	assert(self);
	free(self->key);
	if (data_free)
		data_free(self->value);
	free(self);
}

static void element_dump(element_t * self, void data_dump(void *))
{
	assert(self && data_dump);
	printf("%s => ", self->key);
	data_dump(self->value);
}

static element_t *dict_element_find(dict_t * self, char *key)
{
	int low, high, mid, cmp;
	mid = low = 0;
	high = self->length;
	while (low < high) {
		mid = (low + high) / 2;
		if ((cmp = strcmp(self->elements[mid]->key, key)) < 0)
			low = mid + 1;
		else if (cmp > 0)
			high = mid;
		else
			return self->elements[mid];
	}
	return NULL;
}

static int dict_get_inner_position(dict_t * self, char *key)
{
	unsigned i;
	for (i = 0; i < self->length; i++)
		if (strcmp(self->elements[i]->key, key) > 0)
			return i;
	return self->length;
}

dict_t *dict_new(void)
{
	dict_t *self;
	self = xmalloc(sizeof(dict_t));
	self->length = 0;
	self->size = ELEMENTS_PER_BLOCK;
	self->elements = xmalloc(self->size * sizeof(element_t *));
	return self;
}

void dict_free(dict_t * self, void data_free(void *))
{
	unsigned i;
	assert(self);
	for (i = 0; i < self->length; i++)
		element_free(self->elements[i], data_free);
	free(self->elements);
	free(self);
}

dict_t *dict_add(dict_t * self, char *key, void *data)
{
	element_t *element;
	int position;

	assert(self);

	if (self->length && (element = dict_element_find(self, key))) {
		element->value = data;
		return self;
	}

	element = element_new(key, data);
	position = self->length == 0 ? 0 : dict_get_inner_position(self, key);
	self->length++;
	if (self->length >= self->size) {
		self->size += ELEMENTS_PER_BLOCK;
		self->elements = xrealloc(self->elements,
					  self->size * sizeof(element_t *));
	}
	bcopy(self->elements + position, self->elements + position + 1,
	      (self->length - position - 1) * sizeof(element_t *));
	self->elements[position] = element;

	return self;
}

void *dict_get(dict_t * self, char *key)
{
	element_t *element;
	assert(self);
	if (!self->length)
		return NULL;
	if ((element = dict_element_find(self, key)))
		return element->value;

	return NULL;
}

dict_t *dict_remove(dict_t * self, char *key, void data_free(void *))
{
	element_t *element;
	assert(self);
	if ((element = dict_element_find(self, key)))
		element_free(element, data_free);
	self->length--;
	return self;
}

void dict_dump(dict_t * self, void data_dump(void *))
{
	unsigned i;
	assert(self && data_dump);
	for (i = 0; i < self->length; i++)
		element_dump(self->elements[i], data_dump);
}
