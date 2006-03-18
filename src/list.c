/***************************************************************************
 *            list.c
 *
 *  Thu Nov 17 13:39:56 2005
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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <string.h>
#include "ilenia.h"
#include "list.h"

struct list *list_add(char *data, struct list *l)
{
	struct list *laus1 = NULL;
	laus1 = (struct list *) malloc(sizeof(struct list));
	//laus1->data = (char *) malloc(strlen(data));
	strcpy(laus1->data, data);
	if (l == NULL) {
		l = laus1;
		l->next = NULL;
	} else {
		struct list *laus2 = NULL;
		laus2 = l;
		while (laus2->next != NULL)
			laus2 = laus2->next;
		laus1->next = NULL;
		laus2->next = laus1;
	}
	return (l);
}

struct list *list_find(char *param, struct list *l)
{
	struct list *laus = NULL;
	while (l != NULL) {
		if (strcmp(l->data, param) == 0) {
			laus = list_add(l->data, laus);
		}
		l = l->next;
	}
	return (laus);
}

int list_exists(char *param, struct list *l)
{
	while (l != NULL) {
		if (!strcmp(l->data, param))
			return TRUE;
		l = l->next;
	}
	return FALSE;
}
