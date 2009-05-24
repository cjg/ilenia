/* package.c */

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
#include <string.h>
#include <assert.h>
#include "package.h"
#include "memory.h"
#include "port.h"
#include "list.h"

static port_t *package_new(char *name, char *version)
{
	port_t *self;
	assert(name && version);
	self = port_new(name, version, NULL, list_new(), NULL);
	self->status = PRT_INSTALLED;
	return self;
}

list_t *packages_list_init(void)
{
	list_t *self;

	FILE *dbfile;
	char *line, *name, *version;
	int nread;
	size_t n;

	dbfile = fopen("/var/lib/pkg/db", "r");
	if (!dbfile)
		return NULL;

	self = list_new();
	line = NULL;
	name = NULL;
	version = NULL;
	while ((nread = getline(&line, &n, dbfile)) > 0) {
		*(line + strlen(line) - 1) = 0;
		if (!name)
			name = xstrdup(line);
		else if (!version)
			version = xstrdup(line);
		else if (!strlen(line)) {
			list_append(self, package_new(name, version));
			name = version = NULL;
		}
	}
	free(line);
	fclose(dbfile);
	return self;

}

port_t *packages_list_get(list_t * self, char *port_name)
{
	unsigned i;
	port_t *port;
	assert(self && port_name);

	for (i = 0; i < self->length; i++) {
		port = list_get(self, i);
		if (!strcmp(port->name, port_name))
			return port;
	}

	return NULL;
}
