/* cvs.c */

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
#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "str.h"
#include "cvs.h"
#include "exec.h"

repository_t *cvs_get_repository(driver_t * self, char *supfile,
				 list_t * repositories_hierarchy)
{
	FILE *file;
	char *line, *path, *name, *extension, *local_path, *local_dir;
	repository_t *repository;
	size_t n;
	int nread;

	assert(supfile);
	if (!(extension = rindex(supfile, '.')) || strcmp(extension, ".cvs"))
		return NULL;

	supfile = xstrdup_printf("/etc/ports/%s", supfile);

	file = fopen(supfile, "r");
	if (file == NULL)
		return NULL;

	line = local_path = local_dir = NULL;
	repository = NULL;

	while ((nread = getline(&line, &n, file)) >= 0) {
		*(line + strlen(line) - 1) = 0;
		strtrim(line);
		if (line[0] == '#' || strlen(line) == 0)
			continue;
		else if (strstr(line, "LOCAL_PATH") && strchr(line, '=')) {
			free(local_path);
			local_path = strtrim(xstrdup(strchr(line, '=') + 1));
		} else if (strstr(line, "LOCAL_DIR") && strchr(line, '=')) {
			free(local_dir);
			local_dir = strtrim(xstrdup(strchr(line, '=') + 1));
		}
	}

	if (local_dir && local_path) {
		path = xstrdup_printf("%s/%s", local_path, local_dir);
		strreplaceall(&path, "\"", "");
		while (strstr(path, "//"))
			strreplaceall(&path, "//", "/");
		if (!strncmp("/usr/ports/", path, 11))
			name = xstrdup(path + 11);
		else
			name = xstrdup(strrchr(path, '/') + 1);
		repository = repository_new(name, path, supfile, self,
					    repositories_hierarchy);
	}
	free(local_path);
	free(local_dir);
	free(line);
	fclose(file);
	return repository;
}

void cvs_update(repository_t * repository)
{
	assert(repository != NULL);
	char *args[] = { "cvs", repository->supfile, NULL };
	exec("/etc/ports/drivers/cvs", args);
}
