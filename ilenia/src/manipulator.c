/***************************************************************************
 *            manipulator.c
 *
 *  Sat Jul 10 12:51:53 2004
 *  Copyright  2004 - 2005  Coviello Giuseppe
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

#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "manipulator.h"

/*
char *trim(char s[])
{
	int n;
	while (*s && *s <= 32)
		++s;
	for (n = strlen(s) - 1; n >= 0 && s[n] <= 32; --n)
		s[n] = 0;
	return s;
}*/

void ltrim(char s[])
{
	int n;

	for (n = 0; n < strlen(s); n++)
		if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
			break;
	strcpy(s, s + n);
}

void rtrim(char s[])
{
	int n;

	for (n = strlen(s) - 1; n >= 0; n--)
		if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
			break;
	s[n + 1] = '\0';
}

void trim(char s[])
{
	ltrim(s);
	rtrim(s);
}

char *mid(char s[], int start, int length)
{
	if (length == -1)
		length = strlen(s) - start;

	strncpy(s, s + start, length);
	s[length] = '\0';
	return (s);
}

char *spaces(int n)
{
	if (n < 0)
		return ("");
	char s[n];
	memset(s, ' ', n);
	s[n] = '\0';
	return (strdup(s));
}


char *sed(char *s, char *find, char *replace)
{
	char *tmp = strdup(s);
	s = (char *) malloc(strlen(tmp) + 1);
	int i = 0;
	while (*tmp) {
		if (strncmp(tmp, find, strlen(find)) != 0) {
			s[i++] = *tmp;
			tmp++;
			continue;
		}
		if (strlen(replace) > 1)
			s = (char *) realloc(s,
					     strlen(s) + strlen(tmp) +
					     strlen(replace));
		int z;
		for (z = 0; z < strlen(replace); z++)
			s[i++] = replace[z];

		tmp += strlen(find);

		s[i++] = *tmp;
		tmp++;
	}

	s[i] = '\0';
	/*if (tmp)
	   free(tmp);
	 */
	return (s);
}

char *sedchr(char s[], int find, int replace)
{
	int i;
	char *s_copy;
	s_copy = strdup(s);
	for (i = 0; i < strlen(s_copy); i++) {
		if (s_copy[i] == find)
			s[i] = replace;
		else
			s[i] = s_copy[i];
	}
	s[i] = '\0';
	if (s_copy)
		free(s_copy);
	return (s);
}

int count(char *s, int delim)
{
	int i, c = 0;
	for (i = 0; i <= strlen(s); i++) {
		if (s[i] == delim)
			c++;
	}
	return (c);
}

int old_split(char *s, char *delim, char *splitted[])
{
	int i = 0;
	char *tmp = NULL;
	while (strlen(s) > 0) {
		tmp = NULL;
		tmp = strdup(s);
		strtok(tmp, delim);
		splitted[i] = strdup(tmp);
		s = mid(s, strlen(tmp), END);
		i++;
	}
	if (tmp)
		free(tmp);
	return (i);
}


struct list *split(char *s, char *delim)
{
	struct list *l = NULL;
	while (strlen(s) > 0) {
		char *tmp;
		tmp = strdup(s);
		strtok(tmp, delim);
		s = mid(s, strlen(tmp), END);
		trim(tmp);
		l = list_add(tmp, l);
		if (tmp)
			free(tmp);
	}
	return (l);
}

void reverse(char s[])
{
	int c, i, j;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

char *itoa(int n)
{
	int i = 0, sign;
	char *s;
	s = (char *) malloc(sizeof(char));
	if ((sign = n) < 0)
		n = -n;
	do {
		s = (char *) realloc(s, i + 1);
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	if (sign < 0) {
		s = (char *) realloc(s, i + 1);
		s[i++] = '-';
	}

	s[i] = '\0';
	reverse(s);
	return strdup(s);
}

void strprintf(char **dest, char *fmt, ...)
{
	va_list ap;
	char *p, *d, *sval = NULL;
	int ival;
	va_start(ap, fmt);
	d = (char *) malloc(sizeof(char));
	for (p = fmt; *p; p++) {
		printf("%d\n", strlen(d));
		if (*p != '%') {
			d = (char *) realloc(d, strlen(d) + 2);
			d[strlen(d)] = *p;
			continue;
		}
		switch (*++p) {
		case 'd':
			ival = va_arg(ap, int);
			char *tmp = itoa(ival);
			d = (char *) realloc(d,
					     strlen(d) + strlen(tmp) + 1);
			strcat(d, tmp);
			free(tmp);
			break;
		case 's':
			sval = va_arg(ap, char *);
			d = (char *) realloc(d,
					     strlen(d) + strlen(sval) + 1);
			strcat(d, sval);
			break;
		default:
			d = (char *) realloc(d, strlen(d) + 2);
			d[strlen(d)] = *p;
		}
	}

	d[strlen(d)] = '\0';
	*dest = d;
	va_end(ap);
}
