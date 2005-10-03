/***************************************************************************
 *            dipendenze.c
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
#include "lspacchetti.h"
#include "lsports.h"
#include "manipola.h"
#include "confronto.h"
#include "output.h"
#include "ilenia.h"

struct pkglist *
dipendenze_pacchetto (char *pacchetto, char *collezione)
{
  struct pkglist *thispkg = NULL;
  struct pkglist *dependencies = NULL;
  thispkg = pkglist_find (pacchetto, ports);
  thispkg = pkglist_find (collezione, thispkg);
  while (thispkg->depends != NULL)
    {
      if (pkglist_exists (thispkg->depends->pkg, ports) == 0)
	dependencies =
	  pkglist_add_reversed (thispkg->depends->pkg, "",
				pkglist_get_newer (thispkg->
						   depends->pkg,
						   ports), NULL,
				dependencies);
      else
	dependencies =
	  pkglist_add_reversed (thispkg->depends->pkg, "",
				"not found", NULL, dependencies);
      thispkg->depends = thispkg->depends->next;
    }
  return (dependencies);
}

struct pkglist *
cerca_dipendenze (struct pkglist *_pacchetti)
{
  struct pkglist *dipendenze = NULL;
  while (_pacchetti != NULL)
    {
      if (strcmp (_pacchetti->repo, "not found") != 0)
	{
	  struct pkglist *d = NULL;
	  d = dipendenze_pacchetto (_pacchetti->name, _pacchetti->repo);
	  d = pkglist_add_reversed (_pacchetti->name, "",
				    _pacchetti->repo, NULL, d);
	  while (d != NULL)
	    {
	      if (pkglist_exists (d->name, dipendenze) != 0)
		dipendenze =
		  pkglist_add_reversed (d->name, "",
					d->repo, NULL, dipendenze);
	      d = d->next;
	    }
	}
      _pacchetti = _pacchetti->next;
    }
  return (dipendenze);
}

struct pkglist *
dipendenze (char *pacchetto)
{
  struct pkglist *d = NULL;
  char collezione[255];
  int i = -10;
  if (pkglist_exists (pacchetto, ports) == 0)
    {
      strcpy (collezione, pkglist_get_newer (pacchetto, ports));
      d = pkglist_add_reversed (pacchetto, "", collezione, NULL, d);

      while (i != pkglist_len (d))
	{
	  i = pkglist_len (d);
	  d = cerca_dipendenze (d);
	}
    }
  else
    {
      d = pkglist_add (pacchetto, "", "not found", NULL, d);
    }
  return (d);
}

struct pkglist *
cerca_dipendenti (struct pkglist *_pacchetti)
{
  struct pkglist *dependents = NULL;
  dependents = _pacchetti;
  while (_pacchetti != NULL)
    {
      struct pkglist *p = NULL;
      p = pacchetti;
      while (p != NULL)
	{
	  char repo[255];
	  strcpy (repo, pkglist_get_newer (p->name, ports));
	  struct pkglist *tmp = NULL;
	  if ((tmp = pkglist_find (repo, pkglist_find (p->name, ports))))
	    {
	      if (exists (_pacchetti->name, tmp->depends))
		{
		  if (pkglist_find (p->name, dependents) == NULL)
		    dependents =
		      pkglist_add (p->name, "", repo, NULL, dependents);
		}
	    }
	  p = p->next;
	}
      _pacchetti = _pacchetti->next;
    }
  return (dependents);
}

struct pkglist *
dipendenti (char *pacchetto, int all)
{
  struct pkglist *d = NULL;
  char collezione[255];
  int i = -10;
  strcpy (collezione, pkglist_get_newer (pacchetto, ports));
  d = pkglist_add (pacchetto, "", collezione, NULL, d);

  while (i != pkglist_len (d))
    {
      i = pkglist_len (d);
      d = cerca_dipendenti (d);
      if (all == 0)
	return (d);
    }
  return (d);
}

void
stampa_dipendenze (char *pacchetto)
{
  struct pkglist *d = NULL;
  d = dipendenze (pacchetto);
  while (d != NULL)
    {
      if (strcmp (d->repo, "not found") != 0)
	{
	  printf ("%s [", d->name);
	  if (pkglist_exists (d->name, pacchetti) == 0)
	    {
	      struct pkglist *pa = NULL;
	      pa = pkglist_find (d->name, pacchetti);
	      printf ("installed]");
	      if (strcmp (pa->version, "alias") == 0)
		printf (" (%s)\n", pa->repo);
	      else
		printf ("\n");
	    }
	  else
	    printf (" ]\n");
	}
      else
	{
	  printf ("%s [not found]\n", d->name);
	}
      d = d->next;
    }
}

void
stampa_dipendenti (char *pacchetto, int all)
{
  struct pkglist *d = NULL;
  d = dipendenti (pacchetto, all);
  while (d != NULL)
    {
      if (strcmp (d->repo, "not found") != 0)
	{
	  printf ("%s\n", d->name);
	}
      d = d->next;
    }
}
