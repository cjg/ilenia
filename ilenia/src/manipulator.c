/***************************************************************************
 *            manipulator.c
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
#include "manipulator.h"

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
mid (char s[], int start, int length)
{
  int x, z = 0;
  if (length == -1)
    length = strlen (s) - start;
  if ((start + length) > strlen (s))
    return (s);

  for (x = 0; x <= strlen (s); x++)
    {
      if (x < start)
	continue;
      s[z] = s[x];
      z++;
    }

  s[length] = '\0';
  return (s);
}

char *
spaces (int n)
{
  char s[n];
  int x;
  for (x = 0; x < n; x++)
    s[x] = ' ';
  s[n] = '\0';
  return (strdup (s));
}

char *
sed (char s[], char *find, char *replace)
{
  int i, j = 0;
  char *s_copy;
  s_copy = strdup (s);
  /*
     static char local_s[MASSIMO] = "";
     for (i = 0; i <= MASSIMO; i++)
     local_s[i] = '\0';
   */
  for (i = 0; i <= (strlen (s_copy) - strlen (find)); i++)
    {
      if (strcmp (mid (s_copy, i, strlen (find)), find) == 0)
	{
	  strcat (s, replace);
	  j += strlen (replace);
	  i += strlen (replace) - 1;
	}
      else
	{
	  s[i] = s_copy[i];
	  j++;
	}
    }
  s[j] = '\0';
  if (s_copy)
    free (s_copy);
  return s;
}

char *
sedchr (char s[], int find, int replace)
{
  int i;
  char *s_copy;
  s_copy = strdup (s);
  for (i = 0; i < strlen (s_copy); i++)
    {
      if (s_copy[i] == find)
	s[i] = replace;
      else
	s[i] = s_copy[i];
    }
  s[i] = '\0';
  if (s_copy)
    free (s_copy);
  return s;
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
split (char *s, char *delim, char *splitted[])
{
  int i = 0;
  while (strlen (s) > 0)
    {
      char *tmp;
      tmp = strdup (s);
      strtok (tmp, delim);
      splitted[i] = strdup (tmp);
      s = mid (s, strlen (tmp), END);
      i++;
      if (tmp)
	free (tmp);
    }
  return (i);
}
