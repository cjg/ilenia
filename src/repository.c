/* repository.c */

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
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "output.h"
#include "memory.h"
#include "driver.h"
#include "repository.h"
#include "cache.h"
#include "file.h"

static int get_priority(char *name, list_t * repositories_hierarchy)
{
	int priority;
	assert(name);

	priority = repositories_hierarchy->length;
	while (repositories_hierarchy->length && priority >= 0) {
		if (!strcmp(name, (char *)list_get(repositories_hierarchy,
						   repositories_hierarchy->
						   length - priority)))
			break;
		priority--;
	}
	return priority;
}

static void repository_update(repository_t * self)
{
	driver_t *driver;
	driver = self->driver;
	cache_update_stamp();
	driver->update(self);
}

repository_t *repository_new(char *name, char *path, char *supfile,
			     void *driver, list_t * repositories_hierarchy)
{
	repository_t *self;
	assert(name && path && supfile);

	self = xmalloc(sizeof(repository_t));
	self->name = name;
	self->path = path;
	self->supfile = supfile;
	self->driver = driver;
	self->priority = get_priority(self->name, repositories_hierarchy);
	return self;
}

dict_t *repositories_dict_init(list_t * drivers,
			       list_t * repositories_hierarchy)
{
	dict_t *self;
	DIR *dir;
	struct dirent *entry;
	unsigned i;
	driver_t *driver;
	repository_t *repository;

	assert(drivers && repositories_hierarchy);

	self = dict_new();

	dir = opendir("/etc/ports");
	if (!dir)
		return self;

	while ((entry = readdir(dir))) {
		repository = NULL;
		if (*entry->d_name == '.')
			continue;
		if (entry->d_type != DT_UNKNOWN)
			continue;
		for (i = 0; i < drivers->length; i++) {
			driver = list_get(drivers, i);
			if ((repository = driver->get_repository(driver,
								 entry->d_name,
								 repositories_hierarchy)))
				break;
		}
		if (repository)
			dict_add(self, repository->name, repository);
	}
	closedir(dir);

	return self;
}

void repositories_dict_update(dict_t * self, list_t * repositories_name,
			      int enable_xterm_title)
{
	unsigned i;
	repository_t *repository;

	assert(self != NULL && repositories_name != NULL);

	for (i = 0; i < repositories_name->length; i++) {
		if (enable_xterm_title)
			xterm_set_title("Updating %s (%d on %d) ...",
					repositories_name->elements[i], i + 1,
					repositories_name->length);
		cprintf(stdout, "[CYAN]==> Updating %s (%d on %d)[DEFAULT]\n",
			repositories_name->elements[i], i + 1,
			repositories_name->length);
		if ((repository =
		     dict_get(self, repositories_name->elements[i]))
		    == NULL) {
			warning("repository %s not found!",
				repositories_name->elements[i]);
			continue;
		}
		repository_update(repository);
	}
}

void repositories_dict_update_all(dict_t * self, int enable_xterm_title)
{
	unsigned i;

	assert(self != NULL);

	for (i = 0; i < self->length; i++) {
		if (enable_xterm_title)
			xterm_set_title("Updating %s (%d on %d) ...",
					self->elements[i]->key, i + 1,
					self->length);
		cprintf(stdout, "[CYAN]==> Updating %s (%d on %d)[DEFAULT]\n",
			self->elements[i]->key, i + 1, self->length);
		repository_update(self->elements[i]->value);
	}
}

void repositories_dict_dump(dict_t * self)
{
	unsigned i;
	repository_t *repository;

	assert(self != NULL);

	for (i = 0; i < self->length; i++) {
		repository = self->elements[i]->value;
		printf("%s\nPath: %s\nSupfile: %s\nPriority: %d\n"
		       "Driver: %s\n\n", repository->name,
		       repository->path, repository->supfile,
		       repository->priority, ((driver_t *)
					      repository->driver)->name);
	}
}

void repository_dump(repository_t * self)
{
	assert(self);
	printf("%s %s %s %d\n", self->name, self->path, self->supfile,
	       self->priority);
}

void repository_free(repository_t * self)
{
	assert(self);
	free(self->name);
	free(self->path);
	free(self->supfile);
	free(self);
}
