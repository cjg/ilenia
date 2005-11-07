/***************************************************************************
 *            lsports.c
 *
 *  Sat Jul 10 12:57:55 2004
 *  Copyright  2004 - 2005  Coviello Giuseppe
 *  immigrant@email.it
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include "manipulator.h"
#include "pkglist.h"
#include "deplist.h"
#include "repolist.h"
#include "aliaslist.h"
#include "ilenia.h"

FILE *cachefile;

#define PORTS_LOCATION "/usr/ports"

struct repolist *parse_rsync(char *path, struct repolist *r)
{
	FILE *file;
	file = fopen(path, "r");
	if (file == NULL)
		return (r);
	char *line = NULL;
	size_t n = 0;
	ssize_t nread;

	while ((nread = getline(&line, &n, file)) != -1) {
		char *prefix;
		char *mad_prefix = NULL;
		char *repo;
		line[strlen(line) - 1] = '\0';
		trim(line);

		if (line[0] == '#' || strlen(line) == 0)
			continue;

		if (strstr(line, "destination") == NULL)
			continue;

		repo = get_value(line, "destination");
		prefix = strdup(repo);

		repo = rindex(repo, '/');
		repo = mid(repo, 1, END);

		prefix = mid(prefix, 0, strlen(prefix) - strlen(repo));

		if (strncmp
		    (prefix, PORTS_LOCATION,
		     strlen(PORTS_LOCATION)) == 0
		    && strlen(prefix) > strlen(PORTS_LOCATION) + 1) {
			mad_prefix =
			    mid(prefix, strlen(PORTS_LOCATION) + 1, END);
			strcat(mad_prefix, "/");
			prefix = strdup(PORTS_LOCATION);
			repo = strcat(mad_prefix, repo);
			sed(repo, "//", "/");
		}
		r = repolist_add(repo, prefix, r);
		fclose(file);
		return (r);
	}
	fclose(file);
	return (r);
}

struct repolist *parse_local(char *path, struct repolist *r)
{
	FILE *file;

	file = fopen(path, "r");
	if (file == NULL)
		return (r);

	char *line = NULL;
	char *repo;
	char *prefix = NULL;
	char *mad_prefix;
	size_t n = 0;
	ssize_t nread;

	while ((nread = getline(&line, &n, file)) != -1) {
		line[strlen(line) - 1] = '\0';
		trim(line);
		if (line[0] == '#')
			continue;
		if (strstr(line, "PATH"))
			prefix = get_value(line, "PATH");
	}
	repo = rindex(path, '/');
	repo = mid(repo, 1, strlen(repo) - strlen(index(repo, '.')) - 1);
	mad_prefix = strdup("local/");
	repo = strcat(mad_prefix, repo);
	r = repolist_add(repo, prefix, r);

	return (r);
}

// not changed yet

struct repolist *parse_cvs(char *percorso, struct repolist *p)
{
	FILE *file;

	if ((file = fopen(percorso, "r"))) {
		char riga[255];
		char *prefix = "";
		char *mad_prefix = "";
		char *collezione = "";
		char *value = "";

		while (fgets(riga, 255, file)) {
			char *tmp;
			tmp = strdup(riga);
			trim(tmp);
			strcpy(riga, tmp);
			if (riga[0] != '#') {
				if (strncmp(riga, "LOCAL_PATH", 10) == 0) {
					if (strlen
					    (value =
					     get_value(riga,
						       "LOCAL_PATH"))) {
						prefix = strdup(value);
						if (strncmp
						    (prefix,
						     PORTS_LOCATION,
						     strlen
						     (PORTS_LOCATION))
						    == 0
						    && strlen(prefix) >
						    strlen(PORTS_LOCATION))
						{
							mad_prefix =
							    mid(prefix,
								strlen
								(PORTS_LOCATION)
								+ 1, END);
							if (mad_prefix
							    [strlen
							     (mad_prefix) -
							     1] != '/')
								mad_prefix
								    =
								    strcat
								    (mad_prefix,
								     "/");
							mad_prefix =
							    strdup
							    (mad_prefix);
							prefix =
							    strdup
							    (PORTS_LOCATION);
						}
					}
				}
				if (strncmp(riga, "LOCAL_DIR", 9) == 0)
					if (strlen
					    (value =
					     get_value(riga, "LOCAL_DIR")))
						collezione = strdup(value);
			}
			if ((strlen(prefix) * strlen(collezione)) > 0) {
				if (strlen(mad_prefix) > 0)
					collezione =
					    strcat(mad_prefix, collezione);
				p = repolist_add(collezione, prefix, p);
				return (p);
			}
		}
	}
	return (p);
}

struct repolist *parse_httpup(char *path, struct repolist *r)
{
	FILE *file;
	char *line = NULL;
	size_t n = 0;
	ssize_t nread;

	file = fopen(path, "r");

	if (file == NULL)
		return (r);

	while ((nread = getline(&line, &n, file)) != -1) {
		char *prefix;
		char *mad_prefix = NULL;
		char *repo;
		line[strlen(line) - 1] = '\0';
		trim(line);

		if (line[0] == '#' || strlen(line) == 0)
			continue;

		if (strstr(line, "ROOT_DIR") == NULL)
			continue;

		repo = get_value(line, "ROOT_DIR");
		prefix = strdup(repo);

		repo = rindex(repo, '/');
		repo = mid(repo, 1, END);

		prefix = mid(prefix, 0, strlen(prefix) - strlen(repo));

		if (strncmp
		    (prefix, PORTS_LOCATION,
		     strlen(PORTS_LOCATION)) == 0
		    && strlen(prefix) > strlen(PORTS_LOCATION) + 1) {
			mad_prefix =
			    mid(prefix, strlen(PORTS_LOCATION) + 1, END);
			strcat(mad_prefix, "/");
			prefix = strdup(PORTS_LOCATION);
			repo = strcat(mad_prefix, repo);
			sed(repo, "//", "/");
		}
		r = repolist_add(repo, prefix, r);
		return (r);
	}
	return (r);
}

struct repolist *parse_cvsup(char *path, struct repolist *r)
{
	FILE *file;
	char *line = NULL;
	size_t n = 0;
	ssize_t nread;

	file = fopen(path, "r");
	if (file == NULL)
		return (r);

	char *prefix = NULL;
	char *mad_prefix = NULL;
	char *repo;

	while ((nread = getline(&line, &n, file)) != -1) {
		line[strlen(line) - 1] = '\0';
		trim(line);
		if (line[0] == '#' || strlen(line) == 0)
			continue;

		if (strstr(line, "*default prefix")) {
			prefix = get_value(line, "*default prefix");
			if (prefix == NULL)
				continue;
			if (strlen(prefix) <= strlen(PORTS_LOCATION))
				continue;
			if (strncmp
			    (prefix, PORTS_LOCATION,
			     strlen(PORTS_LOCATION)))
				continue;
			mad_prefix =
			    mid(prefix, strlen(PORTS_LOCATION) + 1, END);
			trim(mad_prefix);
			strcat(mad_prefix, "/");
			prefix = strdup(PORTS_LOCATION);

		}

		if (line[0] == '*')
			continue;

		if (strlen(line)) {
			if (mad_prefix) {
				repo = strdup(mad_prefix);
				strcat(repo, line);
				sed(repo, "//", "/");
			} else
				repo = strdup(line);

			strtok(repo, " ");
			r = repolist_add(repo, prefix, r);
		}
	}

	if (line)
		free(line);
	fclose(file);
	return (r);
}

struct deplist *deplist_from_deprow(char *deprow)
{
	struct deplist *d = NULL;

	if (strlen(deprow) <= 0)
		return (NULL);

	int n, i;

	deprow = sedchr(deprow, ',', ' ');

	while (strstr(deprow, "  "))
		deprow = sed(deprow, "  ", " ");
	n = count(deprow, ' ');

	char *deps[n];
	split(deprow, " ", deps);

	for (i = 0; i < n; i++) {
		trim(deps[i]);
		d = deplist_add(deps[i], d);
	}
	return (d);
}

int parse_pkgfile(char *filename, char *repo)
{
	FILE *file;
	size_t n = 0;
	char *line = NULL;
	ssize_t nread;

	struct deplist *d = NULL;

	char *name = NULL, *version = NULL, *release = NULL;

	file = fopen(filename, "r");
	if (file == NULL)
		return (EXIT_FAILURE);

	while ((nread = getline(&line, &n, file)) != -1) {
		trim(line);
		if (line[0] == '#') {
			line = mid(line, 1, END);
			trim(line);

			if (strstr(line, "Depends") == NULL)
				continue;
			if (strstr(line, ":") == NULL)
				continue;

			line = strstr(line, ":");

			line = mid(line, 1, END);
			trim(line);
			d = deplist_from_deprow(line);
			line = NULL;

		} else if (strncmp(line, "name", 4) == 0)
			name = get_value(line, "name");
		else if (strncmp(line, "version", 7) == 0)
			version = get_value(line, "version");
		else if (strncmp(line, "release", 7) == 0)
			release = get_value(line, "release");
	}

	if (!(name && version && release))
		return (EXIT_FAILURE);

	version = sedchr(version, ' ', '_');

	fprintf(cachefile, "%s %s-%s %s", name, version, release, repo);

	while (d != NULL) {
		fprintf(cachefile, " %s", d->name);
		d = d->next;
	}

	fprintf(cachefile, "\n");
	if (line)
		free(line);
	fclose(file);
	return EXIT_SUCCESS;
}

int read_from_dir(char *repo_name, char *prefix)
{
	DIR *dir;
	struct dirent *info_file;
	struct stat stat_file;
	char path[strlen(prefix) + strlen(repo_name) + 1];

	strcpy(path, prefix);
	if (strncmp(repo_name, "local", 5) != 0) {
		strcat(path, "/");
		strcat(path, repo_name);
	}

	dir = opendir(path);
	if (dir == NULL)
		return (EXIT_FAILURE);

	while ((info_file = readdir(dir))) {
		char filename[strlen(path) + strlen(info_file->d_name) +
			      9];

		sprintf(filename, "%s/%s", path, info_file->d_name);
		stat(filename, &stat_file);
		if (S_ISDIR(stat_file.st_mode)
		    && info_file->d_name[0] != '.') {
			strcat(filename, "/Pkgfile");
			parse_pkgfile(filename, repo_name);
		}
	}
	closedir(dir);
	return (EXIT_SUCCESS);
}

int build_cache(struct repolist *r)
{
	printf("Building cache!\n");
	if (!(cachefile = fopen(CACHE, "w"))) {
		return (-1);
	}
	while (r != NULL) {
		read_from_dir(r->name, r->path);
		r = r->next;
	}
	fclose(cachefile);
	chmod(CACHE,
	      S_IREAD | S_IWRITE | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	printf("cache built!\n");
	return (EXIT_SUCCESS);
}

struct pkglist *lsports()
{
	FILE *file;
	struct pkglist *p = NULL;

	file = fopen(CACHE, "r");
	if (file == NULL)
		return (NULL);

	size_t n = 0;
	char *line = NULL;
	int nread = getline(&line, &n, file);

	if (nread < 0) {
		if (build_cache(ilenia_repos) != EXIT_SUCCESS)
			return (NULL);
		nread = getline(&line, &n, file);
	}

	while (nread > 0) {
		int i, num = count(line, ' ');
		char *splitted_line[num];

		split(line, " ", splitted_line);
		struct deplist *d = NULL;

		for (i = 3; i <= num; i++) {
			trim(splitted_line[i]);
			if (strlen(splitted_line[i]) > 0)
				d = deplist_add(splitted_line[i], d);
		}
		trim(splitted_line[0]);
		trim(splitted_line[1]);
		trim(splitted_line[2]);
		p = pkglist_add_ordered(splitted_line[0],
					splitted_line[1],
					splitted_line[2], d, p);
		nread = getline(&line, &n, file);
	}

	fclose(file);
	return (p);
}

struct repolist *build_repolist()
{
	struct repolist *r = NULL;
	DIR *dir;
	struct dirent *info_file;
	char *extension;

	dir = opendir("/etc/ports");
	if (dir == NULL)
		return (NULL);

	while ((info_file = readdir(dir))) {

		if (strstr(info_file->d_name, ".") == NULL)
			continue;

		extension = strstr(info_file->d_name, ".");

		char filename[strlen(info_file->d_name) + 11];

		sprintf(filename, "/etc/ports/%s", info_file->d_name);

		if (strcmp(extension, ".cvsup") == 0)
			r = parse_cvsup(filename, r);
		else if (strcmp(extension, ".httpup") == 0)
			r = parse_httpup(filename, r);
		else if (strcmp(extension, ".cvs") == 0)
			r = parse_cvs(filename, r);
		else if (strcmp(extension, ".local") == 0)
			r = parse_local(filename, r);
		else if (strcmp(extension, ".rsync") == 0)
			r = parse_rsync(filename, r);
	}

	closedir(dir);

	return (r);
}
