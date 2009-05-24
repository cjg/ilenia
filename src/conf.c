/* conf.c */

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
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "memory.h"
#include "conf.h"
#include "str.h"
#include "port.h"
#include "ini.h"

#ifndef CONF_FILE
#define CONF_FILE "/etc/ilenia.conf"
#endif

static char *policies[] = { "ASK", "STOP", "NEVERMIND" };

conf_t *conf_init(void)
{
	conf_t *self;
	ini_t *ini;
	char *tmp, **splitted;
	int nsplits;
	unsigned i;
	dict_t *vars;

	self = xmalloc(sizeof(conf_t));
	self->post_pkgadd = NULL;
	self->ask_for_update = 1;
	self->not_found_policy = ASK;

	ini = ini_new(CONF_FILE);
	ini_add(ini, "ilenia");
	ini_set_default(ini, "ilenia", "post_pkgadd", xstrdup(""));
	ini_set_default(ini, "ilenia", "ask_for_update", xstrdup("1"));
	ini_set_default(ini, "ilenia", "not_found_policy", xstrdup("ASK"));
	ini_set_default(ini, "ilenia", "repositories_hierarchy", xstrdup(""));
	ini_set_default(ini, "ilenia", "enable_colors", xstrdup("Yes"));
	ini_set_default(ini, "ilenia", "verbose", xstrdup("No"));
	ini_set_default(ini, "ilenia", "enable_xterm_title", xstrdup("Yes"));
	ini_set_default(ini, "ilenia", "default_xterm_title", xstrdup(""));
	ini_set_default(ini, "ilenia", "enable_log", xstrdup("Yes"));
	ini_set_default(ini, "ilenia", "rejected_policy", xstrdup("CHECK"));
	ini_set_default(ini, "ilenia", "never_install", xstrdup(""));
	ini_add(ini, "favourite_repositories");
	ini_add(ini, "locked_versions");
	ini_add(ini, "aliases");
	ini_add(ini, "pkgmk_confs");
	ini_load(ini);

	if ((tmp = getenv("POST_PKGADD")) == NULL)
		tmp = ini_get(ini, "ilenia", "post_pkgadd");
	self->post_pkgadd = xstrdup(tmp);

	if ((tmp = getenv("ASK_FOR_UPDATE")) == NULL)
		tmp = ini_get(ini, "ilenia", "ask_for_update");
	if (!strcasecmp(tmp, "NO"))
		self->ask_for_update = 0;
	else
		self->ask_for_update = 1;

	if ((tmp = getenv("NOT_FOUND_POLICY")) == NULL)
		tmp = ini_get(ini, "ilenia", "not_found_policy");
	if (!strcasecmp(tmp, "NEVERMIND"))
		self->not_found_policy = NEVERMIND;
	else if (!strcasecmp(tmp, "STOP"))
		self->not_found_policy = STOP;
	else
		self->not_found_policy = ASK;

	tmp = ini_get(ini, "ilenia", "repositories_hierarchy");
	if (tmp != NULL) {
		strreplace(&tmp, "\t", " ", -1);
		while (strstr(tmp, "  "))
			strreplace(&tmp, "  ", " ", -1);

		splitted = NULL;
		nsplits = strsplit(tmp, ' ', &splitted);
		self->repositories_hierarchy =
		    list_new_from_array((void **)splitted, nsplits);
		free(splitted);
	} else
		self->repositories_hierarchy = list_new();

	if ((tmp = getenv("ENABLE_COLORS")) == NULL)
		tmp = ini_get(ini, "ilenia", "enable_colors");
	if (!strcasecmp(tmp, "NO"))
		self->enable_colors = 0;
	else
		self->enable_colors = 1;

	if ((tmp = getenv("VERBOSE")) == NULL)
		tmp = ini_get(ini, "ilenia", "verbose");
	if (strcasecmp(tmp, "Yes") == 0)
		self->verbose = 1;
	else
		self->verbose = 0;

	if ((tmp = getenv("ENABLE_XTERM_TITLE")) == NULL)
		tmp = ini_get(ini, "ilenia", "enable_xterm_title");
	if (!strcasecmp(tmp, "NO"))
		self->enable_xterm_title = 0;
	else
		self->enable_xterm_title = 1;
	
	if ((tmp = getenv("DEFAULT_XTERM_TITLE")) == NULL)
		tmp = ini_get(ini, "ilenia", "default_xterm_title");
	self->default_xterm_title = xstrdup(tmp);

	if ((tmp = getenv("ENABLE_LOG")) == NULL)
		tmp = ini_get(ini, "ilenia", "enable_log");
	if (!strcasecmp(tmp, "NO"))
		self->enable_log = 0;
	else
		self->enable_log = 1;

	if ((tmp = getenv("REJECTED_POLICY")) == NULL)
		tmp = ini_get(ini, "ilenia", "rejected_policy");
	if (!strcasecmp(tmp, "NEVERMIND"))
		self->rejected_policy = REJ_NEVERMIND;
	else if (!strcasecmp(tmp, "RUN"))
		self->rejected_policy = REJ_RUN;
	else
		self->rejected_policy = REJ_CHECK;

	tmp = ini_get(ini, "ilenia", "never_install");
	if (tmp != NULL) {
		strreplace(&tmp, "\t", " ", -1);
		while (strstr(tmp, "  "))
			strreplace(&tmp, "  ", " ", -1);

		splitted = NULL;
		nsplits = strsplit(tmp, ' ', &splitted);
		self->never_install =
		    list_new_from_array((void **)splitted, nsplits);
		free(splitted);
	} else
		self->never_install = list_new();

	self->favourite_repositories = dict_new();
	vars = ini_get_vars(ini, "favourite_repositories");

	for (i = 0; vars && i < vars->length; i++)
		dict_add(self->favourite_repositories, vars->elements[i]->key,
			 xstrdup(vars->elements[i]->value));

	self->locked_versions = dict_new();
	vars = ini_get_vars(ini, "locked_versions");

	for (i = 0; vars && i < vars->length; i++)
		dict_add(self->locked_versions, vars->elements[i]->key,
			 xstrdup(vars->elements[i]->value));

	self->aliases = dict_new();
	vars = ini_get_vars(ini, "aliases");
	for (i = 0; vars != NULL && i < vars->length; i++) {
		tmp = vars->elements[i]->value;

		strreplace(&tmp, "\t", " ", -1);
		while (strstr(tmp, "  "))
			strreplace(&tmp, "  ", " ", -1);

		splitted = NULL;
		nsplits = strsplit(tmp, ' ', &splitted);
		if (nsplits == 0)
			continue;
		dict_add(self->aliases, vars->elements[i]->key,
			 list_new_from_array((void **)splitted, nsplits));
		free(splitted);
	}

	self->pkgmk_confs = dict_new();
	vars = ini_get_vars(ini, "pkgmk_confs");

	for (i = 0; vars && i < vars->length; i++)
		dict_add(self->pkgmk_confs, vars->elements[i]->key,
			 xstrdup(vars->elements[i]->value));

	ini_free(ini);
	return self;
}

