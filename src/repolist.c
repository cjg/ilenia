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

struct repolist *
repolist_add (char *repository, char *path, struct repolist *p)
{
  struct repolist *paus = NULL;
  paus = (struct repolist *) malloc (sizeof (struct repolist));
  strcpy (paus->repository, repository);
  strcpy (paus->path, path);
  if (p == NULL)
    {
      p = paus;
      p->next = NULL;
    }
  else
    {
      paus->next = p;
      p = paus;
    }
  return (p);
}

int
repolist_exists (char *delim, struct repolist *p)
{
  while (p != NULL)
    {
      if (strcmp (p->repository, delim) == 0)
	return (1);
      p = p->next;
    }
  return (0);
}

struct repolist *
repolist_find (char *delim, struct repolist *p)
{
  struct repolist *paus = NULL;
  while (p != NULL)
    {
      if (strcmp (p->repository, delim) == 0)
	{
	  paus = repolist_add (p->repository, p->path, paus);
	}
      p = p->next;
    }
  return (paus);
}
