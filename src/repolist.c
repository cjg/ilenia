/***************************************************************************
 *            repolist.c
 *
 *  Sun Mar 20 12:04:42 2005
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
#include "repolist.h"

struct repolist *repolist_add(char *name, char *path, struct repolist *r)
{
	struct repolist *raus = NULL;
	raus = (struct repolist *) malloc(sizeof(struct repolist));
	strcpy(raus->name, name);
	strcpy(raus->path, path);
	if (r == NULL) {
		r = raus;
		r->next = NULL;
	} else {
		raus->next = r;
		r = raus;
	}
	return (r);
}

struct repolist *repolist_find(char *param, struct repolist *r)
{
	struct repolist *raus = NULL;
	while (r != NULL) {
		if (strcmp(r->name, param) == 0) {
			raus = repolist_add(r->name, r->path, raus);
		}
		r = r->next;
	}
	return (raus);
}

int repolist_exists(char *param, struct repolist *r)
{
	while (r != NULL) {
		if (strcmp(r->name, param) == 0)
			return (EXIT_SUCCESS);
		r = r->next;
	}
	return (EXIT_FAILURE);
}
