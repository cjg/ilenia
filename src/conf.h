/* conf.h */

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

#ifndef _CONF_H
#define _CONF_H
#include "list.h"
#include "dict.h"

typedef enum { ASK, STOP, NEVERMIND } not_found_policies_t;

typedef struct {
	char *post_pkgadd;
	int ask_for_update;
	not_found_policies_t not_found_policy;
	int enable_colors;
	int verbose;
	list_t *repositories_hierarchy;
	dict_t *favourite_repositories;
	dict_t *locked_versions;
	dict_t *aliases;
	dict_t *pkgmk_confs;
} conf_t;

conf_t *conf_init(void);
void conf_free(conf_t * self);
void conf_dump(conf_t * self);
void conf_reparse(conf_t * self, list_t * ports);
void aliases_free(dict_t * self);

#endif
