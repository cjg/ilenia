/* output.h */

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

#ifndef _OUTPUT_H
#define _OUTPUT_H
#include <stdio.h>

int coloredprintf(FILE * stream, const char *format, ...);
int uncoloredprintf(FILE * stream, const char *format, ...);
void warning(const char *format, ...);
void error(const char *format, ...);
typedef int (*cprintf_t) (FILE *, const char *, ...);
cprintf_t cprintf;
void xterm_set_title(const char *format, ...);
void xterm_reset_title(const char *default_xterm_title);

#endif
