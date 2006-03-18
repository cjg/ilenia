/***************************************************************************
 *            pkglist.h
 *
 *  Mon Oct 03 12:19:28 2005
 *  Copyright  2004 - 2006  Coviello Giuseppe
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

#ifndef _PKGLIST_H
#define _PKGLIST_H

struct pkglist {
	char name[255];
	char version[255];
	char repo[255];
	struct deplist *depends;
	struct pkglist *next;
};

struct pkglist *pkglist_add(char *name, char *version, char *repo,
			    struct deplist *d, struct pkglist *p);
struct pkglist *pkglist_add_ordered(char *name, char *version, char *repo,
				    struct deplist *d, struct pkglist *p);
struct pkglist *pkglist_add_reversed(char *name, char *version, char *repo,
				     struct deplist *d, struct pkglist *p);
struct pkglist *pkglist_add_deplist(struct deplist *d, struct pkglist *p);
struct pkglist *pkglist_find(char *param, struct pkglist *p);
struct pkglist *pkglist_find_like(char *param, struct pkglist *p);
struct pkglist *pkglist_select_from_repo(char *repo, struct pkglist *p);
int pkglist_len(struct pkglist *p);
int pkglist_exists(char *param, struct pkglist *p);
struct pkglist *pkglist_cat(struct pkglist *dest, struct pkglist *src,
			    int duplicates);
struct pkglist *pkglist_remove(char *name, char *repo, struct pkglist *p);

#endif				/* _PKGLIST_H */
