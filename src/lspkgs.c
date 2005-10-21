/***************************************************************************
 *            lspkgs.c
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
#include <stdlib.h>
#include "lspkgs.h"
#include "manipulator.h"
#include "pkglist.h"
#include "aliaslist.h"
#include "ilenia.h"

struct pkglist *
lspkgs ()
{
  FILE *file = fopen (DB_FILE, "r");

  int new_record = 1;

  struct pkglist *p = NULL;
  struct pkglist *paus = NULL;
  struct pkglist *favoriterepo = NULL;
  struct pkglist *favoriteversion = NULL;

  favoriterepo = get_favorite (REPO);
  favoriteversion = get_favorite (VERSION);

  size_t n = 0;
  char *line = NULL;
  ssize_t nread;
  char *name = NULL;

  while (((nread = getline (&line, &n, file)) != -1))
    {
      line[strlen (line) - 1] = '\0';

      if (new_record && !name)
	name = strdup (line);
      else if (new_record)
	{
	  char *tmp;
	  if ((paus = pkglist_find (name, favoriterepo)) != NULL)
	    {
	      tmp = strdup ("R ");
	      strcat (tmp, paus->repo);
	    }
	  else if ((paus = pkglist_find (name, favoriteversion)) != NULL)
	    {
	      tmp = strdup ("V ");
	      strcat (tmp, paus->version);
	    }
	  else
	    {
	      tmp = strdup ("installed");
	    }
	  p = pkglist_add_ordered (name, line, tmp, NULL, p);
	  struct aliaslist *a = NULL;
	  a = aliaslist_get (name, ilenia_aliases);
	  while (a != NULL)
	    {
	      if (strcmp (a->name, name) != 0)
		p = pkglist_add_ordered (a->name, "alias", name, NULL, p);
	      a = a->next;
	    }

	  name = NULL;
	  new_record = 0;
	}

      if (line[1] == '\0')
	new_record = 1;

    }
  if (line)
    free (line);
  fclose (file);
  return (p);
}


struct pkglist *
get_favorite (int favorite)
{
  FILE *file;
  char *filename = "";
  struct pkglist *p = NULL;

  if (favorite == REPO)
    filename = strdup ("/etc/ports/favoriterepo");
  else
    filename = strdup ("/etc/ports/favoriteversion");

  if ((file = fopen (filename, "r")))
    {
      size_t n = 0;
      char *line = NULL;
      int nread = getline (&line, &n, file);

      while (nread > 0)
	{
	  line = trim (line);
	  if ((strcmp (line, "") != 0) && (line[0] != '#'))
	    {
	      sedchr (line, '\t', ' ');
	      char *favorite_value = index (line, ' ');
	      char *name = mid (line, 0,
				strlen (line) - strlen (favorite_value));
	      favorite_value = trim (favorite_value);
	      p = pkglist_add_ordered (name, favorite_value,
				       favorite_value, NULL, p);
	    }
	  nread = getline (&line, &n, file);
	}
      if (line)
	free (line);
    }
  return (p);
}
