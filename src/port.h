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

typedef enum { PRT_NOTINSTALLED, PRT_INSTALLED, PRT_DIFF,
	PRT_OUTDATED
} port_status_t;

typedef struct {
	char *name;
	char *version;
	port_status_t status;
	repository_t *repository;
	list_t *dependencies;
	list_t *dependencies_exploded;
	unsigned long hash;
	unsigned deep;
	char *description;
	char *pkgmk_conf;
} port_t;

port_t *port_new(char *name, char *version,
		 repository_t * repository, list_t * dependencies,
		 char *description);
void port_dump(port_t * self);
void port_free(port_t * self);
int port_have_readme(port_t * self);
list_t *ports_list_init(dict_t * repositories, int enable_xterm_title);
dict_t *ports_dict_init(list_t * ports_list, list_t * packages, conf_t * conf);
port_t *port_query_by_repository(port_t * self, char *repository_name);
port_t *port_query_by_name(port_t * self, char *name);
port_t *port_query_by_description(port_t * self, char *key);
port_t *port_query_by_hash(port_t * self, unsigned long *hash);
void port_show_outdated(dict_t * ports, list_t * packages, int enable_xterm_title);
void port_show_diffs(dict_t * ports, list_t * packages, int enable_xterm_title);

#endif
