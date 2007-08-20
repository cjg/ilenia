/* cache.c */

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
#include <stdlib.h>
#include <dirent.h>
#include <fnmatch.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "output.h"
#include "memory.h"
#include "repository.h"
#include "cache.h"
#include "str.h"
#include "file.h"
#ifdef _ENABLE_BAD_MAINTAINERS
#include "exec.h"
#endif

static void
cache_parse_pkgfile(repository_t * repository, char *dir_name, FILE * cachefile)
{
	FILE *pkgfile;
	char *line, *deprow, *name, *version, *release, *description;
	size_t n;
	int nread;

	assert(repository != NULL && dir_name != NULL);

	if (chdir(dir_name) != 0)
		return;

	pkgfile = fopen("Pkgfile", "r");

	chdir("..");

	if (!pkgfile)
		return;

	line = deprow = name = version = release = description = NULL;
	version = xstrdup("");
	while ((nread = getline(&line, &n, pkgfile)) >= 0) {
		*(line + strlen(line) - 1) = '\0';
		strtrim(line);
		if (strstr(line, "Depends ") != NULL
		    && strchr(line, ':') != NULL) {
			free(deprow);
			deprow = strtrim(xstrdup(strchr(line, ':') + 1));
			strreplaceall(&deprow, ",", " ");
			while (strstr(deprow, "  "))
				strreplaceall(&deprow, "  ", " ");
		} else if (strstr(line, "Description") != NULL
			   && strchr(line, ':') != NULL) {
			free(description);
			description = strtrim(xstrdup(strchr(line, ':') + 1));
			strreplaceall(&description, ",", " ");
			while (strstr(description, "  "))
				strreplaceall(&description, "  ", " ");
		} else if (strncmp("name=", line, 5) == 0) {
			free(name);
			name = strtrim(xstrdup(strchr(line, '=') + 1));
		} else if (strncmp("version=", line, 8) == 0) {
			free(version);
			version = strtrim(xstrdup(strchr(line, '=') + 1));
#ifdef _ENABLE_BAD_MAINTAINERS
			if (strchr(version, '$') || strchr(version, '`')) {
				char *echo = xstrdup_printf("/bin/echo %s",
							    version);
				char *args[] = { "sh", "-c", echo, NULL };
				execlog("/bin/sh", args, &version);
				free(echo);
				*(version + strlen(version) - 1) = 0;
				strtrim(version);
			}
#endif
			strreplaceall(&version, " ", "_");
			strtrim(version);
		} else if (strncmp("release=", line, 8) == 0) {
			free(release);
			release = strtrim(xstrdup(strchr(line, '=') + 1));
		}

		if (name != NULL && strlen(name) != 0 &&
		    version != NULL && strlen(version) != 0 &&
		    release != NULL && strlen(release) != 0 &&
		    strcmp(name, dir_name) == 0) {
			fprintf(cachefile, "%s %s-%s %s %%%s%%", name, version,
				release, repository->name,
				description != NULL ? description : "");
			if (deprow != NULL && strlen(deprow) != 0)
				fprintf(cachefile, " %s\n", deprow);
			else
				fprintf(cachefile, "\n");
			break;
		}
	}

	free(name);
	free(version);
	free(release);
	free(deprow);
	free(line);
	fclose(pkgfile);
}

static void cache_from_repository(repository_t * repository, FILE * file)
{
	DIR *dir;
	struct dirent *entry;

	assert(repository != NULL && file != NULL);

	dir = opendir(repository->path);
	if (!dir)
		return;

	chdir(repository->path);

	while ((entry = readdir(dir))) {
		if (*entry->d_name == '.')
			continue;
		if (entry->d_type != DT_DIR && entry->d_type != DT_UNKNOWN)
			continue;
		cache_parse_pkgfile(repository, entry->d_name, file);
	}
	closedir(dir);
}

int cache_build(dict_t * repositories, int enable_xterm_title)
{
	FILE *file;
	unsigned i;
	char *current_dir, *cache_file;

	assert(repositories != NULL);

	cache_file = xstrdup_printf("%s/%s", getenv("HOME"), CACHE_FILE);

	printf("Building the cache ");
	fflush(stdout);

	file = fopen(cache_file, "w");
	free(cache_file);
	if (!file)
		return 3;

	current_dir = get_current_dir_name();

	for (i = 0; i < repositories->length; i++) {
		if (enable_xterm_title)
			xterm_set_title("Caching repository %s (%d on %d) ...",
					((repository_t *)
					 repositories->elements[i]->value)->
					name, i + 1, repositories->length);
		cache_from_repository(repositories->elements[i]->value, file);
		printf(".");
		fflush(stdout);
	}
	fclose(file);
	chdir(current_dir);
	free(current_dir);
	printf(" done!\n");
	return 0;
}

void cache_update_stamp(void)
{
	FILE *file;
	if (is_file(STAMP_FILE))
		unlink(STAMP_FILE);
	file = fopen(STAMP_FILE, "w");
	if (file != NULL)
		fclose(file);
}

int cache_is_update(char *cache_file)
{
	struct stat cache_stat, stamp_stat;
	assert(cache_file != NULL);
	lstat(cache_file, &cache_stat);
	lstat(STAMP_FILE, &stamp_stat);
	return cache_stat.st_mtime < stamp_stat.st_mtime;
}
