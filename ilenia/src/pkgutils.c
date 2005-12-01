/***************************************************************************
 *            pkgutils.c
 *
 *  Wed Sep  1 18:55:42 2004
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

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "pkglist.h"
#include "manipulator.h"
#include "confront.h"
#include "dependencies.h"
#include "ilenia.h"
#include "lspkgs.h"
#include "repolist.h"
#include "output.h"
#include "utils.h"
#include "favoritepkgmk.h"

int do_post_pkgadd(char *path)
{
	if (post_pkgadd == NULL)
		return (EXIT_SUCCESS);

	FILE *file;
	file = fopen("/tmp/post_pkgadd.sh", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	fprintf(file, "#!/bin/sh\n\n%s\n\n# End of file", post_pkgadd);
	fclose(file);

	char *args[2];
	args[0] = NULL;
	args[1] = strdup("/tmp/post_pkgadd.sh");

	return (exec(path, "/bin/sh", args));
}

void installscript(char path[], char *script)
{
	if (is_file(path, script) == EXIT_SUCCESS)
		exec(path, script, NULL);
}

int build_install_pkg(int option, char *name)
{
	struct repolist *r;
	char *pkgmk_conf = NULL;

	pkgmk_conf = pkgmklist_get_pkgmk_conf(name, ilenia_favoritepkgmk);
	if (pkgmk_conf == NULL)
		pkgmk_conf = strdup("/etc/pkgmk.conf");

	char *path = NULL;
	char *install_action;
	char *repo = pkglist_get_newer_favorite(name, option);

	if (repo == NULL) {
		printf("Error: %s not found or locked!\n", name);
		return (EXIT_FAILURE);
	}

	r = repolist_find(repo, ilenia_repos);

	if (pkglist_find(name, ilenia_pkgs))
		install_action = strdup("-u");
	else
		install_action = strdup("-i");

	path = strdup(r->path);
	if (path[strlen(path) - 1] != '/')
		strcat(path, "/");

	if (strstr(r->name, "local") == NULL) {
		strcat(path, r->name);
		strcat(path, "/");
	}

	strcat(path, name);
	installscript(path, "pre-install");

	char *args[] =
	    { "", "-d", "-cf", pkgmk_conf, install_action, NULL };

	if (exec(path, "/usr/bin/pkgmk", args) != EXIT_SUCCESS)
		return (EXIT_FAILURE);

	installscript(path, "post-install");

	do_post_pkgadd(path);

	return (EXIT_SUCCESS);
}

void not_found_helper()
{
	if (not_found_policy == STOP_POLICY)
		exit(EXIT_FAILURE);
	if (not_found_policy == NEVERMIND_POLICY)
		return;
	if (ask("Can I ignore above package and continue? [Y/n] ") ==
	    EXIT_FAILURE)
		exit(EXIT_FAILURE);
}

int update_pkg(int option, char *name)
{
	if (getuid() != 0) {
		printf
		    ("Error: only root can update or install packages\n");
		return (EXIT_FAILURE);
	}

	struct pkglist *d = NULL;
	if (option >= 0)
		d = get_dependencies(name);

	while (d != NULL) {
		printf("%s [", d->name);
		if (strcmp(d->repo, "not found") == 0) {
			printf("not found]\n");
			not_found_helper();
			d = d->next;
			continue;
		}

		if (pkglist_exists(d->name, ilenia_pkgs) == 0) {
			printf("installed]\n");
			d = d->next;
			continue;
		}

		printf("install now]\n");
		if (build_install_pkg(option, d->name) != 0)
			return (EXIT_FAILURE);
		d = d->next;
	}

	if (pkglist_exists(name, ilenia_ports) != 0) {
		printf("%s [not found]\n", name);
		return (EXIT_FAILURE);
	}

	printf("%s [install now]\n", name);

	if (build_install_pkg(option, name) != 0)
		return (EXIT_FAILURE);

	return (EXIT_SUCCESS);
}

int update_system(int options)
{
	if (getuid() != 0) {
		printf
		    ("Error: only root can update or install packages\n");
		return (EXIT_FAILURE);
	}

	struct pkglist *p = NULL;
	struct pkglist *q = NULL;

	p = pkglist_confront(UPDATED, options, 0);

	if (pkglist_len(p) < 1) {
		printf("All packages are up-to-date\n");
		return (EXIT_SUCCESS);
	}

	while (p != NULL) {
		if (pkglist_exists(p->name, q) == 0)
			continue;

		char *repo = pkglist_get_newer(p->name, ilenia_ports);
		char *version = pkglist_get_from_repo(p->name, repo,
						      ilenia_ports);

		q = pkglist_add_ordered(p->name, version, repo, NULL, q);
		p = p->next;
	}

	/*
	 * someone wants that ilenia ask them if they're sure to update all 
	 * packages, I hate this feature, then I've to add another feature 
	 * that bypass this feature
	 */
	if (ask_for_update) {
		pkglist_print(q);
		if (ask
		    ("Are you sure to update the above packages? [Y/n] ")
		    == EXIT_FAILURE)
			return (EXIT_SUCCESS);
	}

	while (q != NULL) {
		if (build_install_pkg(options, q->name) != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		q = q->next;
	}

	return (EXIT_SUCCESS);
}


int remove_pkg(char *name, int checkdeps, int all)
{
	if (getuid() != 0) {
		printf("Error: only root can remove packages\n");
		return (EXIT_FAILURE);
	}

	struct pkglist *p = NULL;

	p = get_dependents(name, 0);


	if (pkglist_len(p) > 1 && checkdeps && !all) {
		printf
		    ("Warning: there are some packages that depends from %s, use --all or --no-deps, to remove all packages that depends from %s or to not check dependencies (use at your risk)\nYou can use ilenia -T --all%s to see a list of the packages that need %s.\n",
		     name, name, name, name);
		return (EXIT_FAILURE);
	}

	p = NULL;

	if (all)
		p = get_dependents(name, 1);
	else
		p = pkglist_add_reversed(name, NULL, NULL, NULL, p);

	while (p) {
		printf("Removing %s ...\n", p->name);
		char *args[] = { "pkgrm", p->name, NULL };
		if (exec(NULL, "/usr/bin/pkgrm", args) != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		p = p->next;
	}

	return (EXIT_SUCCESS);
}
