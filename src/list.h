/* list.h */

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

#ifndef _LIST_H
#define _LIST_H

typedef struct {
	unsigned length;
	void **elements;
	unsigned size;
} list_t;

list_t *list_new(void);
list_t *list_new_from_array(void **array, int array_len);
void list_free(list_t * self, void data_free(void *));
list_t *list_append(list_t * self, void *data);
list_t *list_prepend(list_t * self, void *data);
list_t *list_insert(list_t * self, int position, void *data);
list_t *list_remove(list_t * self, int position, void data_free(void *));
void *list_get(list_t * self, int position);
int list_get_position(list_t * self, void *data_query(void *, void *),
		      void *data_query_arg);
void list_dump(list_t * self, void data_dump(void *));
list_t *list_query(list_t * self, void *data_query(void *, void *),
		   void *data_query_arg);
list_t *list_swap(list_t * self, unsigned position1, unsigned position2);
list_t *list_reverse(list_t * self);
char *list_xstrdup(list_t *self, const char * sep, char *data_str(void *));
#define list_free(self, data_free) list_free((self), \
					     (void (*)(void *)) (data_free))
#define list_query(self, query, arg) list_query((self), \
						(void *(*)(void *, void*)) (query), \
						(void *) (arg))
#define list_get_position(self, query, arg) list_get_position((self), \
						(void *(*)(void *, void*)) (query), \
						(void *) (arg))
#define list_dump(self, data_dump) list_dump((self), (void (*)(void *))(data_dump))
#define list_xstrdup(self, sep, data_str) list_xstrdup((self), (sep), (char \
								       *(*)(void *))(data_str))
#endif
