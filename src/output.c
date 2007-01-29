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
#include "confront.h"
#include "manipulator.h"
#include "utils.h"

void error(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(1);
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
	char *path, *pkgfile_path;
	struct repolist *repo;
	pkglist *p = NULL;

	p = pkglist_find(name, ilenia_ports);

	if(!p) {
		warning("%s not found!", name);
		return;
	}

	repo = repolist_find(p->repo, ilenia_repos);
	
	path = strdup_printf("%s/%s/%s", repo->path, repo->name, name);
	pkgfile_path = strdup_printf("%s/%s/%s/Pkgfile", repo->path, repo->name, name);

	FILE *f;
	f = fopen(pkgfile_path, "r");
	if(!f)
		exit(1);

	printf("Name: %s\nVersion: %s\n", p->name, p->version);
	char *line = NULL;
	size_t n;
	int nread;

	while((nread = getline(&line, &n, f)) > 0) {
		char *_line = strdup(line);
		trim(_line);
		if(*_line != '#')
			continue;
		_line++;
		trim(_line);
		while(strstr(_line, "  "))
			strreplaceall(_line, "  ", " ");
		printf("%s\n", _line);
		free(_line);
	}
	
	free(line);
	fclose(f);

	printf("Repository: %s\n", repo->name);
	printf("Path: %s\n", path);
	printf("Installed: %s\n", (pkglist_exists(p->name, ilenia_pkgs) ==
				   EXIT_SUCCESS) ? "yes" : "no");
	printf("README: %s\n", (is_file(path, "README") == EXIT_SUCCESS) ? "yes"
	       : "no");
	printf("pre-install: %s\n", (is_file(path, "pre-install") ==
				     EXIT_SUCCESS) ? "yes" : "no");
	printf("post-install: %s\n", (is_file(path, "post-install") ==
				      EXIT_SUCCESS) ? "yes" : "no");
	free(pkgfile_path);
	free(path);
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
