/***************************************************************************
 *            aliaslist.c
 *
 *  Tue Avr 12 16:03:52 2005
 *  Copyright  2005 - 2006  Coviello Giuseppe
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "aliaslist.h"
#include "ilenia.h"
#include "manipulator.h"

struct aliaslist *aliaslist_add(char *name, struct aliaslist *a)
{
	struct aliaslist *aaus = NULL;
	aaus = (struct aliaslist *) malloc(sizeof(struct aliaslist));
	strcpy(aaus->name, name);
	if (a == NULL) {
		a = aaus;
		a->next = NULL;
	} else {
		aaus->next = a;
		a = aaus;
	}
	return (a);
}

struct aliaslist *aliaslist_get(char *param, struct aliaseslist *s)
{
	while (s != NULL) {
		if (aliaslist_exists(param, s->alias) == EXIT_SUCCESS)
			return (s->alias);
		s = s->next;
	}
	return (NULL);
}

int aliaslist_exists(char *param, struct aliaslist *a)
{
	while (a != NULL) {
		if (strcmp(a->name, param) == 0)
			return (EXIT_SUCCESS);
		a = a->next;
	}
	return (EXIT_FAILURE);
}

struct aliaseslist *aliaseslist_add(struct aliaslist *a,
				    struct aliaseslist *s)
{
	struct aliaseslist *saus = NULL;
	saus = (struct aliaseslist *) malloc(sizeof(struct aliaseslist));
	saus->alias = a;
	if (s == NULL) {
		s = saus;
		s->next = NULL;
	} else {
		saus->next = s;
		s = saus;
	}
	return (s);
}

struct aliaseslist *aliaseslist_build()
{
	struct aliaseslist *s = NULL;
	FILE *aliasfile;
	aliasfile = fopen(ALIAS_FILE, "r");
	if (aliasfile == NULL)
		return (NULL);
	size_t n = 0;
	char *line = NULL;
	int nread;

	while ((nread = getline(&line, &n, aliasfile)) > 0) {
		trim(line);
		if (!(strlen(line) > 0 && line[0] != '#'))
			continue;

		struct aliaslist *alias = NULL;
		struct list *tmp = NULL;
		tmp = split(line, " ");

		while (tmp != NULL) {
			alias = aliaslist_add(tmp->data, alias);
			tmp = tmp->next;
		}
		s = aliaseslist_add(alias, s);
	}

	if (line)
		free(line);

	return (s);
}
