/***************************************************************************
 *            manipola.h
 *
 *  Sat Jul 10 12:49:03 2004
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

#define FINE -1
#define MASSIMO 2555

char *trim (char *s);
char *tab2spazi (char *s);
char *mid (char *stringa, int inizio, int lunghezza);
char *mid_ (char *stringa, int inizio);
char *spazi (int n);
char *sedchr (char *s, int trova, int sostituisci);
char *sed (char *s, char *trova, char *sostituisci);
