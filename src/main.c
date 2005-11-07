/***************************************************************************
 *            main.c
 *
 *  Sun Oct 30 12:33:33 2005
 *  Copyright  2005  Coviello Giuseppe
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

#include <argp.h>
#include <stdlib.h>
#include "../config.h"
#include "ilenia.h"
#include "lsports.h"
#include "lspkgs.h"
#include "aliaslist.h"
#include "update.h"
#include "repolist.h"
#include "pkglist.h"
#include "output.h"
#include "confront.h"
#include "dependencies.h"
#include "pkgutils.h"

const char *argp_program_version = VERSION;
const char *argp_program_bug_address = "<immigrant@email.it>";
static char doc[] = "Package manager for CRUX";
static char args_doc[] = "ARG1";

#define OPT_CACHE 1
#define OPT_REPOSITORY_LIST 2
#define OPT_NO_FAVORITE_REPO 3
#define OPT_NO_FAVORITE_VERSION 4
#define OPT_NO_DEPS 5
#define OPT_ALL 6

static struct argp_option options[] = {
	{"update", 'u', "REPO", OPTION_ARG_OPTIONAL, "Update ports tree"},
	{"list", 'l', "REPO", OPTION_ARG_OPTIONAL, "List ports"},
	{"search", 's', "PACKAGE", 0, "Search for ports"},
	{"diff", 'd', 0, 0, "List version differences"},
	{"updated", 'p', 0, 0, "List ports with newer version"},
	{"depedencies", 'D', "PACKAGE", 0,
	 "List dependencies of a package"},
	{"update-pkg", 'U', "PACKAGE", OPTION_ARG_OPTIONAL,
	 "Update or install a package with dependencies"},
	{"dependents", 'T', "PACKAGE", 0,
	 "List dependents of a package\n"},
	{"remove", 'R', "PACKAGE", 0,
	 "Remove a package checking dependencies"},
	{"cache", OPT_CACHE, 0, 0, "Rebuild the cache"},
	{"repository-list", OPT_REPOSITORY_LIST, 0, 0,
	 "List repository that ilenia are using"},
	{"no-favorite-repo", OPT_NO_FAVORITE_REPO, 0, 0,
	 "Ignore the user's favorite repos"},
	{"no-favorite-version", OPT_NO_FAVORITE_VERSION, 0, 0,
	 "Ignore the user's favorite versions"},
	{"no-deps", OPT_NO_DEPS, 0, 0, "Do not check dependencies"},
	{"all", OPT_ALL, 0, 0, "Shows or remove all dependents packages"},
	{0}
};

struct arguments {
	int update;
	char *update_repo;
	int list;
	char *list_repo;
	char *search_package;
	int diff;
	int updated;
	char *dependencies_package;
	int update_pkg;
	char *update_pkg_package;
	char *dependents_package;
	char *remove_package;
	int cache;
	int repository_list;
	int no_favorite_repo;
	int no_favorite_version;
	int no_deps;
	int all;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;

	switch (key) {
	case 'u':
		arguments->update = 1;
		arguments->update_repo = arg;
		break;
	case 'l':
		arguments->list = 1;
		arguments->list_repo = arg;
		break;
	case 's':
		arguments->search_package = arg;
		break;
	case 'd':
		arguments->diff = 1;
		break;
	case 'p':
		arguments->updated = 1;
		break;
	case 'D':
		arguments->dependencies_package = arg;
		break;
	case 'U':
		arguments->update_pkg = 1;
		arguments->update_pkg_package = arg;
		break;
	case 'T':
		arguments->dependents_package = arg;
		break;
	case 'R':
		arguments->remove_package = arg;
		break;
	case OPT_CACHE:
		arguments->cache = 1;
		break;
	case OPT_REPOSITORY_LIST:
		arguments->repository_list = 1;
		break;
	case OPT_NO_FAVORITE_REPO:
		arguments->no_favorite_repo = NO_FAVORITE_REPO;
		break;
	case OPT_NO_FAVORITE_VERSION:
		arguments->no_favorite_version = NO_FAVORITE_VERSION;
		break;
	case OPT_NO_DEPS:
		arguments->no_deps = -1;
		break;
	case OPT_ALL:
		arguments->all = 1;
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char **argv)
{
	struct arguments arguments;

	arguments.update = 0;
	arguments.update_repo = NULL;
	arguments.list = 0;
	arguments.list_repo = NULL;
	arguments.search_package = NULL;
	arguments.diff = 0;
	arguments.updated = 0;
	arguments.dependencies_package = NULL;
	arguments.update_pkg = 0;
	arguments.update_pkg_package = NULL;
	arguments.dependents_package = NULL;
	arguments.remove_package = NULL;
	arguments.cache = 0;
	arguments.repository_list = 0;
	arguments.no_favorite_repo = 0;
	arguments.no_favorite_version = 0;
	arguments.no_deps = 1;
	arguments.all = 0;

	if (argc < 2) {
		char *fake_arg[2];
		fake_arg[0] = "ilenia";
		fake_arg[1] = "--help";
		argp_parse(&argp, 2, fake_arg, 0, 0, &arguments);
	}

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	if (parse_ileniarc() != 0)
		return (EXIT_FAILURE);

	ilenia_repos = build_repolist();

	if (arguments.cache) {
		FILE *file;
		if ((file = fopen(CACHE, "w")))
			fclose(file);
	}

	ilenia_favoriterepo = get_favorite(FAVORITE_REPO);
	ilenia_favoriteversion = get_favorite(FAVORITE_VERSION);
	ilenia_aliases = aliaseslist_build();
	ilenia_ports = lsports();
	ilenia_pkgs = lspkgs();

	if (arguments.update_repo != NULL) {
		update_repo(arguments.update_repo);
		arguments.update = 0;
	}

	if (arguments.update)
		update_all_repos();

	if (arguments.list_repo != NULL) {
		if (repolist_exists(arguments.list_repo, ilenia_repos))
			pkglist_print(pkglist_select_from_repo
				      (arguments.list_repo, ilenia_ports));
		arguments.list = 0;
	}

	if (arguments.list)
		pkglist_print(ilenia_ports);

	if (arguments.search_package != NULL)
		pkglist_print(pkglist_find_like(arguments.search_package,
						ilenia_ports));

	int confront_options =
	    arguments.no_favorite_repo + arguments.no_favorite_version;

	int update_options = arguments.no_deps;
	if (confront_options)
		update_options = confront_options * update_options;

	if (arguments.diff)
		pkglist_confront(DIFF, confront_options, 1);

	if (arguments.updated)
		pkglist_confront(UPDATED, confront_options, 1);

	if (arguments.dependencies_package != NULL)
		print_dependencies(arguments.dependencies_package);

	if (arguments.update_pkg_package != NULL) {
		update_pkg(update_options, arguments.update_pkg_package);
		arguments.update_pkg = 0;
	}

	if (arguments.update_pkg)
		update_system(update_options);

	if (arguments.dependents_package != NULL)
		print_dependents(arguments.dependents_package,
				 arguments.all);

	if (arguments.remove_package != NULL)
		remove_pkg(arguments.dependents_package, arguments.no_deps,
			   arguments.all);

	if (arguments.repository_list) {
		while (ilenia_repos != NULL) {
			printf("name %s path %s\n", ilenia_repos->name,
			       ilenia_repos->path);
			ilenia_repos = ilenia_repos->next;
		}
	}

	exit(EXIT_SUCCESS);
}
