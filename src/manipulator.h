/***************************************************************************
 *            manipulator.h
 *
 *  Sat Jul 10 12:49:03 2004
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

#ifndef _MANIPULATOR_H
#define _MANIPULATOR_H

#include "list.h"

#define END -1

void ltrim(char *s);
void rtrim(char *s);
void trim(char *s);
char *mid(char s[], int start, int length);
char *spaces(int n);
char *sed(char *s, char *find, char *replace);
char *sedchr(char *s, int find, int replace);
int count(char *s, int delim);
int old_splitsplit(char *s, char *delim, char *splitted[]);
struct list *split(char *s, char *delim);
void strprintf(char **dest, char *fmt, ...);
extern char *strdup_printf(const char *fmt, ...);
char * strreplace(char *s, char *find, char *replace, int n_replace);
#define strreplaceall(s, find, replace) strreplace((s), (find), (replace), -1)

#endif				/* _MANIPULATOR_H */
