/* output.c */

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
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "output.h"
#include "memory.h"
#include "str.h"

#define BLACK   "\033[0;30m"
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define BLUE    "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN    "\033[0;36m"
#define WHITE   "\033[0;37m"
#define DEFAULT "\033[0;00m"

char *colors[][2] = {
	{"[BLACK]", BLACK},
	{"[RED]", RED},
	{"[GREEN]", GREEN},
	{"[YELLOW]", YELLOW},
	{"[BLUE]", BLUE},
	{"[MAGENTA]", MAGENTA},
	{"[CYAN]", CYAN},
	{"[WHITE]", WHITE},
	{"[DEFAULT]", DEFAULT}
};

#define NUM_COLORS 9

static int vcprintf(FILE * stream, const char *format, va_list ap, int colored)
{
	int n;
	int size;
	char *s;
	char *sptr;
	va_list ap2;

	size = strlen(format) + 1;
	s = xstrdup(format);

	va_copy(ap2, ap);
	n = vsnprintf(s, size, format, ap2);
	va_end(ap2);

	if (n >= size) {
		size = n + 1;
		s = (char *)xrealloc(s, size);
		va_copy(ap2, ap);
		n = vsnprintf(s, size, format, ap);
		va_end(ap2);
	}

	for (sptr = s; *sptr != 0; sptr++) {
		for (n = 0; n < NUM_COLORS; n++) {
			if (strncmp(sptr, colors[n][0], strlen(colors[n][0])) !=
			    0)
				continue;
			sptr += strlen(colors[n][0]);
			if (colored)
				fprintf(stream, "%s", colors[n][1]);
			break;
		}
		fprintf(stream, "%c", *sptr);
	}

	free(s);
	return n;
}

int coloredprintf(FILE * stream, const char *format, ...)
{
	int ret;
	va_list ap;

	assert(format);

	if (!strlen(format))
		return printf(format);

	va_start(ap, format);
	ret = vcprintf(stream, format, ap, 1);
	va_end(ap);

	return ret;
}

int uncoloredprintf(FILE * stream, const char *format, ...)
{
	int ret;
	va_list ap;

	assert(format);

	if (!strlen(format))
		return printf(format);

	va_start(ap, format);
	ret = vcprintf(stream, format, ap, 0);
	va_end(ap);

	return ret;
}

void warning(const char *format, ...)
{
	va_list ap;
	assert(format);
	va_start(ap, format);
	cprintf(stderr, "[YELLOW]Warning: ");
	if (cprintf == coloredprintf)
		vcprintf(stderr, format, ap, 1);
	else
		vcprintf(stderr, format, ap, 0);
	cprintf(stderr, "[DEFAULT]\n");
	va_end(ap);
}

void error(const char *format, ...)
{
	va_list ap;
	assert(format);
	va_start(ap, format);
	cprintf(stderr, "[RED]Error: ");
	if (cprintf == coloredprintf)
		vcprintf(stderr, format, ap, 1);
	else
		vcprintf(stderr, format, ap, 0);
	cprintf(stderr, "[DEFAULT]\n");
	va_end(ap);
}
