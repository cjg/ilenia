/* repository.h */

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

#ifndef _REPOSITORY_H
#define _REPOSITORY_H
#include "dict.h"
#include "list.h"

typedef struct {
	char *name;
	char *path;
	char *supfile;
	void *driver;
	int priority;
} repository_t;

repository_t *repository_new(char *name, char *path, char *supfile,
			     void *driver, list_t * repositories_hierarchy);
dict_t *repositories_dict_init(list_t * drivers,
			       list_t * repositories_hierarchy);
void repositories_dict_update(dict_t * self, list_t * repositories_name);
void repositories_dict_update_all(dict_t * self);
void repositories_dict_dump(dict_t * self);
void repository_dump(repository_t * self);
void repository_free(repository_t * self);

#endif
