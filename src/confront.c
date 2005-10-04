/***************************************************************************
 *            confronto.c
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
#include "pkglist.h"
#include "manipola.h"
#include "vercmp.h"
#include "confronto.h"

void
prettyprint (char *str1, char *str2, char *str3, char *str4)
{
  char tmp[255];
  strcpy (tmp, str1);
  strcat (tmp, spazi (21 - strlen (str1)));
  strcat (tmp, spazi (2));
  strcat (tmp, spazi (17 - strlen (str2)));
  strcat (tmp, str2);
  strcat (tmp, spazi (2));
  strcat (tmp, str3);
  strcat (tmp, spazi (19 - strlen (str3)));
  strcat (tmp, spazi (2));
  strcat (tmp, spazi (16 - strlen (str4)));
  strcat (tmp, str4);
  printf ("%s\n", tmp);
}

struct pkglist *
pkglist_confront (struct pkglist *pkgs, struct pkglist *ports,
		  int type, int options, int print)
{
  struct pkglist *paus = NULL;
  struct pkglist *confront = NULL;
  if (print)
    printf
      ("Name                   Installed Version  Repository               Port Version \n");
  while (pkgs != NULL)
    {
      paus = ports;
      while (paus != NULL)
	{
	  if (strcmp (paus->name, pkgs->name) == 0)
	    {
	      int test;
	      int skip = 0;
	      if (type == DIFFERENZE)
		{
		  test = strcmp (pkgs->version, paus->version);
		}
	      else
		{
		  test = vercmp (pkgs->version, paus->version);
		}
	      if ((pkgs->repo[0] == 'R')
		  && (options != NO_REPO && options != NO_FAVORITE))
		{
		  if (strcmp (mid (pkgs->repo, 2, FINE), paus->repo) != 0)
		    skip = 1;
		}
	      if ((pkgs->repo[0] == 'V')
		  && (options != NO_VERSION && options != NO_FAVORITE))
		{
		  if (strcmp (mid (pkgs->repo, 2, FINE), pkgs->version) == 0)
		    skip = 1;
		}
	      if (test != 0 && skip != 1)
		{
		  confront = pkglist_add_ordered (pkgs->name,
						  paus->version,
						  paus->repo, NULL, confront);
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
  char version[255] = "";
  static char repo[255] = "";
  while (p != NULL)
    {
      if (strcmp (name, p->name) == 0)
	{
	  if (strcmp (version, "") == 0)
	    {
	      strcpy (version, p->version);
	      strcpy (repo, p->repo);
	    }
	  else
	    {
	      if (vercmp (version, p->version))
		{
		  strcpy (version, p->version);
		  strcpy (repo, p->repo);
		}
	    }
	}
      p = p->next;
    }
  return ((char *) repo);
}

char *
pkglist_get_from_version (char *name, char *version, struct pkglist *p)
{
  static char repo[255] = "";
  while (p != NULL)
    {
      if (strcmp (name, p->name) == 0)
	{
	  if (strcmp (version, p->version) == 0)
	    {
	      strcpy (repo, p->repo);
	      return (p->repo);
	    }
	}
      p = p->next;
    }
  return ((char *) repo);
}

char *
pkglist_get_from_repo (char *name, char *repo, struct pkglist *p)
{
  static char version[255] = "";
  while (p != NULL)
    {
      if (strcmp (name, p->name) == 0)
	{
	  if (strcmp (repo, p->repo) == 0)
	    {
	      strcpy (version, p->version);
	      return (p->version);
	    }
	}
      p = p->next;
    }
  return ((char *) version);
}
