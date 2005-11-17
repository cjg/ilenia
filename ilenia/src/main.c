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
#include "list.h"

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

#define ACT_UPDATE 10
#define ACT_LIST 11
#define ACT_SEARCH 12
#define ACT_DIFF 13
#define ACT_UPDATED 14
#define ACT_DEPENDENCIES 15
#define ACT_UPDATE_PKG 16
#define ACT_DEPENDENTS 17
#define ACT_REMOVE 18
#define ACT_CACHE 19
#define ACT_REPOSITORY_LIST 20

static struct argp_option options[] = {
	{"update", 'u', 0, 0, "Update ports tree"},
	{"list", 'l', 0, 0, "List ports"},
	{"search", 's', 0, 0, "Search for ports"},
	{"diff", 'd', 0, 0, "List version differences"},
	{"updated", 'p', 0, 0, "List ports with newer version"},
	{"depedencies", 'D', 0, 0, "List dependencies of a package"},
	{"update-pkg", 'U', 0, 0,
	 "Update or install a package with dependencies"},
	{"dependents", 'T', 0, 0, "List dependents of a package\n"},
	{"remove", 'R', 0, 0, "Remove a package checking dependencies"},
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
	int action;
/*
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
*/
	int no_favorite_repo;
	int no_favorite_version;
	int no_deps;
	int all;
	struct list *args;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;

	switch (key) {
	case 'u':
		arguments->action += ACT_UPDATE;
		break;
	case 'l':
		arguments->action += ACT_LIST;
		break;
	case 's':
		arguments->action += ACT_SEARCH;
		break;
	case 'd':
		arguments->action += ACT_DIFF;
		break;
	case 'p':
		arguments->action += ACT_UPDATED;
		break;
	case 'D':
		arguments->action += ACT_DEPENDENCIES;
		break;
	case 'U':
		arguments->action += ACT_UPDATE_PKG;
		break;
	case 'T':
		arguments->action += ACT_DEPENDENTS;
		break;
	case 'R':
		arguments->action += ACT_REMOVE;
		break;
	case OPT_CACHE:
		arguments->action += ACT_CACHE;
		break;
	case OPT_REPOSITORY_LIST:
		arguments->action += ACT_REPOSITORY_LIST;
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
	case ARGP_KEY_ARG:
		arguments->args = list_add(arg, arguments->args);
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
	/*
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
	 */
	arguments.action = 0;
	arguments.no_favorite_repo = 0;
	arguments.no_favorite_version = 0;
	arguments.no_deps = 1;
	arguments.all = 0;
	arguments.args = NULL;

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

	if (arguments.action == ACT_CACHE) {
		FILE *file;
		if ((file = fopen(CACHE, "w")))
			fclose(file);
	}

	ilenia_favoriterepo = get_favorite(FAVORITE_REPO);
	ilenia_favoriteversion = get_favorite(FAVORITE_VERSION);
	ilenia_aliases = aliaseslist_build();
	ilenia_ports = lsports();
	ilenia_pkgs = lspkgs();

	if (arguments.action > 20 || arguments.action == 0) {
		printf("Error: please perform an action at a time!\n");
		return (EXIT_FAILURE);
	}

	int confront_options =
	    arguments.no_favorite_repo + arguments.no_favorite_version;

	int update_options = arguments.no_deps;
	if (confront_options)
		update_options = confront_options * update_options;

	if (arguments.action == ACT_UPDATE) {
		if (arguments.args == NULL) {
			update_all_repos();
			return (EXIT_SUCCESS);
		}
		while (arguments.args) {
			update_repo(arguments.args->data);
			arguments.args = arguments.args->next;
		}
	}

	if (arguments.action == ACT_LIST) {
		if (arguments.args == NULL) {
			pkglist_print(ilenia_ports);
			return (EXIT_SUCCESS);
		}
		while (arguments.args) {
			pkglist_print(pkglist_select_from_repo
				      (arguments.args->data,
				       ilenia_ports));
			arguments.args = arguments.args->next;
		}
	}

	if (arguments.action == ACT_SEARCH) {
		if (arguments.args == NULL) {
			printf
			    ("Error: action search requires an argument!\n");
			return (EXIT_FAILURE);
		}
		while (arguments.args) {
			pkglist_print(pkglist_find_like
				      (arguments.args->data,
				       ilenia_ports));
			arguments.args = arguments.args->next;
		}
	}

	if (arguments.action == ACT_DIFF)
		pkglist_confront(DIFF, confront_options, 1);

	if (arguments.action == ACT_UPDATED)
		pkglist_confront(UPDATED, confront_options, 1);

	if (arguments.action == ACT_DEPENDENCIES) {
		if (arguments.args == NULL) {
			printf
			    ("Error: action dependencies requires an argument!\n");
			return (EXIT_FAILURE);
		}
		while (arguments.args) {
			print_dependencies(arguments.args->data);
			arguments.args = arguments.args->next;
		}
	}

	if (arguments.action == ACT_UPDATE_PKG) {
		if (arguments.args == NULL) {
			update_system(update_options);
			return (EXIT_SUCCESS);
		}
		while (arguments.args) {
			update_pkg(update_options, arguments.args->data);
			arguments.args = arguments.args->next;
		}
	}

	if (arguments.action == ACT_DEPENDENTS) {
		if (arguments.args == NULL) {
			printf
			    ("Error: action dependents requires an argument!\n");
			return (EXIT_FAILURE);
		}
		while (arguments.args) {
			print_dependents(arguments.args->data,
					 arguments.all);
			arguments.args = arguments.args->next;
		}
	}

	if (arguments.action == ACT_REMOVE) {
		if (arguments.args == NULL) {
			printf
			    ("Error: action remove requires an argument!\n");
			return (EXIT_FAILURE);
		}
		while (arguments.args) {
			remove_pkg(arguments.args->data, arguments.no_deps,
				   arguments.all);
			arguments.args = arguments.args->next;
		}
	}


	if (arguments.action == ACT_REPOSITORY_LIST) {
		while (ilenia_repos != NULL) {
			printf("name %s path %s\n", ilenia_repos->name,
			       ilenia_repos->path);
			ilenia_repos = ilenia_repos->next;
		}
	}

	exit(EXIT_SUCCESS);
}
