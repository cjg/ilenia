/***************************************************************************
 *            vercmp.c
 *
 *  Sat Jul 10 13:22:49 2004
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
 */

#include <ctype.h>

/* GNU's strverscmp() function, taken from glibc 2.3.2 sources */

#define S_N	0x0
#define S_I	0x4
#define S_F	0x8
#define S_Z	0xC
#define CMP	2
#define LEN	3

int strverscmp (s1, s2)
     const char *s1;
     const char *s2;
{
  const unsigned char *p1 = (const unsigned char *) s1;
  const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;
  int state;
  int diff;
  static const unsigned int next_state[] = {
    S_N, S_I, S_Z, S_N,
    S_N, S_I, S_I, S_I,
    S_N, S_F, S_F, S_F,
    S_N, S_F, S_Z, S_Z
  };
  
  static const int result_type[] = {
    CMP, CMP, CMP, CMP, CMP, LEN, CMP, CMP,
    CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
    CMP, -1, -1, CMP, +1, LEN, LEN, CMP,
    +1, LEN, LEN, CMP, CMP, CMP, CMP, CMP,
    CMP, CMP, CMP, CMP, CMP, LEN, CMP, CMP,
    CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
    CMP, +1, +1, CMP, -1, CMP, CMP, CMP,
    -1, CMP, CMP, CMP
  };
  
  if (p1 == p2)
    return 0;
  c1 = *p1++;
  c2 = *p2++;
  state = S_N | ((c1 == '0') + (isdigit (c1) != 0));

  while ((diff = c1 - c2) == 0 && c1 != '\0') {
    state = next_state[state];
    c1 = *p1++;
    c2 = *p2++;
    state |= (c1 == '0') + (isdigit (c1) != 0);
  }

  state = result_type[state << 2 | (((c2 == '0') + (isdigit (c2) != 0)))];

  switch (state) {
  case CMP:
    return diff;
  case LEN:
    while (isdigit (*p1++))
      if (!isdigit (*p2++))
	return 1;
    return isdigit (*p2) ? -1 : diff;
  default:
    return state;
  }
}

int vercmp (char *installato, char *port) {
  int i = strverscmp (installato, port);
  if (i > 0) {
    // La versione installata è più aggiornata
    return (0);
  } else if (i < 0) {
    // Il pacchetto deve essere aggiornato
    return (1);
  }
  // Stessa versione
  return 0;
}
