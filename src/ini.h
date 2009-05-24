/* ini.h */

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

#ifndef _INI_H

#define _INI_H

#include "dict.h"

typedef struct {
	char *filepath;
	dict_t *sections;
} ini_t;

extern ini_t *ini_new(char *filepath);
extern void ini_free(ini_t * self);
extern ini_t *ini_add(ini_t * self, char *section);
extern ini_t *ini_remove(ini_t * self, char *section);
extern ini_t *ini_set(ini_t * self, char *section, char *var, char *value);
extern ini_t *ini_set_default(ini_t * self, char *section, char *var,
			      char *value);
extern ini_t *ini_unset(ini_t * self, char *section, char *var);
extern int ini_has_section(ini_t * self, char *section);
extern int ini_has_var(ini_t * self, char *section, char *var);
extern char *ini_get(ini_t * self, char *section, char *var);
extern int ini_load(ini_t * self);
extern int ini_save(ini_t * self);
extern void ini_dump(ini_t * self);
dict_t *ini_get_vars(ini_t * self, char *section);

#endif
