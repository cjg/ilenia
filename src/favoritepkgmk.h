/***************************************************************************
 *            favoritepkgmk.h
 *
 *  Wed Nov 30 16:11:10 2005
 *  Copyright  2005 - 2006  Coviello Giuseppe
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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _FAVORITEPKGMK_H
#define _FAVORITEPKGMK_H
#include "list.h"

struct pkgmklist {
	char *pkgmk_conf;
	struct list *pkgs;
	struct pkgmklist *next;
};

struct pkgmklist *pkgmklist_add(char *pkgmk_conf, struct list *pkgs,
				struct pkgmklist *m);
char *pkgmklist_get_pkgmk_conf(char *pkg, struct pkgmklist *m);
struct pkgmklist *pkgmklist_build();

#endif				/* _FAVORITEPKGMK_H */
