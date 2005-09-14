/*
 *  list.c
 *  Wed Sep 14 12:40:24 2005
 *  Copyright 2005 - 2006 Coviello Giuseppe
 *  immigrant@email.it
 *
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
#include "list.h"

struct list *
add (char *s, struct list *l)
{
  struct list *paus = NULL;
  paus = (struct list *) malloc (sizeof (struct list));
  strcpy (paus->item, s);
  if (l == NULL)
    {
      l = paus;
      l->next = NULL;
    }
  else
    {
      paus->next = l;
      l = paus;
    }
  return (l);
}

int
exists (char *item, struct list *l)
{
  while (l != NULL)
    {
      if (strcmp (l->item, item) == 0)
	return (1);
      l = l->next;
    }
  return (0);
}
