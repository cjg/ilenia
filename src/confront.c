/***************************************************************************
 *            confront.c
 *
 *  Sat Jul 10 13:33:11 2004
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pkglist.h"
#include "manipulator.h"
#include "vercmp.h"
#include "confront.h"
#include "ilenia.h"
#include "output.h"

void prettyprint(char *str1, char *str2, char *str3, char *str4)
{
	printf("%-26s %18s %-14s %18s\n", str1, str2, str3, str4);
}

struct pkglist *pkglist_confront(int type, int options, int print)
{
	struct pkglist *p = NULL;
	
	if (print)
		prettyprint("Name", "Installed Version", "Repository",
			    "Port Version");

	struct pkglist *ilenia_pkgs_ptr = ilenia_pkgs;
	struct pkglist *tmp = NULL;
	while (ilenia_pkgs_ptr) {
		tmp = pkglist_find(ilenia_pkgs_ptr->name, ilenia_ports);

		if(!tmp) {
			ilenia_pkgs_ptr = ilenia_pkgs_ptr->next;
			continue;
		}

		int test;

		if (type == DIFF)
			test = strcmp(ilenia_pkgs_ptr->version, tmp->version);
		else
			test = vercmp(ilenia_pkgs_ptr->version, tmp->version);

		if (test == 0) {
			ilenia_pkgs_ptr = ilenia_pkgs_ptr->next;
			continue;
		}

		p = pkglist_add_ordered(ilenia_pkgs_ptr->name, tmp->version, tmp->repo,
					NULL, p);

		if (print)
			prettyprint(ilenia_pkgs_ptr->name,
				    ilenia_pkgs_ptr->version, tmp->repo, tmp->version);

		ilenia_pkgs_ptr = ilenia_pkgs_ptr->next;
	}

	return (p);
}

char *pkglist_get_from_version(char *name, char *version,
			       struct pkglist *p)
{
	while (p != NULL) {
		if (strcmp(name, p->name) == 0) {
			if (strcmp(version, p->version) == 0)
				return (p->repo);
		}
		p = p->next;
	}
	return (NULL);
}

char *pkglist_get_from_repo(char *name, char *repo, struct pkglist *p)
{
	if (!repo)
		return NULL;

	while (p != NULL) {
		if (strcmp(name, p->name) == 0) {
			if (strcmp(repo, p->repo) == 0)
				return (p->version);
		}
		p = p->next;
	}
	return (NULL);
}
