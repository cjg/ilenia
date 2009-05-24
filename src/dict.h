/* dict.h */

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

#ifndef _DICT_H
#define _DICT_H

typedef struct {
	char *key;
	void *value;
	unsigned hash;
} element_t;

typedef struct {
	unsigned length;
	element_t **elements;
	unsigned size;
	unsigned not_sorted;
} dict_t;

dict_t *dict_new(void);
void dict_free(dict_t * self, void data_free(void *));
dict_t *dict_add(dict_t * self, char *key, void *data);
void *dict_get(dict_t * self, char *key);
dict_t *dict_remove(dict_t * self, char *key, void data_free(void *));
void dict_dump(dict_t * self, void data_dump(void *));
#define dict_free(self, data_free) dict_free((self), \
					     (void (*)(void *)) (data_free))

#endif
