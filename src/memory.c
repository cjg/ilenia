/* memory.c */

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void *xmalloc(size_t size)
{
	void *ptr;
	ptr = malloc(size);
	if (!ptr)
		abort();
	return ptr;
}

void *xrealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (!ptr)
		abort();
	return ptr;
}

char *xstrdup(const char *s)
{
	char *dup;
	dup = strdup(s);
	if (!dup)
		abort();
	return (dup);
}

char *xstrndup(const char *s, size_t n)
{
	char *dup;
	dup = strndup(s, n);
	if (!dup)
		abort();
	return (dup);
}

char *xstrdup_printf(const char *fmt, ...)
{
	int n;
	int size;
	char *s;
	va_list ap;

	size = 1;

	s = xmalloc(size);

	va_start(ap, fmt);
	n = vsnprintf(s, size, fmt, ap);
	va_end(ap);

	if (n > -1 && n < size)
		return s;

	size = n + 1;
	s = (char *)xrealloc(s, size);
	va_start(ap, fmt);
	n = vsnprintf(s, size, fmt, ap);
	va_end(ap);

	return s;
}