void aliases_free(dict_t * self)
{
	unsigned i;
	assert(self != NULL);
	for (i = 0; i < self->length; i++)
		list_free((list_t *) self->elements[i]->value, free);
	dict_free(self, NULL);
}

void conf_free(conf_t * self)
{
	assert(self != NULL);
	free(self->post_pkgadd);
	free(self->default_xterm_title);
	list_free(self->repositories_hierarchy, free);
	dict_free(self->favourite_repositories, NULL);
	dict_free(self->locked_versions, free);
	aliases_free(self->aliases);
	dict_free(self->pkgmk_confs, free);
	list_free(self->never_install, free);
	free(self);
}

void conf_dump(conf_t * self)
{
	unsigned i, j;
	port_t *port;
	list_t *list;
	assert(self);
	printf("POST_PKGADD = %s\n", self->post_pkgadd);
	printf("ASK_FOR_UPDATE = %d\n", self->ask_for_update);
	printf("NOT_FOUND_POLICY = %s\n", policies[self->not_found_policy]);
	printf("REPOSITORIES_HIERARCHY = ");
	for (i = 0; i < self->repositories_hierarchy->length; i++)
		printf("%s ",
		       (char *)list_get(self->repositories_hierarchy, i));
	printf("\n");
	printf("NEVER_INSTALL = ");
	for (i = 0; i < self->never_install->length; i++)
		printf("%s ",
		       (char *)list_get(self->never_install, i));
	printf("\n");
	printf("FAVOURITE_REPOSITORIES =\n");
	for (i = 0; i < self->favourite_repositories->length; i++) {
		port = self->favourite_repositories->elements[i]->value;
		if (port == NULL)
			continue;
		printf("\t%s = %s\n", port->name, port->repository->name);
	}
	printf("LOCKED_VERSIONS =\n");
	for (i = 0; i < self->locked_versions->length; i++) {
		printf("\t%s = %s\n", self->locked_versions->elements[i]->key,
		       (char *)self->locked_versions->elements[i]->value);
	}
	printf("ALIASES =\n");
	for (i = 0; i < self->aliases->length; i++) {
		printf("\t%s = ", self->aliases->elements[i]->key);
		list = self->aliases->elements[i]->value;
		for (j = 0; j < list->length; j++)
			printf("%s ", (char *)list->elements[j]);
		printf("\n");
	}
}

void conf_reparse(conf_t * self, list_t * ports)
{
	list_t *list1, *list2;
	dict_t *favourite_repositories;
	char *port_name, *repository_name;
	unsigned i, j;
	port_t *port;
	assert(self && ports);

	favourite_repositories = dict_new();

	for (i = 0; i < self->favourite_repositories->length; i++) {
		port_name =
		    xstrdup(self->favourite_repositories->elements[i]->key);
		repository_name =
		    xstrdup(self->favourite_repositories->elements[i]->value);
		list1 = list_query(ports, port_query_by_name, port_name);
		list2 =
		    list_query(list1, port_query_by_repository,
			       repository_name);
		for (j = 0; j < list2->length; j++) {
			port = list_get(list2, j);
			dict_add(favourite_repositories, port->name, port);
		}
		list_free(list1, NULL);
		list_free(list2, NULL);
		free(port_name);
		free(repository_name);
	}

	dict_free(self->favourite_repositories, free);
	self->favourite_repositories = favourite_repositories;
}
