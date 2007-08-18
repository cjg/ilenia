/* list.c */

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
#include <string.h>
#include "list.h"
#include "memory.h"
#include "str.h"

#undef list_free
#undef list_query
#undef list_get_position
#undef list_dump
#undef list_xstrdup


#define BLOCKSIZE 512
#define ELEMENTS_PER_BLOCK (BLOCKSIZE / sizeof(void *))


list_t *list_new(void)
{
	list_t *self;
	self = xmalloc(sizeof(list_t));
	self->length = 0;
	self->size = ELEMENTS_PER_BLOCK;
	self->elements = xmalloc(self->size * sizeof(void *));
	return self;
}

list_t *list_new_from_array(void **array, int array_len)
{
	int i;
	list_t *self;
	assert(array != NULL || array_len == 0);
	self = list_new();
	for (i = 0; i < array_len; i++) {
		list_append(self, array[i]);
	}
	return self;
}

void list_free(list_t * self, void data_free(void *))
{
	unsigned i;
	assert(self);
	for (i = 0; data_free && i < self->length; i++)
		data_free(self->elements[i]);
	free(self->elements);
	free(self);
}

list_t *list_append(list_t * self, void *data)
{
	assert(self);
	self->length++;
	if (self->length >= self->size) {
		self->size += ELEMENTS_PER_BLOCK;
		self->elements = xrealloc(self->elements,
					  self->size * sizeof(void *));
	}
	self->elements[self->length - 1] = data;
	return self;
}

list_t *list_prepend(list_t * self, void *data)
{
	assert(self);
	self->length++;
	if (self->length >= self->size) {
		self->size += ELEMENTS_PER_BLOCK;
		self->elements = xrealloc(self->elements,
					  self->size * sizeof(void *));
	}
	bcopy(self->elements, self->elements + 1, (self->length - 1) *
	      sizeof(void *));
	self->elements[0] = data;
	return self;
}

list_t *list_insert(list_t * self, int position, void *data)
{
	assert(self);

	if (position < 0 && (position = self->length + position) < 0)
		return NULL;

	if ((unsigned)position > self->length)
		position = self->length;

	self->length++;

/* 	if (self->length >= self->size) { */
		self->size += ELEMENTS_PER_BLOCK;
		self->elements = xrealloc(self->elements,
					  self->size * sizeof(void *));
/* 	} */

	bcopy(self->elements + position, self->elements + position + 1,
	      (self->length - position - 1) * sizeof(void *));

	self->elements[position] = data;

	return self;
}

list_t *list_remove(list_t * self, int position, void data_free(void *))
{
	assert(self);

	if (position < 0 && (position = self->length - 1 + position) < 0)
		return NULL;

	if ((unsigned)position >= self->length)
		position = self->length - 1;

	self->length--;

	if (data_free)
		data_free(self->elements[position]);

	memmove(self->elements + position, self->elements + position + 1,
		(self->length - position) * sizeof(void *));
	memset(self->elements + self->length, 0, sizeof(void *));

	return self;
}

void *list_get(list_t * self, int position)
{
	assert(self);

	if (position < 0 && (position = self->length - 1 + position) < 0)
		return NULL;

	if ((unsigned)position >= self->length)
		position = self->length - 1;

	return self->elements[position];
}

int
list_get_position(list_t * self, void *data_query(void *, void *),
		  void *data_query_arg)
{
	unsigned i;
	assert(self && data_query);
	for (i = 0; i < self->length; i++)
		if (data_query(self->elements[i], data_query_arg))
			return i;
	return -1;
}

void list_dump(list_t * self, void data_dump(void *))
{
	unsigned i;
	assert(self && data_dump);
	for (i = 0; i < self->length; i++)
		data_dump(self->elements[i]);
}

list_t *list_query(list_t * self, void *data_query(void *, void *),
		   void *data_query_arg)
{
	list_t *result;
	unsigned i;
	void *element;

	assert(self && data_query);

	result = list_new();

	for (i = 0; i < self->length; i++)
		if ((element = data_query(self->elements[i], data_query_arg)))
			list_append(result, element);

	return result;
}

list_t *list_swap(list_t * self, unsigned pos1, unsigned pos2)
{
	void *tmp;
	assert(self != NULL && pos1 < self->length && pos2 < self->length);

	tmp = self->elements[pos2];
	self->elements[pos2] = self->elements[pos1];
	self->elements[pos1] = tmp;
	return self;
}

list_t *list_reverse(list_t * self)
{
	unsigned i;
	assert(self);
	for (i = 0; i < self->length / 2; i++)
		list_swap(self, i, self->length - 1 - i);
	return self;
}

char *list_xstrdup(list_t *self, const char *sep, char *data_str(void *))
{
	unsigned i;
	char *s, *data;
	assert(self != NULL);

	s = xstrdup("");

	for(i = 0; i < self->length; i++) {
		if(data_str != NULL)
			data = data_str(*(self->elements + i));
		else
			data = xstrdup_printf("%p", *(self->elements + i));

		strappend(&s, data);

		free(data);

		if(sep != NULL && (i + 1) < self->length)
			strappend(&s, sep);
	}

	return s;
}
