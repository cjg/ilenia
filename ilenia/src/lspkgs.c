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
  struct pkglist *p = NULL;
  char *line = NULL;
  char *name = NULL;
  size_t n = 0;
  ssize_t nread;
  int newrecord = 1;

  while (((nread = getline (&line, &n, file)) != -1))
    {
      line[strlen (line) - 1] = '\0';

      if (line[0] == '\0')
	{
	  newrecord = 1;
	  continue;
	}

      if (!newrecord)
	continue;

      if (name == NULL)
	name = strdup (line);
      else
	{
	  p = pkglist_add_ordered (name, line, NULL, NULL, p);
	  name = NULL;
	  newrecord = 0;
	}
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
  file = fopen (filename, "r");
  if (file == NULL)
    return (NULL);

  size_t n = 0;
  char *line = NULL;
  int nread;
  char *favoriterow[2];

  while ((nread = getline (&line, &n, file)) > 0)
    {
      trim (line);
      if ((strcmp (line, "") == 0) || (line[0] == '#'))
	continue;

      sedchr (line, '\t', ' ');

      while (strstr (line, "  "))
	sed (line, "  ", " ");

      split (line, " ", favoriterow);
      trim (favoriterow[1]);
      p = pkglist_add_ordered (favoriterow[0],
			       favoriterow[1], favoriterow[1], NULL, p);
    }

  if (line)
    free (line);

  if (filename)
    free (filename);

  return (p);
}
