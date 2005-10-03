/***************************************************************************
 *            pkglist.c
 *
 *  Mon Oct 03 12:45:04 2005
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
#include <stdlib.h>
#include <string.h>
#include "pkglist.h"
#include "deplist.h"

struct pkglist *
pkglist_add (char *name, char *version, char *repo, struct deplist *d,
	     struct pkglist *p)
{
  struct pkglist *paus = NULL;
  paus = (struct pkglist *) malloc (sizeof (struct pkglist));
  strcpy (paus->name, name);
  strcpy (paus->version, version);
  strcpy (paus->repo, repo);
  paus->depends = d;
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

struct pkglist *
pkglist_add_ordered (char *name, char *version, char *repo, struct deplist *d,
		     struct pkglist *p)
{
  struct pkglist *paus1 = NULL;
  struct pkglist *paus2 = NULL;
  int pos;
  paus1 = (struct pkglist *) malloc (sizeof (struct pkglist));
  strcpy (paus1->name, name);
  strcpy (paus1->version, version);
  strcpy (paus1->repo, repo);
  paus1->depends = d;
  if (p == NULL)
    {
      p = paus1;
      p->next = NULL;
    }
  else
    {
      if (strcmp (p->name, paus1->name) > 0)
	{
	  paus1->next = p;
	  p = paus1;
	}
      else
	{
	  paus2 = p;
	  pos = 0;
	  while (paus2->next != NULL && pos != 1)
	    {
	      if (strcmp (paus2->next->name, paus1->name) < 0)
		paus2 = paus2->next;
	      else
		pos = 1;
	    }
	  paus1->next = paus2->next;
	  paus2->next = paus1;
	}
    }
  return (p);
}

struct pkglist *
pkglist_add_reversed (char *name, char *version,
		      char *repo, struct deplist *d, struct pkglist *p)
{
  struct pkglist *paus1 = NULL;
  paus1 = (struct pkglist *) malloc (sizeof (struct pkglist));
  strcpy (paus1->name, name);
  strcpy (paus1->version, version);
  strcpy (paus1->repo, repo);
  paus1->depends = d;
  if (p == NULL)
    {
      p = paus1;
      p->next = NULL;
    }
  else
    {
      struct pkglist *paus2 = NULL;
      paus2 = p;
      while (paus2->next != NULL)
	paus2 = paus2->next;
      paus1->next = NULL;
      paus2->next = paus1;
    }
  return (p);
}

struct pkglist *
pkglist_add_deplist (struct deplist *d, struct pkglist *p)
{
  p->depends = d;
  return (p);
}

struct pkglist *
pkglist_find (char *param, struct pkglist *p)
{
  struct pkglist *paus = NULL;
  while (p != NULL)
    {
      if ((strcmp (p->name, param) == 0)
	  || (strcmp (p->repo, param) == 0)
	  || (strcmp (p->version, param) == 0))
	{
	  paus =
	    pkglist_add_ordered (p->name, p->version,
				 p->repo, p->depends, paus);
	}
      p = p->next;
    }
  return (paus);
}

struct pkglist *
pkglist_find_like (char *param, struct pkglist *p)
{
  struct pkglist *paus = NULL;
  while (p != NULL)
    {
      if (strstr (p->name, param))
	{
	  paus =
	    pkglist_add_ordered (p->name, p->version,
				 p->repo, p->depends, paus);
	}
      p = p->next;
    }
  return (paus);
}

struct pkglist *
pkglist_select_from_repo (char *repo, struct pkglist *p)
{
  struct pkglist *paus = NULL;
  while (p != NULL)
    {
      if (strcmp (p->repo, repo) == 0)
	paus = pkglist_add_ordered (p->name, p->version, p->repo,
				    p->depends, paus);
      p = p->next;
    }
  return (paus);
}

int
pkglist_len (struct pkglist *p)
{
  int i;
  struct pkglist *paus;
  for (paus = p, i = 0; paus; paus = paus->next, i++);
  return (i);
}

int
pkglist_exists (char *param, struct pkglist *p)
{
  while (p != NULL)
    {
      if (strcmp (p->name, param) == 0
	  || strcmp (p->version, param) == 0 || strcmp (p->repo, param) == 0)
	return (0);
      p = p->next;
    }
  return (-1);
}
