/***************************************************************************
 *            lspacchetti.c
 *
 *  Sat Jul 10 12:14:11 2004
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
#include "lspacchetti.h"
#include "manipola.h"
#include "pkglist.h"
#include "aliaslist.h"
#include "ilenia.h"

struct pkglist *
prendi_favorite (int quale)
{
  FILE *favorite;
  char riga[255] = "";
  char nomefile[255] = "";
  char nome[255] = "";
  char favorite_opzioni[255] = "";
  struct pkglist *p = NULL;
  if (quale == REPO)
    {
      strcpy (nomefile, "/etc/ports/favoriterepo");
    }
  else
    {
      strcpy (nomefile, "/etc/ports/favoriteversion");
    }
  if ((favorite = fopen (nomefile, "r")))
    {
      while (fgets (riga, 255, favorite))
	{
	  strcpy (riga, trim (riga));
	  if ((strcmp (riga, "") != 0) && (riga[0] != '#'))
	    {
	      strcpy (riga, tab2spazi (riga));
	      strcpy (favorite_opzioni, index (riga, ' '));
	      strcpy (nome,
		      mid (riga, 0,
			   strlen (riga) - strlen (favorite_opzioni)));
	      strcpy (favorite_opzioni, trim (favorite_opzioni));
	      p = pkglist_add_ordered (nome, favorite_opzioni,
				       favorite_opzioni, NULL, p);
	    }
	}
    }
  return (p);
}

struct pkglist *
lspacchetti ()
{
  FILE *file_io = fopen (DB_FILE, "r");
  char riga[255];
  char nome[255] = "";
  int nuovo_record = 1;
  struct pkglist *p = NULL;
  struct pkglist *paus = NULL;
  struct pkglist *favoriterepo = NULL;
  struct pkglist *favoriteversion = NULL;
  favoriterepo = prendi_favorite (REPO);
  favoriteversion = prendi_favorite (VERSIONE);
  while (fgets (riga, 255, file_io))
    {
      int tmp = strlen (riga);
      riga[tmp - 1] = riga[tmp];
      if (nuovo_record && !strcmp (nome, ""))
	{
	  strcpy (nome, riga);
	}
      else if (nuovo_record)
	{
	  char tmp[255] = "";
	  if ((paus = pkglist_find (nome, favoriterepo)) != NULL)
	    {
	      strcpy (tmp, "R ");
	      strcat (tmp, paus->repo);
	    }
	  else if ((paus = pkglist_find (nome, favoriteversion)) != NULL)
	    {
	      strcpy (tmp, "V ");
	      strcat (tmp, paus->version);
	    }
	  else
	    {
	      strcpy (tmp, "installato");
	    }
	  p = pkglist_add_ordered (nome, riga, tmp, NULL, p);
	  struct aliaslist *a = NULL;
	  a = aliaslist_get (nome, ilenia_aliases);
	  while (a != NULL)
	    {
	      if (strcmp (a->name, nome) != 0)
		p = pkglist_add_ordered (a->name, "alias", nome, NULL, p);
	      a = a->next;
	    }
	  strcpy (nome, "");
	  nuovo_record = 0;
	}
      if (riga[1] == '\0')
	nuovo_record = 1;
    }
  fclose (file_io);
  return (p);
}
