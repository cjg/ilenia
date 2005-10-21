/***************************************************************************
 *            confront.c
 *
 *  Sat Jul 10 13:33:11 2004
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pkglist.h"
#include "manipulator.h"
#include "vercmp.h"
#include "confront.h"

void
prettyprint (char *str1, char *str2, char *str3, char *str4)
{
  str1 = mid (str1, 0, 26);
  str2 = mid (str2, 0, 16);
  str3 = mid (str3, 0, 28);
  str4 = mid (str4, 0, 16);

  strcat (str1, spaces (26 - strlen (str1)));
  strcat (str2, spaces (18 - strlen (str2)));
  strcat (str3, spaces (14 - strlen (str3)));
  strcat (str4, spaces (18 - strlen (str4)));

  printf ("%s %s %s %s\n", str1, str2, str3, str4);
}

struct pkglist *
pkglist_confront (struct pkglist *pkgs, struct pkglist *ports,
		  int type, int options, int print)
{
  struct pkglist *paus = NULL;
  struct pkglist *confront = NULL;
  if (print)
    printf
      ("Name                       Installed Version  Repository     Port Version \n");
  while (pkgs != NULL)
    {
      paus = ports;
      while (paus != NULL)
	{
	  if (strcmp (paus->name, pkgs->name) == 0)
	    {
	      int test;
	      int skip = 0;
	      if (type == DIFF)
		{
		  test = strcmp (pkgs->version, paus->version);
		}
	      else
		{
		  test = vercmp (pkgs->version, paus->version);
		}
	      if ((pkgs->repo[0] == 'R')
		  && (options != NO_FAVORITE_REPO && options != NO_FAVORITES))
		{
		  if (strcmp (mid (pkgs->repo, 2, END), paus->repo) != 0)
		    skip = 1;
		}
	      if ((pkgs->repo[0] == 'V')
		  && (options != NO_FAVORITE_VERSION
		      && options != NO_FAVORITES))
		{
		  if (strcmp (mid (pkgs->repo, 2, END), pkgs->version) == 0)
		    skip = 1;
		}
	      if (test != 0 && skip != 1)
		{
		  confront =
		    pkglist_add_ordered (pkgs->
					 name,
					 paus->
					 version, paus->repo, NULL, confront);
		  if (print)
		    prettyprint (pkgs->name,
				 pkgs->version, paus->repo, paus->version);
		}
	    }
	  paus = paus->next;
	}
      pkgs = pkgs->next;
    }
  return (confront);
}


char *
pkglist_get_newer (char *name, struct pkglist *p)
{
  char *version = NULL;
  static char *repo = NULL;
  while (p != NULL)
    {
      if (strcmp (name, p->name) == 0)
	{
	  if (!version)
	    {
	      version = strdup (p->version);
	      repo = strdup (p->repo);
	    }
	  else
	    {
	      if (vercmp (version, p->version))
		{
		  version = strdup (p->version);
		  repo = strdup (p->repo);
		}
	    }
	}
      p = p->next;
    }
  return (repo);
}

char *
pkglist_get_from_version (char *name, char *version, struct pkglist *p)
{
  while (p != NULL)
    {
      if (strcmp (name, p->name) == 0)
	{
	  if (strcmp (version, p->version) == 0)
	    return (p->repo);
	}
      p = p->next;
    }
  return (NULL);
}

char *
pkglist_get_from_repo (char *name, char *repo, struct pkglist *p)
{
  while (p != NULL)
    {
      if (strcmp (name, p->name) == 0)
	{
	  if (strcmp (repo, p->repo) == 0)
	    return (p->version);
	}
      p = p->next;
    }
  return (NULL);
}
