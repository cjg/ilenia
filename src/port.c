/* port.c */

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
#include <assert.h>
#include <string.h>
#include <fnmatch.h>
#include <math.h>
#include "memory.h"
#include "str.h"
#include "package.h"
#include "conf.h"
#include "cache.h"
#include "file.h"
#include "output.h"

port_t *port_new(char *name, char *version,
		 repository_t * repository, list_t * dependencies, char
		 *description)
{
	port_t *self;

	assert(name != NULL && version != NULL);

	self = xmalloc(sizeof(port_t));
	self->name = name;
	self->version = version;
	self->status = PRT_NOTINSTALLED;
	self->repository = repository;
	self->dependencies = dependencies;
	self->dependencies_exploded = NULL;
/* 	self->hash = sdbm_hash(name); */
	self->description = description;
	self->pkgmk_conf = NULL;
	self->deep = 0;
	self->cyclic_dependencies_status = CYCLIC_DEPENDENCIES_NOT_CHECKED;
	return self;
}

void port_dump(port_t * self)
{
	assert(self);
	printf("%s %s %s\n", self->name, self->version,
	       (self->repository !=
		NULL ? self->repository->name : "not found"));

}

int port_have_readme(port_t * self)
{
	char *path;
	int ret;

	assert(self != NULL);

	if (self->repository == NULL)
		return 0;

	path = xstrdup_printf("%s/%s/README", self->repository->path,
			      self->name);
	ret = is_file(path);
	free(path);
	return ret;
}

void port_free(port_t * self)
{
	free(self->version);
	list_free(self->dependencies, free);
	if (self->dependencies_exploded != NULL)
		list_free(self->dependencies_exploded, NULL);
	free(self->name);
	free(self->description);
	free(self->pkgmk_conf);
	free(self);
}

list_t *ports_list_init(dict_t * repositories, int enable_xterm_title)
{
	list_t *self;
	FILE *cachefile;
	char *line, **splitted, *description, *cache_file;
	int nread, nsplit;
	size_t n;

	cache_file = xstrdup_printf("%s/%s", getenv("HOME"), CACHE_FILE);

	if (!is_file(cache_file) || cache_is_update(cache_file)) {
		if (cache_build(repositories, enable_xterm_title)) {
			free(cache_file);
			return NULL;
		}
	}

	cachefile = fopen(cache_file, "r");
	free(cache_file);

	if (!cachefile)
		return NULL;

	self = list_new();
	line = NULL;
	nread = getline(&line, &n, cachefile);

	if (nread < 0) {
		cache_build(repositories, enable_xterm_title);
		nread = getline(&line, &n, cachefile);
	}

	while (nread > 0) {
		*(line + strlen(line) - 1) = 0;
		description = strchr(line, '%') + 1;
		description = xstrndup(description, strlen(description) -
				       strlen(strchr(description, '%')));
		if (strlen(description) > 0) {
			char *xdescription = xstrdup_printf("%%%s%%",
							    description);
			strreplace(&line, xdescription, "", 1);
			free(xdescription);
		} else
			strreplace(&line, "%%", "", 1);
		while (strstr(line, "  ") != NULL)
			strreplaceall(&line, "  ", " ");
		splitted = NULL;
		nsplit = strsplit(line, ' ', &splitted);
		list_append(self, port_new(splitted[0],
					   splitted[1],
					   dict_get(repositories,
						    splitted[2]),
					   list_new_from_array((void **)
							       &splitted[3],
							       nsplit - 3),
					   description));
		free(splitted[2]);
		free(splitted);
		nread = getline(&line, &n, cachefile);
	}
	if (line)
		free(line);
	fclose(cachefile);

	return self;
}

static dict_t *pkgmk_confs_explode(dict_t * pkgmk_confs, list_t * ports)
{
	unsigned i, j;
	dict_t *exploded;
	char *key, *pkgmk_conf;
	list_t *list;
	port_t *port;

	exploded = dict_new();

	for (i = 0; i < pkgmk_confs->length; i++) {
		key = xstrdup(pkgmk_confs->elements[i]->key);
		pkgmk_conf = xstrdup(pkgmk_confs->elements[i]->value);
		if (!is_file(pkgmk_conf)) {
			warning("pkgmk's conf %s, not found!", pkgmk_conf);
			free(key);
			free(pkgmk_conf);
			continue;
		}
		list = list_query(ports, port_query_by_name, key);
		for (j = 0; j < list->length; j++) {
			port = list->elements[j];
			if (dict_get(exploded, port->name) == NULL)
				dict_add(exploded, port->name,
					 xstrdup(pkgmk_conf));
		}
		list_free(list, NULL);
		free(key);
		free(pkgmk_conf);
	}

	return exploded;
}

