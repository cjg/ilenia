/***************************************************************************
 *            deplist.c
 *
 *  Mon Dec 20 13:58:12 2004
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

#include <stdlib.h>
#include <string.h>
#include "deplist.h"

struct deplist *deplist_add(char *name, struct deplist *d)
{
	struct deplist *daus = NULL;
	daus = (struct deplist *) malloc(sizeof(struct deplist));
	strcpy(daus->name, name);
	if (d == NULL) {
		d = daus;
		d->next = NULL;
	} else {
		daus->next = d;
		d = daus;
	}
	return (d);
}

int deplist_exists(char *param, struct deplist *d)
{
	while (d != NULL) {
		if (strcmp(d->name, param) == 0)
			return (1);
		d = d->next;
	}
	return (0);
}
