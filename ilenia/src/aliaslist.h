/***************************************************************************
 *            aliaslist.h
 *
 *  Tue Avr 12 16:01:32 2005
 *  Copyright  2005 - 2006  Coviello Giuseppe
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

struct aliaslist
{
  char pkg[255];
  struct aliaslist *next;
};

struct aliaseslist
{
  struct aliaslist *alias;
  struct aliaseslist *next;
};

struct aliaslist *aliaslist_add (char *pkg, struct aliaslist *p);
struct aliaslist *aliaslist_get (char *delim, struct aliaseslist *as);
struct aliaseslist *aliaseslist_add (struct aliaslist *a,
				     struct aliaseslist *p);
struct aliaseslist *aliaseslist_build ();
int aliaslist_exists (char *delim, struct aliaslist *p);