hash_t *port_hash_init(list_t * ports_list, list_t * packages, conf_t * conf)
{
	hash_t *self;
	dict_t *pkgmk_confs_exploded;
	port_t *port, *found, *package;
	unsigned i;
	int cmp;
	char *locked_version, *pkgmk_conf;

	assert(ports_list != NULL && packages != NULL && conf != NULL);

	pkgmk_confs_exploded = pkgmk_confs_explode(conf->pkgmk_confs,
						   ports_list);

	self = hash_new_with_size(1000);

	for (i = 0; i < ports_list->length; i++) {
		port = list_get(ports_list, i);
		if ((pkgmk_conf = dict_get(pkgmk_confs_exploded, port->name)) !=
		    NULL)
			port->pkgmk_conf = xstrdup(pkgmk_conf);
		if (list_get_position(conf->never_install, strequ, port->name)
		    != -1)
			port->status = PRT_NEVERINSTALL;
		if ((locked_version =
		     dict_get(conf->locked_versions, port->name))) {
			if (!strcmp(port->version, locked_version))
				hash_add(self, port->name, port);
		} else if (locked_version == NULL &&
			   (found = dict_get(conf->favourite_repositories,
					     port->name)) != NULL) {
			if (strcmp(found->repository->name,
				   port->repository->name) == 0)
				hash_add(self, port->name, port);
		} else if (!locked_version
			   && !(found = hash_get(self, port->name)))
			hash_add(self, port->name, port);
		else if (!locked_version && port->repository->priority >
			 found->repository->priority)
			hash_add(self, port->name, port);
		else if (!locked_version && port->repository->priority ==
			 found->repository->priority &&
			 strverscmp(port->version, found->version) > 0)
			hash_add(self, port->name, port);
	}

	for (i = 0; i < packages->length; i++) {
		package = list_get(packages, i);
		if (!(port = hash_get(self, package->name))) {
			hash_add(self, package->name, package);
			continue;
		}
		if(port->status == PRT_NEVERINSTALL)
			continue;
		if ((cmp = strverscmp(port->version, package->version)) > 0)
			port->status = PRT_OUTDATED;
		else if (cmp < 0)
			port->status = PRT_DIFF;
		else
			port->status = PRT_INSTALLED;
	}

	dict_free(pkgmk_confs_exploded, free);

	return self;
}

port_t *port_query_by_repository(port_t * self, char *repository_name)
{
	if (!fnmatch(repository_name, self->repository->name, FNM_CASEFOLD))
		return self;
	return NULL;
}

port_t *port_query_by_name(port_t * self, char *name)
{
	if (!fnmatch(name, self->name, FNM_CASEFOLD))
		return self;
	return NULL;
}

port_t *port_query_by_description(port_t * self, char *key)
{
	if (!fnmatch(key, self->description, FNM_CASEFOLD))
		return self;
	return NULL;
}

void port_swap(port_t * port1, port_t * port2)
{
	port_t *tmp;
	tmp = port2;
	port2 = port1;
	port1 = tmp;
}

void port_show_outdated(hash_t * ports, list_t * packages,
			int enable_xterm_title)
{
	port_t *port, *package;
	hashiterator_t *iter;

	assert(ports != NULL && packages != NULL);

	if (enable_xterm_title)
		xterm_set_title("Finding outdated packages ...");
/* 	for (i = 0; i < ports->length; i++) { */
	iter = hashiterator_new(ports);
	while(hashiterator_next(iter)) {
		port = hashiterator_get(iter);
		if (port->status == PRT_OUTDATED) {
			package = packages_list_get(packages, port->name);
			printf("%-26s %18s %-14s %18s\n",
			       port->name, package->version,
			       port->repository->name, port->version);
		}
	}
}

void port_show_diffs(hash_t * ports, list_t * packages, int enable_xterm_title)
{
	port_t *port, *package;
	hashiterator_t *iter;

	assert(ports != NULL && packages != NULL);

	if (enable_xterm_title)
		xterm_set_title("Finding packages with different versions ...");
/* 	for (i = 0; i < ports->length; i++) { */
	iter = hashiterator_new(ports);
	while(hashiterator_next(iter)) {
		port = hashiterator_get(iter);
		if (port->status == PRT_OUTDATED || port->status == PRT_DIFF) {
			package = packages_list_get(packages, port->name);
			printf("%-26s %18s %-14s %18s\n",
			       port->name, package->version,
			       port->repository->name, port->version);
		}
	}
}
