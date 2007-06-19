/* dependencies.h */

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

#ifndef _DEPENDENCIES_H
#define _DEPENDENCIES_H

#include "list.h"
#include "dict.h"

list_t *dependencies_list(list_t * self, char *port_name, dict_t * ports_dict,
			  dict_t * aliases, dict_t * not_founds, 
			  int enable_xterm_title);
void
dependencies_dump(list_t * ports_name, dict_t * ports_dict, dict_t * aliases,
		  dict_t * not_founds, int tree, int verbose, int
		  enable_xterm_title);
list_t *dependents_list(char *port_name, dict_t * ports_dict, dict_t * aliases,
			int all, int enable_xterm_title);
void dependents_dump(char *port_name, dict_t * ports_dict,
		     dict_t * aliases, int tree, int verbose, int all, int
		     enable_xterm_title);

#endif
