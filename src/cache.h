/* cache.h */

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

#ifndef _CACHE_H
#define _CACHE_H

#include "dict.h"

int cache_build(dict_t * repositories, int enable_xterm_title);
void cache_update_stamp(void);
int cache_is_update(char *cache_file);

#ifndef CACHE_FILE
#define CACHE_FILE ".ilenia.cache"
#endif
#ifndef STAMP_FILE
#define STAMP_FILE "/usr/ports/.ilenia.stamp"
#endif

#endif
