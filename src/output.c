/***************************************************************************
 *            output.c
 *
 *  Fri Jul 16 18:45:34 2004
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
#include <stdarg.h>
#include <stdlib.h>
#include "pkglist.h"
#include "ilenia.h"
#include "repolist.h"
#include "dbutils.h"
#include "../config.h"
#include "confront.h"

void error(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	safe_exit(1);
}

void warning(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "Warning: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void info(char *name, int options)
{
	/*
	struct repolist *repo;
<<<<<<< .working
	reponame = pkglist_get_newer_favorite(name, options);
	
	struct pkglist *p=NULL;
	p=db_get(name);
	if (p->repo == NULL) {
		printf("Error: %s not found!\n", name);
		return;
	}
=======
	reponame = pkglist_get_newer_favorite(name, options);
	if (reponame == NULL)
		error("%s not found!", name);
>>>>>>> .merge-right.r230
	repo = repolist_find(p->repo, ilenia_repos);
	printf("%s%s/%s\n", repo->path, repo->name, name);
	*/
	printf("klj");
}


void pkglist_print(struct pkglist *p)
{
	//       33                                20                   20
	printf
	    ("Package                           Version              Repository\n");
	while (p != NULL) {
		printf("%-33s %-20s %-20s\n", p->name, p->version,
		       p->repo);
		/*
		   printf("%s%s %s%s %s%s\n", mid(p->name, 0, 33),
		   spaces(33 - strlen(mid(p->name, 0, 33))),
		   mid(p->version, 0, 20),
		   spaces(20 - strlen(mid(p->version, 0, 20))),
		   mid(p->repo, 0, 20),
		   spaces(20 - strlen(mid(p->repo, 0, 20))));
		 */
		p = p->next;
	}
	printf("\n");
}
