/***************************************************************************
 *            aggiorna.c
 *
 *  Tue Feb 15 19:02:36 2004
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
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "manipola.h"
#include "ilenia.h"

char *
get_value (char *s, char *var)
{
  char *tmp = "";
  if (strncmp (s, var, strlen (var)) == 0)
    {
      char *splitted_s[2];
      split (s, "=", splitted_s);
      splitted_s[0] = trim (splitted_s[0]);
      if (strcmp (splitted_s[0], var) == 0)
	{
	  splitted_s[1] = trim (splitted_s[1]);
	  splitted_s[1] = mid (splitted_s[1], 1, END);
	  splitted_s[1] = trim (splitted_s[1]);
	  if (splitted_s[1][0] == '\"' || splitted_s[1][0] == '\"')
	    {
	      splitted_s[1] = mid (splitted_s[1], 1, strlen (splitted_s[1]) -
				   2);
	      splitted_s[1] = trim (splitted_s[1]);
	    }
	  tmp = strdup (splitted_s[1]);
	}
    }
  return (tmp);
}

int
parse_ileniarc ()
{
  FILE *rc;
  ask_for_update = 1;
  post_pkgadd = strdup ("");
  if ((rc = fopen ("/etc/ilenia.rc", "r")))
    {
      size_t n = 0;
      char *line = NULL;
      int nread = getline (&line, &n, rc);
      while (nread > 0)
	{
	  line = trim (line);
	  if (line[0] != '#')
	    {
	      if (strstr (line, "POST_PKGADD"))
		post_pkgadd = get_value (line, "POST_PKGADD");
	      if (strstr (line, "ASK_FOR_UPDATE"))
		{
		  if (strcasecmp (get_value (line, "ASK_FOR_UPDATE"), "No") ==
		      0)
		    ask_for_update = 0;
		}
	    }
	  nread = getline (&line, &n, rc);
	}
      line = NULL;
      free (line);
    }
  else
    printf ("Warning you don't have a ilenia.rc file.\n");
  return (0);
}

int
ask (char *question, ...)
{
  va_list args;

  va_start (args, question);
  vprintf (question, args);
  va_end (args);

  size_t n = 0;
  char *line = NULL;

  if (getline (&line, &n, stdin))
    {
      line = trim (line);
      if (!strcasecmp (line, "Y") || !strlen (line))
	return (1);
    }
  return (0);
}
