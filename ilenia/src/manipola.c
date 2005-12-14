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
#include <stdlib.h>
#include <string.h>
#include "manipola.h"

char *
sed (char *s, char *trova, char *sostituisci)
{
  int i, j = 0;
  static char local_s[MASSIMO] = "";
  char tmp[strlen(s)];
  for (i = 0; i <= MASSIMO; i++)
    local_s[i] = '\0';
  for (i = 0; i <= (strlen (s) - strlen (trova)); i++)
    {
	    strcpy(tmp, s);
      if (strcmp (mid (tmp, i, strlen (trova)), trova) == 0)
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

char *sedng(char *s, char *find, char *replace)
{
	char *tmp = strdup(s);
	s = (char *) malloc(strlen(tmp));
	int i = 0;
	while (*tmp) {
		if (strncmp(tmp, find, strlen(find)) != 0) {
			s[i++] = *tmp;
			tmp++;
			continue;
		}
		if (strlen(replace) > 1)
			s = (char *) realloc(s,
					     strlen(s) + strlen(tmp) +
					     strlen(replace));
		int z;
		for (z = 0; z < strlen(replace); z++)
			s[i++] = replace[z];

		tmp += strlen(find);

		s[i++] = *tmp;
		tmp++;
	}
	tmp = NULL;
	free(tmp);
	s[i] = '\0';
	return (s);
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

char *mid(char s[], int start, int length)
{
	if (length == -1)
		length = strlen(s) - start;

	strncpy(s, s + start, length);
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
