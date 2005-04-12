/***************************************************************************
 *            aliaslist.c
 *
 *  Tue Avr 12 16:03:52 2005
 *  Copyright  2005 - 2006  Coviello Giuseppe
 *  slash@crux-it.org
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
#include "manipola.h"

struct aliaslist *
aliaslist_add (char *pkg, struct aliaslist *p)
{
  struct aliaslist *paus = NULL;
  paus = (struct aliaslist *) malloc (sizeof (struct aliaslist));
  strcpy (paus->pkg, pkg);
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

struct aliaseslist *
aliaseslist_add (struct aliaslist *a, struct aliaseslist *p)
{
  struct aliaseslist *paus = NULL;
  paus = (struct aliaseslist *) malloc (sizeof (struct aliaseslist));
  paus->alias = a;
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

struct aliaslist *
aliaslist_get (char *delim, struct aliaseslist *as)
{
  while (as != NULL)
    {
      if (aliaslist_exists (delim, as->alias))
	return (as->alias);
      as = as->next;
    }
  return (NULL);
}

int
aliaslist_exists (char *delim, struct aliaslist *p)
{
  while (p != NULL)
    {
      if (strcmp (p->pkg, delim) == 0)
	return (1);
      p = p->next;
    }
  return (0);
}

struct aliaseslist *
aliaseslist_build ()
{
  struct aliaseslist *as = NULL;
  FILE *aliasfile;
  char row[255];

  if ((aliasfile = fopen (ALIAS_FILE, "r")))
    {
      while (fgets (row, 255, aliasfile))
	{
	  if (strlen (row) > 0)
	    {
	      char alias[255][MASSIMO];
	      int how_many_alias, i;
	      struct aliaslist *a = NULL;
	      how_many_alias = split (row, " ", alias);
	      for (i = 0; i < how_many_alias; i++)
		a = aliaslist_add (trim (alias[i]), a);
	      as = aliaseslist_add (a, as);
	    }
	}

    }
  return (as);
}
