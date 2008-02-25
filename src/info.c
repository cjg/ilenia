/* info.c */

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
#include "port.h"
#include "output.h"
#include "info.h"
#include "memory.h"
#include "str.h"
#include "file.h"

void port_info_dump(port_t * port)
{
	FILE *file;
	char *description, *url, *maintainer, *path, *line;
	size_t n;

	assert(port != NULL);

	path =
	    xstrdup_printf("%s/%s/Pkgfile", port->repository->path, port->name);

	file = fopen(path, "r");

	if (file == NULL)
		return;

	line = description = url = maintainer = NULL;
	while (getline(&line, &n, file) >= 0) {
		*(line + strlen(line) - 1) = '\0';
		strtrim(line);
		if (strstr(line, "Description") != NULL
		    && strchr(line, ':') != NULL) {
			free(description);
			description = strtrim(xstrdup(strchr(line, ':') + 1));
		}
		if (strstr(line, "URL") != NULL && strchr(line, ':') != NULL) {
			free(url);
			url = strtrim(xstrdup(strchr(line, ':') + 1));
		}
		if (strstr(line, "Maintainer") != NULL
		    && strchr(line, ':') != NULL) {
			free(maintainer);
			maintainer = strtrim(xstrdup(strchr(line, ':') + 1));
		}
	}
	free(line);
	printf("Name: %s\n", port->name);
	printf("Version: %s\n", port->version);
	printf("Description: %s\n", description);
	printf("URL: %s\n", url);
	printf("Repository: %s\n", port->repository->name);
	printf("Maintainer: %s\n", maintainer);
	if (port->pkgmk_conf != NULL)
		printf("pkgmk.conf: %s\n", port->pkgmk_conf);
	printf("Path: %s/%s\n", port->repository->path, port->name);
	free(path);
	path =
	    xstrdup_printf("%s/%s/README", port->repository->path, port->name);
	printf("README: %s\n", is_file(path) ? "yes" : "no");
	free(path);
	path = xstrdup_printf("%s/%s/pre-install", port->repository->path,
			      port->name);
	printf("pre-install: %s\n", is_file(path) ? "yes" : "no");
	free(path);
	path = xstrdup_printf("%s/%s/post-install", port->repository->path,
			      port->name);
	printf("post-install: %s\n\n", is_file(path) ? "yes" : "no");
	free(path);
	free(description);
	free(url);
	free(maintainer);
	fclose(file);
}

void info_dump(char *port_name, hash_t * ports_hash)
{
	port_t *port;

	assert(port_name != NULL && ports_hash != NULL);

	if ((port = hash_get(ports_hash, port_name)) == NULL ||
	    port->repository == NULL)
		return warning("%s not found!", port_name);

	port_info_dump(port);
}

void port_readme_dump(port_t * port)
{
	char *path, *pager;

	assert(port != NULL);

	path = xstrdup_printf("%s/%s/README", port->repository->path,
			      port->name);

	pager = getenv("PAGER");

	if (pager == NULL)
		pager = "less";

	if (is_file(path)) {
		strprepend(&path, " ");
		system(strprepend(&path, pager));
	} else
		warning("%s doesn't have a README!", port->name);
	free(path);
}

void readme_dump(char *port_name, hash_t * ports_hash)
{
	port_t *port;

	assert(port_name != NULL && ports_hash != NULL);

	if ((port = hash_get(ports_hash, port_name)) == NULL ||
	    port->repository == NULL)
		return warning("%s not found!", port_name);

	port_readme_dump(port);
}
