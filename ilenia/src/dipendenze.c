/***************************************************************************
 *            dipendenze.c
 *
 *  Sat Sep 11 18:04:21 2004
 *  Copyright  2004 - 2005  Coviello Giuseppe
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

#include <stdio.h>
#include <string.h>
#include "db.h"
#include "deplist.h"
#include "lspacchetti.h"
#include "lsports.h"
#include "manipola.h"
#include "confronto.h"
#include "output.h"
#include "ilenia.h"

struct db *
dipendenze_pacchetto (char *pacchetto, char *collezione)
{
  struct db *thispkg = NULL;
  struct db *dependencies = NULL;
  thispkg = cerca (pacchetto, ports);
  thispkg = cerca (collezione, thispkg);
  while (thispkg->depends != NULL)
    {
      if (esiste (thispkg->depends->pkg, ports) == 0)
	dependencies =
	  inserisci_elemento_inverso (thispkg->depends->pkg, "",
				      il_piu_aggiornato (thispkg->
							 depends->pkg,
							 ports), NULL,
				      dependencies);
      else
	dependencies =
	  inserisci_elemento_inverso (thispkg->depends->pkg, "",
				      "not found", NULL, dependencies);
      thispkg->depends = thispkg->depends->next;
    }
  return (dependencies);
}

struct db *
cerca_dipendenze (struct db *_pacchetti)
{
  struct db *dipendenze = NULL;
  while (_pacchetti != NULL)
    {
      if (strcmp (_pacchetti->collezione, "not found") != 0)
	{
	  struct db *d = NULL;
	  d = dipendenze_pacchetto (_pacchetti->nome, _pacchetti->collezione);
	  d = inserisci_elemento_inverso (_pacchetti->nome, "",
					  _pacchetti->collezione, NULL, d);
	  while (d != NULL)
	    {
	      if (esiste (d->nome, dipendenze) != 0)
		dipendenze =
		  inserisci_elemento_inverso (d->nome, "",
					      d->collezione, NULL,
					      dipendenze);
	      d = d->prossimo;
	    }
	}
      _pacchetti = _pacchetti->prossimo;
    }
  return (dipendenze);
}

struct db *
dipendenze (char *pacchetto)
{
  struct db *d = NULL;
  char collezione[255];
  int i = -10;
  if (esiste (pacchetto, ports) == 0)
    {
      strcpy (collezione, il_piu_aggiornato (pacchetto, ports));
      d = inserisci_elemento_inverso (pacchetto, "", collezione, NULL, d);

      while (i != conta (d))
	{
	  i = conta (d);
	  d = cerca_dipendenze (d);
	}
    }
  else
    {
      d = inserisci_elemento (pacchetto, "", "not found", NULL, d);
    }
  return (d);
}

struct db *
cerca_dipendenti (struct db *_pacchetti)
{
  struct db *dependents = NULL;
  dependents = _pacchetti;
  while (_pacchetti != NULL)
    {
      struct db *p = NULL;
      p = pacchetti;
      while (p != NULL)
	{
	  char repo[255];
	  strcpy (repo, il_piu_aggiornato (p->nome, ports));
	  struct db *tmp = NULL;
	  if ((tmp = cerca (repo, cerca (p->nome, ports))))
	    {
	      if (exists (_pacchetti->nome, tmp->depends))
		{
		  if (cerca (p->nome, dependents) == NULL)
		    dependents =
		      inserisci_elemento (p->nome, "", repo, NULL,
					  dependents);
		}
	    }
	  p = p->prossimo;
	}
      _pacchetti = _pacchetti->prossimo;
    }
  return (dependents);
}

struct db *
dipendenti (char *pacchetto, int all)
{
  struct db *d = NULL;
  char collezione[255];
  int i = -10;
  strcpy (collezione, il_piu_aggiornato (pacchetto, ports));
  d = inserisci_elemento (pacchetto, "", collezione, NULL, d);

  while (i != conta (d))
    {
      i = conta (d);
      d = cerca_dipendenti (d);
      if (all == 0)
	return (d);
    }
  return (d);
}

void
stampa_dipendenze (char *pacchetto)
{
  struct db *d = NULL;
  d = dipendenze (pacchetto);
  while (d != NULL)
    {
      if (strcmp (d->collezione, "not found") != 0)
	{
	  printf ("%s [", d->nome);
	  if (esiste (d->nome, pacchetti) != 0)
	    printf (" ]\n");
	  else
	    printf ("installed]\n");
	}
      else
	{
	  printf ("%s [not found]\n", d->nome);
	}
      d = d->prossimo;
    }
}

void
stampa_dipendenti (char *pacchetto, int all)
{
  struct db *d = NULL;
  d = dipendenti (pacchetto, all);
  while (d != NULL)
    {
      if (strcmp (d->collezione, "not found") != 0)
	{
	  printf ("%s\n", d->nome);
	}
      d = d->prossimo;
    }
}
