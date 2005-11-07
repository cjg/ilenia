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
		if (aliaslist_exists(param, s->alias))
			return (s->alias);
		s = s->next;
	}
	return (NULL);
}

int aliaslist_exists(char *param, struct aliaslist *a)
{
	while (a != NULL) {
		if (strcmp(a->name, param) == 0)
			return (1);
		a = a->next;
	}
	return (0);
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

	if ((aliasfile = fopen(ALIAS_FILE, "r"))) {
		size_t n = 0;
		char *line = NULL;
		int nread = getline(&line, &n, aliasfile);
		while (nread > 0) {
			trim(line);
			if (strlen(line) > 0 && line[0] != '#') {
				int i, num_alias;
				struct aliaslist *a = NULL;
				num_alias = count(line, ' ');
				char *alias[num_alias];
				split(line, " ", alias);
				for (i = 0; i < num_alias; i++) {
					trim(alias[i]);

					a = aliaslist_add(alias[i],
							  a);
				}
				s = aliaseslist_add(a, s);
			}
			nread = getline(&line, &n, aliasfile);
		}
		line = NULL;
		free(line);
	}
	return (s);
}
