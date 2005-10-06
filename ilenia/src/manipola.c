/***************************************************************************
 *            manipola.c
 *
 *  Sat Jul 10 12:51:53 2004
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

#include <string.h>
#include <stdlib.h>
#include "manipola.h"

char *
sed (char *s, char *trova, char *sostituisci)
{
  int i, j = 0;
  static char local_s[MASSIMO] = "";
  for (i = 0; i <= MASSIMO; i++)
    local_s[i] = '\0';
  for (i = 0; i <= (strlen (s) - strlen (trova)); i++)
    {
      if (strcmp (mid (s, i, strlen (trova)), trova) == 0)
	{
	  strcat (local_s, sostituisci);
	  j += strlen (sostituisci);
	  i += strlen (sostituisci) - 1;
	}
      else
	{
	  local_s[i] = s[i];
	  j++;
	}
    }
  local_s[j] = '\0';
  return ((char *) local_s);
}

char *
sedchr (char *s, int trova, int sostituisci)
{
  int i;
  static char local_s[MASSIMO];
  for (i = 0; i < strlen (s); i++)
    {
      if (s[i] == trova)
	local_s[i] = sostituisci;
      else
	local_s[i] = s[i];
    }
  local_s[i] = '\0';
  return ((char *) local_s);
}

char *
trim (char *s)
{
  int n;
  while (*s && *s <= 32)
    ++s;
  for (n = strlen (s) - 1; n >= 0 && s[n] <= 32; --n)
    s[n] = 0;
  return s;
}

char *
tab2spazi (char *s)
{
  static char saus[MASSIMO] = "";
  int i;
  strcpy (saus, s);
  for (i = 0; i <= strlen (saus); i++)
    {
      if (saus[i] == '\t')
	saus[i] = ' ';
    }
  return ((char *) saus);
}

char *
oldmid (char *s, int inizio, int lunghezza)
{
  static char local_s[MASSIMO] = "";
  strcpy (local_s, s);
  int x, z = 0;
  if (lunghezza == FINE)
    lunghezza = strlen (local_s) - inizio;
  if ((inizio + lunghezza) <= strlen (local_s))
    {
      for (x = 0; x <= strlen (local_s); x++)
	{
	  if ((x >= inizio))
	    {
	      local_s[z] = local_s[x];
	      z++;
	    }
	}
    }
  local_s[lunghezza] = '\0';
  return ((char *) local_s);
}

char *
mid (char *s, int start, int length)
{
  int x, z = 0;
  if (length == -1)
    length = strlen (s) - start;
  if ((start + length) <= strlen (s))
    {
      for (x = 0; x <= strlen (s); x++)
	{
	  if ((x >= start))
	    {
	      s[z] = s[x];
	      z++;
	    }
	}
    }
  s[length] = '\0';
  return (s);
}

char *
mid_ (char *s, int inizio)
{
  int lunghezza = strlen (s) - inizio;
  return (mid (s, inizio, lunghezza));
}

char *
spazi (int n)
{
  static char s[MASSIMO] = "";
  int x;
  for (x = 0; x < n; x++)
    {
      s[x] = ' ';
    }
  s[n] = '\0';
  return ((char *) s);
}

int
count (char *s, int delim)
{
  int i, c = 0;
  for (i = 0; i <= strlen (s); i++)
    {
      if (s[i] == delim)
	c++;
    }
  return (c);
}

int
split2 (char *s, char *delim, char *splitted[])
{
  int i = 0;
  while (strlen (s) > 0)
    {
      char *tmp;
      tmp = strdup (s);
      strtok (tmp, delim);
      splitted[i] = strdup (tmp);
      s = mid (s, strlen (tmp), FINE);
      i++;
      tmp = NULL;
      free (tmp);
    }
  return (i);
}

int
split (char *s, char *delim, char splitted[][MASSIMO])
{
  char local_s[MASSIMO];
  char tmp[MASSIMO];
  strcpy (local_s, s);
  int i = 0;
  while (strlen (local_s) > 0)
    {
      strcpy (tmp, local_s);
      strtok (tmp, delim);
      strcpy (splitted[i], tmp);
      strcpy (local_s, mid (local_s, strlen (tmp), FINE));
      i++;
    }
  return (i);
}
