/***************************************************************************
 *            dependencies.c
 *
 *  Sat Sep 11 18:04:21 2004
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
#include "deplist.h"
#include "manipola.h"
#include "confront.h"
#include "output.h"
#include "ilenia.h"

struct pkglist *
get_package_dependencies (char *name, char *repo)
{
  struct pkglist *pkg = NULL;
  struct pkglist *dependencies = NULL;
  pkg = pkglist_find (name, ilenia_ports);
  pkg = pkglist_find (repo, pkg);
  while (pkg->depends != NULL)
    {
      if (pkglist_exists (pkg->depends->name, ilenia_ports) == 0)
	dependencies =
	  pkglist_add_reversed (pkg->depends->name, "",
				pkglist_get_newer (pkg->
						   depends->name,
						   ilenia_ports), NULL,
				dependencies);
      else
	dependencies =
	  pkglist_add_reversed (pkg->depends->name, "",
				"not found", NULL, dependencies);
      pkg->depends = pkg->depends->next;
    }
  return (dependencies);
}

struct pkglist *
find_dependencies (struct pkglist *p)
{
  struct pkglist *dependencies = NULL;
  while (p != NULL)
    {
      if (strcmp (p->repo, "not found") != 0)
	{
	  struct pkglist *d = NULL;
	  d = get_package_dependencies (p->name, p->repo);
	  d = pkglist_add_reversed (p->name, "", p->repo, NULL, d);
	  while (d != NULL)
	    {
	      if (pkglist_exists (d->name, dependencies) != 0)
		dependencies =
		  pkglist_add_reversed (d->name, "",
					d->repo, NULL, dependencies);
	      d = d->next;
	    }
	}
      p = p->next;
    }
  return (dependencies);
}

struct pkglist *
find_dependents (struct pkglist *p)
{
  struct pkglist *dependents = NULL;
  dependents = p;
  while (p != NULL)
    {
      struct pkglist *pkgs = NULL;
      pkgs = ilenia_pkgs;
      while (pkgs != NULL)
	{
	  char *repo;
	  repo = pkglist_get_newer (pkgs->name, ilenia_ports);
	  struct pkglist *tmp = NULL;
	  if ((tmp = pkglist_find (repo, pkglist_find (pkgs->name,
						       ilenia_ports))))
	    {
	      if (deplist_exists (p->name, tmp->depends))
		{
		  if (pkglist_find (pkgs->name, dependents) == NULL)
		    dependents =
		      pkglist_add (pkgs->name, "", repo, NULL, dependents);
		}
	    }
	  pkgs = pkgs->next;
	}
      p = p->next;
    }
  return (dependents);
}

struct pkglist *
get_dependencies (char *name)
{
  struct pkglist *p = NULL;
  char *repo;
  int i = -10;

  if (pkglist_exists (name, ilenia_ports) == 0)
    {
      repo = pkglist_get_newer (name, ilenia_ports);
      p = pkglist_add_reversed (name, "", repo, NULL, p);
      while (i != pkglist_len (p))
	{
	  i = pkglist_len (p);
	  p = find_dependencies (p);
	}
    }
  else
    {
      p = pkglist_add (name, "", "not found", NULL, p);
    }
  return (p);
}

struct pkglist *
get_dependents (char *name, int all)
{
  struct pkglist *p = NULL;
  char *repo;
  int i = -10;

  repo = pkglist_get_newer (name, ilenia_ports);
  p = pkglist_add (name, "", repo, NULL, p);

  while (i != pkglist_len (p))
    {
      i = pkglist_len (p);
      p = find_dependents (p);
      if (all == 0)
	return (p);
    }
  return (p);
}

void
print_dependencies (char *name)
{
  struct pkglist *p = NULL;
  p = get_dependencies (name);
  while (p != NULL)
    {
      if (strcmp (p->repo, "not found") != 0)
	{
	  printf ("%s [", p->name);
	  if (pkglist_exists (p->name, ilenia_pkgs) == 0)
	    {
	      printf ("installed]");

	      struct pkglist *paus = NULL;
	      paus = pkglist_find (p->name, ilenia_pkgs);

	      if (strcmp (paus->version, "alias") == 0)
		printf (" (%s)\n", paus->repo);
	      else
		printf ("\n");
	    }
	  else
	    printf (" ]\n");
	}
      else
	{
	  printf ("%s [not found]\n", p->name);
	}
      p = p->next;
    }
}

void
print_dependents (char *name, int all)
{
  struct pkglist *p = NULL;
  p = get_dependents (name, all);
  while (p != NULL)
    {
      if (strcmp (p->repo, "not found") != 0)
	{
	  printf ("%s\n", p->name);
	}
      p = p->next;
    }
}
