/* driver.c */

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

#include <assert.h>
#include "memory.h"
#include "rsync.h"
#include "httpup.h"
#include "cvs.h"
#include "local.h"
#include "driver.h"

static driver_t *driver_new(char *name,
			    repository_t * get_repository(struct driver_s *,
							  char
							  *, list_t *),
			    void update(repository_t *))
{
	driver_t *self;
	assert(name && get_repository && update);
	self = xmalloc(sizeof(driver_t));
	self->name = name;
	self->get_repository = get_repository;
	self->update = update;
	return self;
}

list_t *drivers_list_init(void)
{
	list_t *self;
	self = list_new();
	list_append(self, driver_new("rsync", rsync_get_repository,
				     rsync_update));
	list_append(self, driver_new("httpup", httpup_get_repository,
				     httpup_update));
	list_append(self, driver_new("cvs", cvs_get_repository, cvs_update));
	list_append(self, driver_new("local", local_get_repository,
				     local_update));
	return self;
}
