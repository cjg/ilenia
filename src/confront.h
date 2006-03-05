/***************************************************************************
 *            confront.h
 *
 *  Sat Jul 10 13:27:54 2004
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

#define DIFF     3
#define UPDATED	 4

#define REGULAR		     1
#define NO_FAVORITE_REPO     2
#define NO_FAVORITE_VERSION  3
#define NO_FAVORITES	     5

struct pkglist *pkglist_confront(int type, int options, int print);
char *pkglist_get_newer(char *name, struct pkglist *p);
char *pkglist_get_newer_favorite(char *name, int option);
char *pkglist_get_from_version(char *nome, char *version,
			       struct pkglist *p);
char *pkglist_get_from_repo(char *name, char *repo, struct pkglist *p);
