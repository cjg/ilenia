/***************************************************************************
 *            db.c
 *
 *  Sat Jul 10 12:04:21 2004
 *  Copyright  2004  Coviello Giuseppe
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

struct db * inserisci_elemento_ordinato (char *_nome, char *_versione, char *_collezione,
					 struct db *p) {
  struct db *paus = NULL;
  struct db *paux = NULL;
  int posizione;
  paus = (struct db *) malloc (sizeof (struct db));
  strcpy (paus->nome, _nome);
  strcpy (paus->versione, _versione);
  strcpy (paus->collezione, _collezione);
  if (p == NULL) {
    p = paus;
    p->prossimo = NULL;
  } else {
    if (strcmp (p->nome, paus->nome) > 0) {
      paus->prossimo = p;
      p = paus;
    } else {
      paux = p;
      posizione = 0;
      while (paux->prossimo != NULL && posizione != 1) {
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

struct db * cerca (char *parametro, struct db *p) {
  struct db *paus = NULL;
  while (p != NULL) {
    if ((strcmp (p->nome, parametro) == 0) || (strcmp (p->collezione, parametro) == 0) ||
	(strcmp (p->versione, parametro) == 0)) {
      paus = inserisci_elemento_ordinato (p->nome, p->versione, p->collezione, paus);}
    p = p->prossimo;
  }
  return(paus);
}
