/* str.h */

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

#ifndef _STR_H
#define _STR_H

int strequ(const char *s1, const char *s2);
char *strltrim(char *s);
char *strrtrim(char *s);
char *strtrim(char *s);
char *strappend(char **dest, const char *src);
char *strprepend(char **dest, const char *src);
char *strreplace(char **s, const char *find, const char *replace, int n_replace);
char *strupper(char *s);
char *strlower(char *s);
int strsplit(const char *str, const char sep, char ***ptr_array);

#define strreplaceall(s, find, replace) strreplace((s), (find), (replace), -1)

#endif
