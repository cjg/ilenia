/***************************************************************************
 *            manipola.c
 *
 *  Sat Jul 10 12:51:53 2004
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

#include <string.h>
#include "manipola.h"

/*
char * getValue(char *s, char del )
{
		static char local_s[255]="";
		strcpy(local_s, s);
    string::size_type pos = s.find( del );
    if ( pos != string::npos && pos+1 < s.length() ) {
        return s.substr( pos + 1 );
    }
    return "";
}
*/

char *
sed (char *s, char *trova, char *sostituisci)
{
	int i, j = 0;
	static char local_s[255] = "";
	for (i = 0; i <= 255; i++)
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
	static char local_s[255];
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
	static char saus[255] = "";
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
mid (char *s, int inizio, int lunghezza)
{
	static char local_s[255] = "";
	strcpy (local_s, s);
	int x, z = 0;
	if (lunghezza == FINE)
	{
		lunghezza = strlen (local_s) - inizio;
	}
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
mid_ (char *s, int inizio)
{
	int lunghezza = strlen (s) - inizio;
	return (mid (s, inizio, lunghezza));
}

char *
spazi (int n)
{
	static char s[255] = "";
	int x;
	for (x = 0; x < n; x++)
	{
		s[x] = ' ';
	}
	s[n] = '\0';
	return ((char *) s);
}
