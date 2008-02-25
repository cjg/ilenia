/* port.h */

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

#ifndef _PORT_H
#define _PORT_H
#include "conf.h"
#include "list.h"
#include "repository.h"
#include "hash.h"

typedef enum {PRT_NOTINSTALLED, PRT_INSTALLED, PRT_DIFF, PRT_OUTDATED,
	      PRT_NEVERINSTALL } port_status_t;

typedef enum { CYCLIC_DEPENDENCIES_NOT_CHECKED, CYCLIC_DEPENDENCIES_CHECKED,
	       CYCLIC_DEPENDENCIES_EXIST } port_cyclic_dependencies_status_t;

typedef struct {
	char *name;
	char *version;
	port_status_t status;
	repository_t *repository;
	list_t *dependencies;
	list_t *dependencies_exploded;
	unsigned deep;
	char *description;
	char *pkgmk_conf;
	port_cyclic_dependencies_status_t cyclic_dependencies_status;
} port_t;

port_t *port_new(char *name, char *version,
		 repository_t * repository, list_t * dependencies,
		 char *description);
void port_dump(port_t * self);
void port_free(port_t * self);
int port_have_readme(port_t * self);
list_t *ports_list_init(dict_t * repositories, int enable_xterm_title);
hash_t *port_hash_init(list_t * ports_list, list_t * packages, conf_t * conf);
port_t *port_query_by_repository(port_t * self, char *repository_name);
port_t *port_query_by_name(port_t * self, char *name);
port_t *port_query_by_description(port_t * self, char *key);
void port_show_outdated(hash_t * ports, list_t * packages, int enable_xterm_title);
void port_show_diffs(hash_t * ports, list_t * packages, int enable_xterm_title);
#define list_get_port(list,i) ((port_t *)((list)->elements[(i)]))
#define dict_get_port(dict,name) ((port_t *)(dict_get((dict),(name))))
#define dict_get_port_at(dict,i) ((port_t *)((dict)->elements[(i)]->value))
#endif
