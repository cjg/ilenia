/***************************************************************************
 *            favoritepkgmk.c
 *
 *  Wed Nov 30 16:17:18 2005
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manipulator.h"
#include "favoritepkgmk.h"
#include "output.h"

struct pkgmklist *pkgmklist_add(char *pkgmk_conf, struct list *pkgs,
				struct pkgmklist *m)
{
	struct pkgmklist *maus1 = NULL;
	maus1 = (struct pkgmklist *) malloc(sizeof(struct pkgmklist));
	maus1->pkgmk_conf = (char *) malloc(strlen(pkgmk_conf) + 1);
	strcpy(maus1->pkgmk_conf, pkgmk_conf);
	maus1->pkgs = pkgs;
	if (m == NULL) {
		m = maus1;
		m->next = NULL;
	} else {
		struct pkgmklist *maus2 = NULL;
		maus2 = m;
		while (maus2->next != NULL)
			maus2 = maus2->next;
		maus1->next = NULL;
		maus2->next = maus1;
	}
	return (m);
}

char *pkgmklist_get_pkgmk_conf(char *pkg, struct pkgmklist *m)
{
	while (m != NULL) {
		if (list_find(pkg, m->pkgs))
			return strdup(m->pkgmk_conf);
		m = m->next;
	}
	return (NULL);
}

struct pkgmklist *pkgmklist_build()
{
	FILE *file;
	struct pkgmklist *m = NULL;
	file = fopen("/etc/ports/favoritepkgmk", "r");
	if (file == NULL) {
		warning("/etc/ports/favoritepkgmk not found!");
		return (NULL);
	}

	size_t n = 0;
	char *line = NULL;
	int nread;

	while ((nread = getline(&line, &n, file)) > 0) {
		trim(line);
		if ((strcmp(line, "") == 0) || (line[0] == '#'))
			continue;

		sedchr(line, '\t', ' ');

		while (strstr(line, "  "))
			sed(line, "  ", " ");

		struct list *l = NULL;

		l = split(line, " ");
		if (l)
			m = pkgmklist_add(l->data, l->next, m);
	}
	fclose(file);
	return (m);
}
