/* str.c */

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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "memory.h"

int strequ(const char *s1, const char *s2)
{
	if (strcmp(s1, s2) == 0)
		return 1;
	return 0;
}

char *strltrim(char *s)
{
	char *ptr_s;
	ptr_s = s;
	for (; isspace(*ptr_s); ptr_s++) ;
	bcopy(ptr_s, s, strlen(ptr_s) + 1);
	return s;
}

char *strrtrim(char *s)
{
	int n;

	for (n = strlen(s) - 1; n >= 0; n--)
		if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
			break;
	s[n + 1] = '\0';

	return s;
}

char *strtrim(char *s)
{
	char *ptr_s, *tmp;
	int n;

	ptr_s = s;
	for (; isspace(*ptr_s); ptr_s++) ;

	for (n = strlen(ptr_s); n > 0 && isspace(*(ptr_s + n - 1)); n--) ;
	if (n < 0)
		n = 0;

	tmp = xstrdup(ptr_s);
	strcpy(s, tmp);
	*(s + n) = 0;
	free(tmp);

	return s;
}

char *strappend(char *dest, const char *src)
{
	assert(dest != NULL);
	assert(src != NULL);
	dest = xrealloc(dest, strlen(dest) + strlen(src) + 1);
	strcat(dest, src);
	return dest;
}

char *strprepend(char *dest, const char *src)
{
	assert(dest && src);
	dest = xrealloc(dest, strlen(dest) + strlen(src) + 1);
	bcopy(dest, dest + strlen(src), strlen(dest) + 1);
	memcpy(dest, src, strlen(src));
	return dest;
}

char *strreplace(char *s, const char *from, const char *to, int max)
{
	int fromlen, tolen, buflen, i;
	char *sptr, *buf, *tptr;

	assert(s != NULL && from != NULL && to != NULL);

	if (strstr(s, from) == NULL)
		return s;

	fromlen = strlen(from);
	tolen = strlen(to);
	buflen = strlen(s);

	if ((buflen / fromlen) * (tolen + 1) > buflen)
		buflen = (buflen / fromlen) * (tolen + 1);

	buf = xmalloc(buflen);
	memset(buf, 0, buflen);
	i = 0;
	sptr = s;
	while ((tptr = strstr(sptr, from)) != NULL && max-- != 0) {
		strncpy(buf + i, sptr, tptr - sptr);
		i += tptr - sptr + tolen;
		strcat(buf, to);
		sptr = tptr + fromlen;
	}

	if (*sptr != 0)
		strncat(buf, sptr, strlen(sptr));

	if (strlen(buf) > strlen(s))
		s = xrealloc(s, strlen(buf) + 1);

	strcpy(s, buf);
	free(buf);
	return s;
}

/* char *strreplace(char *s, const char *find, const char *replace, int n_replace) */
/* { */
/* 	char *sptr, *tmp; */
/* 	int find_len; */
/* 	int replace_len; */
/* 	int diff_len; */
/* 	size_t size, len; */

/* 	assert(s != NULL && find != NULL && replace != NULL); */

/* 	find_len = strlen(find); */
/* 	replace_len = strlen(replace); */
/* 	diff_len = replace_len - find_len; */
/* 	size = len = strlen(s); */

/* 	for (sptr = s; strlen(sptr) >= find_len && n_replace; sptr++) { */
/* 		if (strncmp(sptr, find, find_len)) */
/* 			continue; */

/* 		if (diff_len > 0 && size < len + diff_len) { */
/* 			size = len + diff_len + 1; */
/* 			s = xrealloc(s, size); */
/* 		} */

/* 		if (diff_len) { */
/* 			len += diff_len; */
/* 			tmp = xstrdup(sptr + find_len); */
/* 			strcpy(sptr + replace_len, tmp); */
/* 			free(tmp); */
/* 		} */

/* 		memcpy(sptr, replace, replace_len); */

/* 		sptr += replace_len; */
/* 		--n_replace; */
/* 	} */

/* 	return s; */
/* } */

#define strreplaceall(s, find, replace) strreplace((s), (find), (replace), ALL)

char *strupper(char *s)
{
	char *sptr = s;
	for (; *sptr; s++)
		if (*sptr >= 'a' && *sptr <= 'z')
			*sptr -= 'a' - 'A';
	return s;
}

char *strlower(char *s)
{
	char *sptr = s;
	for (; *sptr; sptr++)
		if (*sptr >= 'A' && *sptr <= 'Z')
			*sptr += 'a' - 'A';
	return s;
}

int strsplit(const char *str, const char sep, char ***ptr_array)
{
	int n_splits;
	int offset;
	int token_size;
	int str_len;
	char *index_ptr;

	offset = 0;
	n_splits = 0;
	str_len = strlen(str);

	while ((index_ptr = index(str + offset, sep))) {
		++n_splits;

		*ptr_array = (char **)xrealloc(*ptr_array,
					       n_splits * sizeof(char *));

		token_size = str_len - strlen(index_ptr) - offset + 1;
		*(*ptr_array + (n_splits - 1)) = (char *)xmalloc(token_size);
		memccpy(*(*ptr_array + (n_splits - 1)), str + offset, sep,
			token_size);
		*(*(*ptr_array + (n_splits - 1)) + token_size - 1) = 0;

		offset = str_len - strlen(index_ptr) + 1;
	}

	if (offset < str_len) {
		++n_splits;

		*ptr_array = (char **)xrealloc(*ptr_array,
					       n_splits * sizeof(char *));

		*(*ptr_array + (n_splits - 1)) = (char *)xmalloc(str_len -
								 offset + 1);
		strcpy(*(*ptr_array + (n_splits - 1)), str + offset);
	}

	return n_splits;
}

char *strreadline(FILE * stream)
{
	int i;
	char c, *ptr_s;

	for (i = 0, ptr_s = NULL; !feof(stream);) {
		c = fgetc(stream);
		if (c == '\n' || c == '\r')
			break;
		ptr_s = xrealloc(ptr_s, i + 1);
		*(ptr_s + i++) = c;
	}

	ptr_s = xrealloc(ptr_s, i + 1);
	*(ptr_s + i) = '\0';

	if (strlen(ptr_s) == 0) {
		free(ptr_s);
		return NULL;
	}

	return ptr_s;
}
