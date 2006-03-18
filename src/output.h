/***************************************************************************
 *            output.h
 *
 *  Fri Jul 16 18:51:36 2004
 *  Copyright  2004 - 2006  Coviello Giuseppe
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

#ifndef _OUTPUT_H
#define _OUTPUT_H

#include <stdarg.h>
#include "pkglist.h"

void error(char *fmt, ...);
void warning(char *fmt, ...);
void info(char *name, int options);
void pkglist_print(struct pkglist *p);
void help();
void version();

#endif				/* _OUTPUT_H */
