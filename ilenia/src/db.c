/***************************************************************************
 *            db.c
 *
 *  Sat Jul 10 12:04:21 2004
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
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "deplist.h"

struct db *
add_deplist (struct deplist *d, struct db *p)
{
  p->depends = d;
  return (p);
}

int
esiste (char *qualcosa, struct db *p)
{
  while (p != NULL)
    {
      if (strcmp (p->nome, qualcosa) == 0
	  || strcmp (p->versione, qualcosa) == 0
	  || strcmp (p->collezione, qualcosa) == 0)
	return (0);
      p = p->prossimo;
    }
  return (-1);
}

struct db *
rimuovi_elemento (char *nome, struct db *p)
{
  struct db *canc = NULL;
  struct db *temp = NULL;
  struct db *paz = NULL;
  temp = p;
  while (temp->prossimo && strcmp (temp->prossimo->nome, nome) != 0)
    {
      paz =
	inserisci_elemento_inverso (temp->nome, temp->versione,
				    temp->collezione, temp->depends, paz);
      temp = temp->prossimo;
    }
  paz =
    inserisci_elemento_inverso (temp->nome, temp->versione,
				temp->collezione, temp->depends, paz);
  if (strcmp (temp->prossimo->nome, nome) == 0)
    {
      canc = temp->prossimo->prossimo;
    }
  while (canc != NULL)
    {
      paz =
	inserisci_elemento_inverso (canc->nome, canc->versione,
				    canc->collezione, canc->depends, paz);
      canc = canc->prossimo;
    }
  return (paz);
}

struct db *
inserisci_elemento (char *_nome, char *_versione,
		    char *_collezione, struct deplist *d, struct db *p)
{
  struct db *paus = NULL;
  paus = (struct db *) malloc (sizeof (struct db));
  strcpy (paus->nome, _nome);
  strcpy (paus->versione, _versione);
  strcpy (paus->collezione, _collezione);
  paus->depends = d;
  if (p == NULL)
    {
      p = paus;
      p->prossimo = NULL;
    }
  else
    {
      paus->prossimo = p;
      p = paus;
    }
  return (p);
}

struct db *
inserisci_elemento_inverso (char *_nome, char *_versione,
			    char *_collezione, struct deplist *d,
			    struct db *p)
{
  struct db *paus = NULL;
  paus = (struct db *) malloc (sizeof (struct db));
  strcpy (paus->nome, _nome);
  strcpy (paus->versione, _versione);
  strcpy (paus->collezione, _collezione);
  paus->depends = d;
  if (p == NULL)
    {
      p = paus;
      p->prossimo = NULL;
    }
  else
    {
      struct db *paz = NULL;
      paz = p;
      while (paz->prossimo != NULL)
	paz = paz->prossimo;
      paus->prossimo = NULL;
      paz->prossimo = paus;
    }
  return (p);
}

struct db *
inserisci_elemento_ordinato (char *_nome, char *_versione,
			     char *_collezione,
			     struct deplist *d, struct db *p)
{
  struct db *paus = NULL;
  struct db *paux = NULL;
  int posizione;
  paus = (struct db *) malloc (sizeof (struct db));
  strcpy (paus->nome, _nome);
  strcpy (paus->versione, _versione);
  strcpy (paus->collezione, _collezione);
  paus->depends = d;
  if (p == NULL)
    {
      p = paus;
      p->prossimo = NULL;
    }
  else
    {
      if (strcmp (p->nome, paus->nome) > 0)
	{
	  paus->prossimo = p;
	  p = paus;
	}
      else
	{
	  paux = p;
	  posizione = 0;
	  while (paux->prossimo != NULL && posizione != 1)
	    {
	      if (strcmp (paux->prossimo->nome, paus->nome) < 0)
		paux = paux->prossimo;
	      else
		posizione = 1;
	    }
	  paus->prossimo = paux->prossimo;
	  paux->prossimo = paus;
	}
    }
  return (p);
}


struct db *
cerca (char *parametro, struct db *p)
{
  struct db *paus = NULL;
  while (p != NULL)
    {
      if ((strcmp (p->nome, parametro) == 0)
	  || (strcmp (p->collezione, parametro) == 0)
	  || (strcmp (p->versione, parametro) == 0))
	{
	  paus =
	    inserisci_elemento_ordinato (p->nome, p->versione,
					 p->collezione, p->depends, paus);
	}
      p = p->prossimo;
    }
  return (paus);
}

int
conta (struct db *p)
{
  int i;
  struct db *paus;
  for (paus = p, i = 0; paus; paus = paus->prossimo, i++);
  return (i);
}

struct db *
rimuovi_duplicati (struct db *p)
{
  struct db *paus = NULL;
  struct db *paux = NULL;
  while (p != NULL)
    {
      paus =
	inserisci_elemento (p->nome, p->versione, p->collezione,
			    p->depends, paus);
      p = p->prossimo;
    }
  while (paus != NULL)
    {
      if (!cerca (paus->nome, paux))
	{
	  paux =
	    inserisci_elemento (paus->nome, paus->versione,
				paus->collezione, paus->depends, paux);
	}
      paus = paus->prossimo;
    }
  return (paux);
}

struct db *
db_like (char *delim, struct db *p)
{
  struct db *paus = NULL;
  while (p != NULL)
    {
      if (strstr (p->nome, delim))
	{
	  paus =
	    inserisci_elemento_ordinato (p->nome, p->versione,
					 p->collezione, p->depends, paus);
	}
      p = p->prossimo;
    }
  return (paus);
}
