/***************************************************************************
 *            aggiorna.c
 *
 *  Tue Feb 15 19:02:36 2004
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
 *
 */

#include <stdio.h>
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
      char splitted_s[2][MASSIMO];
      split (s, "=", splitted_s);
      strcpy (splitted_s[0], trim (splitted_s[0]));
      if (strcmp (splitted_s[0], var) == 0)
	{
	  strcpy (splitted_s[1], trim (splitted_s[1]));
	  strcpy (splitted_s[1], mid (splitted_s[1], 1, FINE));
	  strcpy (splitted_s[1], trim (splitted_s[1]));
	  if (splitted_s[1][0] == '\"' || splitted_s[1][0] == '\"')
	    {
	      strcpy (splitted_s[1],
		      mid (splitted_s[1], 1, strlen (splitted_s[1]) - 2));
	      strcpy (splitted_s[1], trim (splitted_s[1]));
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
      char row[MASSIMO];
      while (fgets (row, MASSIMO, rc))
	{
	  strcpy (row, trim (row));
	  if (row[0] != '#')
	    {
	      if (strncmp (row, "POST_PKGADD", 11) == 0)
		{
		  char splitted_row[2][MASSIMO];
		  split (row, "=", splitted_row);
		  strcpy (row, mid (splitted_row[1], 1, FINE));
		  strcpy (row, trim (row));
		  strcpy (row, mid (row, 1, strlen (row) - 2));
		  post_pkgadd = strdup (row);
		}
	      if (strstr (row, "ASK_FOR_UPDATE"))
		{
		  if (strcasecmp (get_value (row, "ASK_FOR_UPDATE"), "No") ==
		      0)
		    ask_for_update = 0;
		}
	    }
	}
    }
  else
    printf ("Warning you don't have a ilenia.rc file.\n");
  return (0);
}

int
ask (char *question, ...)
{
  char response[20];
  va_list args;

  va_start (args, question);
  vprintf (question, args);
  va_end (args);

  if (fgets (response, 20, stdin))
    {
      strcpy (response, trim (response));
      if (!strcasecmp (response, "Y") || !strlen (response))
	return (1);
    }
  return (0);
}
