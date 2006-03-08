/***************************************************************************
 *            dependencies.c
 *
 *  Sat Sep 11 18:04:21 2004
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
#include <stdlib.h>
#include <string.h>
#include "pkglist.h"
#include "deplist.h"
#include "manipulator.h"
#include "confront.h"
#include "output.h"
#include "ilenia.h"

struct pkglist *get_package_dependencies(char *name, char *repo)
{
	struct pkglist *pkg = NULL;
	struct pkglist *dependencies = NULL;
	pkg = pkglist_find(name, ilenia_ports);
	pkg = pkglist_select_from_repo(repo, pkg);
	if (!pkg)
		return NULL;
	while (pkg->depends != NULL) {
		char *repo = NULL, *version;
		repo =
		    pkglist_get_newer_favorite(pkg->depends->name,
					       REGULAR);
		if (repo != NULL) {
			version = pkglist_get_from_repo(pkg->depends->name,
							repo,
							ilenia_ports);
			dependencies =
			    pkglist_add_reversed(pkg->depends->name,
						 version, repo, NULL,
						 dependencies);
		} else
			dependencies =
			    pkglist_add_reversed(pkg->depends->name, "",
						 "not found", NULL,
						 dependencies);
		pkg->depends = pkg->depends->next;
	}
	return (dependencies);
}

struct pkglist *pkglist_remove_duplicates(struct pkglist *p)
{
	struct pkglist *paus = NULL;
	while (p != NULL) {
		if (pkglist_exists(p->name, paus) != 0)
			paus = pkglist_add_reversed(p->name, "", p->repo,
						    NULL, paus);
		p = p->next;
	}
	return (paus);
}

struct pkglist *find_dependencies(struct pkglist *p)
{
	struct pkglist *dependencies = NULL;
	while (p != NULL) {
		if (strcmp(p->repo, "not found") == 0) {
			p = p->next;
			continue;
		}
		struct pkglist *d = NULL;
		d = get_package_dependencies(p->name, p->repo);
		d = pkglist_add_reversed(p->name, p->version, p->repo,
					 NULL, d);
		dependencies = pkglist_cat(dependencies, d, 0);
		p = p->next;
	}
	return (dependencies);
}

struct pkglist *find_dependents(struct pkglist *p)
{
	struct pkglist *dependents = NULL;
	dependents = p;
	while (p != NULL) {
		struct pkglist *pkgs = NULL;
		pkgs = ilenia_pkgs;
		while (pkgs != NULL) {
			char *repo;
			repo =
			    pkglist_get_newer_favorite(pkgs->name,
						       REGULAR);

			if (repo == NULL) {
				pkgs = pkgs->next;
				continue;
			}

			struct pkglist *tmp = NULL;
			if ((tmp =
			     pkglist_select_from_repo(repo,
						      pkglist_find(pkgs->
								   name,
								   ilenia_ports))))
			{
				if (deplist_exists(p->name, tmp->depends)
				    == EXIT_SUCCESS) {
					if (pkglist_find
					    (pkgs->name,
					     dependents) == NULL)
						dependents =
						    pkglist_add(pkgs->name,
								"", repo,
								NULL,
								dependents);
				}
			}
			pkgs = pkgs->next;
		}
		p = p->next;
	}
	return (dependents);
}

struct pkglist *get_dependencies(char *name)
{
	struct pkglist *p = NULL;
	char *repo, *version;
	int i = -10;

	if (pkglist_exists(name, ilenia_ports) == 0) {
		repo = pkglist_get_newer_favorite(name, REGULAR);
		version = pkglist_get_from_repo(name, repo, ilenia_ports);
		p = pkglist_add_reversed(name, version, repo, NULL, p);
		while (i != pkglist_len(p)) {
			i = pkglist_len(p);
			p = find_dependencies(p);
		}
	} else {
		p = pkglist_add(name, "", "not found", NULL, p);
	}
	return (p);
}

struct pkglist *get_dependents(char *name, int all)
{
	struct pkglist *p = NULL;
	char *repo;
	int i = -10;

	repo = pkglist_get_newer_favorite(name, REGULAR);
	p = pkglist_add(name, "", repo, NULL, p);

	while (i != pkglist_len(p)) {
		i = pkglist_len(p);
		p = find_dependents(p);
		if (all == 0)
			return (p);
	}
	return (p);
}

void print_dependencies(char *name)
{
	struct pkglist *p = NULL;
	p = get_dependencies(name);
	while (p != NULL) {
		if (strcmp(p->repo, "not found") == 0) {
			printf("%s [not found]\n", p->name);
			p = p->next;
			continue;
		}

		printf("%s [", p->name);

		if (pkglist_exists(p->name, ilenia_pkgs) != 0) {
			printf(" ]\n");
			p = p->next;
			continue;
		}
		printf("installed]");

		struct pkglist *paus = NULL;
		paus = pkglist_find(p->name, ilenia_pkgs);

		if (strcmp(paus->version, "alias") == 0)
			printf(" (%s)\n", paus->repo);
		else
			printf("\n");
		p = p->next;
	}
}

void print_dependents(char *name, int all)
{
	struct pkglist *p = NULL;
	p = get_dependents(name, all);
	while (p != NULL) {
		if (strcmp(p->repo, "not found") != 0) {
			printf("%s\n", p->name);
		}
		p = p->next;
	}
}
