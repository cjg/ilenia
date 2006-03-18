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
	char *repo = NULL;
	char *version = NULL;
	while (ilenia_pkgs) {
		repo =
		    pkglist_get_newer_favorite(ilenia_pkgs->name, options);

		if (repo == NULL) {
			ilenia_pkgs = ilenia_pkgs->next;
			continue;
		}

		version =
		    pkglist_get_from_repo(ilenia_pkgs->name, repo,
					  ilenia_ports);
		int test;

		if (type == DIFF)
			test = strcmp(ilenia_pkgs->version, version);
		else
			test = vercmp(ilenia_pkgs->version, version);

		if (test == 0) {
			ilenia_pkgs = ilenia_pkgs->next;
			continue;
		}

		p = pkglist_add_ordered(ilenia_pkgs->name, version, repo,
					NULL, p);
		if (print)
			prettyprint(ilenia_pkgs->name,
				    ilenia_pkgs->version, repo, version);

		ilenia_pkgs = ilenia_pkgs->next;
	}

	if (repo)
		free(repo);

	version = NULL;

	return (p);
}

char *pkglist_get_newer(char *name, struct pkglist *p)
{
	char *version = NULL;
	char *repo = NULL;

	while (p != NULL) {
		if (strcmp(name, p->name) != 0) {
			p = p->next;
			continue;
		}

		if (repo == NULL) {
			version = strdup(p->version);
			repo = strdup(p->repo);
			p = p->next;
			continue;
		}


		if (vercmp(version, p->version)) {
			version = strdup(p->version);
			repo = strdup(p->repo);
		}
		p = p->next;
	}

	if (version)
		free(version);

	if (repo)
		return (strdup(repo));

	return (NULL);
}

char *pkglist_get_newer_favorite(char *name, int option)
{
	char *repo = NULL;
	char *version = NULL;
	struct pkglist *p = NULL;

	if (option != NO_FAVORITE_REPO && option != NO_FAVORITES) {
		p = pkglist_find(name, ilenia_favoriterepo);
		if (p)
			repo = p->repo;
		p = NULL;
	}

	if (option != NO_FAVORITE_VERSION && option != NO_FAVORITES) {
		p = pkglist_find(name, ilenia_favoriteversion);
		if (p)
			version = p->version;
		p = NULL;
	}

	if (repo && version) {
		if (strcmp
		    (pkglist_get_from_repo(name, repo, ilenia_ports),
		     version))
			error("%s", "fix your favorite!");
	}

	if (repo == NULL && version == NULL)
		repo = pkglist_get_newer(name, pkglist_find(name,
							    ilenia_ports));

	if (version) {
		repo =
		    pkglist_get_from_version(name, version, ilenia_ports);
		if (repo == NULL)
			warning("%s with version %s not found!",
				name, version);
		free(version);
	}

	if (repo) {
		if (strcmp(repo, "!lock") == 0)
			return (NULL);

		if (pkglist_get_from_repo(name, repo, ilenia_ports) ==
		    NULL)
			warning("%s in repo %s not found!", name, repo);
		else
			return (strdup(repo));
	}

	return (NULL);
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
