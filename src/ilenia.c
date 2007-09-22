/* ilenia.c */

/* ilenia -- A package manager for CRUX
 *
 * Copyright (C) 2006 - 2007
 *     Giuseppe Coviello <cjg@cruxppc.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <argp.h>
#include <stdlib.h>
#include <string.h>
#include "../config.h"
#include "port.h"
#include "package.h"
#include "dependencies.h"
#include "conf.h"
#include "cache.h"
#include "driver.h"
#include "output.h"
#include "info.h"
#include "update.h"
#include "str.h"
#include "memory.h"
#include "remove.h"

const char *argp_program_version = "ilenia " VERSION " (Pianeta Amiga)"
    "\n"
    "Copyright (C) 2006-2007 Giuseppe Coviello.\n"
    "This is free software.  You may redistribute copies of it under the terms of\n"
    "the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.\n"
    "There is NO WARRANTY, to the extent permitted by law.";
const char *argp_program_bug_address = "Giuseppe Coviello <cjg@cruxppc.org>";
static char doc[] = "A package manager for CRUX (and CRUX PPC of course)";
static char args_doc[] = "ACT [ARG(S)]";

enum OPT { OPT_CACHE = 300, OPT_REPOSITORY_LIST, OPT_NO_FAVOURITE_REPOSITORIES,
	   OPT_NO_LOCKED_VERSIONS, OPT_ALL, OPT_FETCH_ONLY, OPT_JUST_INSTALL,
	   OPT_NO_REPOSITORIES_HIERARCHY, OPT_TREE, OPT_NO_ALIASES,
	   OPT_NO_COLORS, OPT_NO_NEVER_INSTALL
};

enum ACT { ACT_UPDATE = 300, ACT_LIST, ACT_SEARCH, ACT_INFO, ACT_README,
	ACT_DIFF, ACT_UPDATED, ACT_DEPENDENCIES, ACT_UPDATE_PKG,
	ACT_DEPENDENTS, ACT_REMOVE, ACT_REPOSITORY_LIST, ACT_SEARCHDESC
};

static struct argp_option options[] = {
	{"ACTIONS", 0, 0, OPTION_DOC, "", 0},
	{"update", 'u', 0, 0, "Update ports tree", 1},
	{"list", 'l', 0, 0, "List ports", 1},
	{"search", 's', 0, 0, "Search for ports by name", 1},
	{"search-desc", ACT_SEARCHDESC, 0, 0,
	 "Search for ports by description", 1},
	{"info", 'i', 0, 0, "Get info on a port", 1},
	{"show-readme", 'r', 0, 0, "Show the README of a port", 1},
	{"diff", 'd', 0, 0, "List version differences", 1},
	{"updated", 'p', 0, 0, "List ports with newer version", 1},
	{"depedencies", 'D', 0, 0, "List dependencies of a package", 1},
	{"update-pkg", 'U', 0, 0,
	 "Update or install a package with dependencies", 1},
	{"dependents", 'T', 0, 0, "List dependents of a package", 1},
	{"remove", 'R', 0, 0, "Remove a package checking dependencies", 1},
	{"\n  OPTIONS", 0, 0, OPTION_DOC, "", 2},
	{"tree", OPT_TREE, 0, 0, "Dump a tree dependencies insted of a list",
	 3},
	{"cache", OPT_CACHE, 0, 0, "Rebuild the cache", 3},
	{"repository-list", OPT_REPOSITORY_LIST, 0, 0,
	 "List repository that ilenia are using", 1},
	{"no-favourite", OPT_NO_FAVOURITE_REPOSITORIES, 0, 0,
	 "Ignore the user's favourite repositories", 3},
	{"no-locked", OPT_NO_LOCKED_VERSIONS, 0, 0,
	 "Ignore the user's favorite versions", 3},
	{"no-colors", OPT_NO_COLORS, 0, 0, "Disable colored output", 3},
	{"all", OPT_ALL, 0, 0, "Shows or remove all dependents packages", 3},
	{"fetch-only", OPT_FETCH_ONLY, 0, 0, "Just fetch the needed sources",
	 3},
	{"just-install", OPT_JUST_INSTALL, 0, 0, 
	 "Just install dependencies don't update outdate ones"},
	{"no-hierarchy", OPT_NO_REPOSITORIES_HIERARCHY, 0, 0,
	 "Do not use the repositories gerarchy", 3},
	{"no-aliases", OPT_NO_ALIASES, 0, 0, "Do no use aliases", 3},
	{"verbose", 'v', 0, 0, "Generate verbose output", 3},
	{"no-never-install", OPT_NO_NEVER_INSTALL, 0, 0, 
	 "Avoid the never_install list", 3},
	{0}
};

struct arguments {
	int action;
	int no_favourite_repositories;
	int no_locked_versions;
	int no_aliases;
	int no_colors;
	int no_deps;
	int no_never_install;
	int all;
	int fetch_only;
	int just_install;
	int no_repositories_hierarchy;
	int tree;
	int verbose;
	int rebuild_cache;
	list_t *args;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state);

static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char **argv)
{
	unsigned i;
	conf_t *conf;
	dict_t *repositories;
	dict_t *ports_dict;
	dict_t *not_founds;
	list_t *ports_list;
	list_t *packages;
	list_t *list;
	list_t *drivers;
	struct arguments arguments;

	arguments.action = 0;
	arguments.no_favourite_repositories = 0;
	arguments.no_locked_versions = 0;
	arguments.no_aliases = 0;
	arguments.no_colors = 0;
	arguments.no_deps = 1;
	arguments.no_never_install = 0;
	arguments.all = 0;
	arguments.fetch_only = 0;
	arguments.just_install = 0;
	arguments.no_repositories_hierarchy = 0;
	arguments.tree = 0;
	arguments.verbose = 0;
	arguments.rebuild_cache = 0;
	arguments.args = list_new();

	if (argc < 2) {
		char *fake_arg[2];
		fake_arg[0] = "ilenia";
		fake_arg[1] = "--help";
		argp_parse(&argp, 2, fake_arg, 0, 0, &arguments);
	}

	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	debug("arguments parsed!");

	conf = conf_init();
	debug("configuration initialized!");

	if (arguments.no_repositories_hierarchy) {
		list_free(conf->repositories_hierarchy, free);
		conf->repositories_hierarchy = list_new();
	}

	if (arguments.no_favourite_repositories) {
		dict_free(conf->favourite_repositories, NULL);
		conf->favourite_repositories = dict_new();
	}

	if (arguments.no_locked_versions) {
		dict_free(conf->locked_versions, free);
		conf->locked_versions = dict_new();
	}

	if (arguments.no_aliases) {
		aliases_free(conf->aliases);
		conf->aliases = dict_new();
	}

	if (arguments.no_never_install) {
		list_free(conf->never_install, free);
		conf->never_install = list_new();
	}

	drivers = drivers_list_init();
	repositories = repositories_dict_init(drivers,
					      conf->repositories_hierarchy);
	if (!conf->enable_colors || arguments.no_colors)
		cprintf = uncoloredprintf;
	else
		cprintf = coloredprintf;

	if (arguments.rebuild_cache)
		cache_build(repositories, conf->enable_xterm_title);
	packages = packages_list_init();
	if ((ports_list = ports_list_init(repositories,
					  conf->enable_xterm_title)) == NULL) {
		list_free(arguments.args, NULL);
		dict_free(repositories, repository_free);
		list_free(packages, port_free);
		list_free(drivers, free);
		dict_free(conf->favourite_repositories, free);
		conf->favourite_repositories = dict_new();
		conf_free(conf);
		return 1;
	}

	conf_reparse(conf, ports_list);
#ifndef NDEBUG
	conf_dump(conf);
#endif
	ports_dict = ports_dict_init(ports_list, packages, conf);
	not_founds = dict_new();

	if (!arguments.verbose)
		arguments.verbose = conf->verbose;

	switch (arguments.action) {
	case ACT_LIST:
		if (!arguments.args->length) {
			list_dump(ports_list, port_dump);
			break;
		}
		for (i = 0; i < arguments.args->length; i++) {
			if (dict_get(repositories, list_get(arguments.args, i))
			    == NULL) {
				warning("repository %s not found!",
					list_get(arguments.args, i));
				continue;
			}
			list = list_query(ports_list,
					  port_query_by_repository,
					  list_get(arguments.args, i));
			list_dump(list, port_dump);
			list_free(list, NULL);
		}
		break;
	case ACT_SEARCH:
		if (!arguments.args->length) {
			warning("action --search (-s) requires at least an "
				"argument!");
			break;
		}
		for (i = 0; i < arguments.args->length; i++) {
			list = list_query(ports_list, port_query_by_name,
					  list_get(arguments.args, i));
			if (arguments.verbose)
				list_dump(list, port_info_dump);
			else
				list_dump(list, port_dump);
			if (list->length == 0)
				printf("%s not found!\n",
				       (char *)list_get(arguments.args, i));
			list_free(list, NULL);
		}
		break;
	case ACT_SEARCHDESC:
		if (!arguments.args->length) {
			warning("action --search-desc requires at least an "
				"argument!");
			break;
		}
		for (i = 0; i < arguments.args->length; i++) {
			char *key = xstrdup(list_get(arguments.args, i));
			if (isalpha(*key) && isalpha(*(key + strlen(key) - 1))) {
				strprepend(&key, "*");
				strappend(&key, "*");
			}
			list = list_query(ports_list, port_query_by_description,
					  key);
			if (arguments.verbose)
				list_dump(list, port_info_dump);
			else
				list_dump(list, port_dump);
			list_free(list, NULL);
			free(key);
		}
		break;
	case ACT_DIFF:
		port_show_diffs(ports_dict, packages, conf->enable_xterm_title);
		break;
	case ACT_UPDATED:
		port_show_outdated(ports_dict, packages, conf->enable_xterm_title);
		break;
	case ACT_DEPENDENCIES:
		if (!arguments.args->length) {
			warning("action --dependencies (-D) requires at "
				"least an argument!");
			break;
		}
		//for (i = 0; i < arguments.args->length; i++)
		dependencies_dump(arguments.args,
				  ports_dict, conf->aliases,
				  not_founds,
				  arguments.tree, arguments.verbose, 
				  conf->enable_xterm_title);
		break;
	case ACT_DEPENDENTS:
		if (!arguments.args->length) {
			warning("action --dependents (-T) requires at "
				"least an argument!");
			break;
		}
		for (i = 0; i < arguments.args->length; i++)
			dependents_dump(list_get(arguments.args, i),
					ports_dict, conf->aliases,
					arguments.tree, arguments.verbose,
					arguments.all, conf->enable_xterm_title);
		break;
	case ACT_INFO:
		if (!arguments.args->length) {
			warning("action --info requires at "
				"least an argument!");
			break;
		}
		for (i = 0; i < arguments.args->length; i++)
			info_dump(list_get(arguments.args, i), ports_dict);
		break;
	case ACT_README:
		if (!arguments.args->length) {
			warning("action --show-readme requires at "
				"least an argument!");
			break;
		}
		for (i = 0; i < arguments.args->length; i++)
			readme_dump(list_get(arguments.args, i), ports_dict);
		break;
	case ACT_REPOSITORY_LIST:
		repositories_dict_dump(repositories);
		break;
	case ACT_UPDATE:
		if (arguments.args->length)
			repositories_dict_update(repositories, arguments.args,
						 conf->enable_xterm_title); 
		else
			repositories_dict_update_all(repositories,
				conf->enable_xterm_title);
		break;
	case ACT_REMOVE:
		if (!arguments.args->length) {
			warning("action --remove (-R) requires at "
				"least an argument!");
			break;
		}
		remove_packages(arguments.args, packages, ports_dict, conf,
				arguments.all);
		break;
	case ACT_UPDATE_PKG:
		if (!arguments.args->length) {
			update_system(ports_dict,
				      arguments.fetch_only,
				      conf);
			break;
		}
		update_package(arguments.args, ports_dict,
			       arguments.fetch_only,
			       conf, arguments.just_install);
		break;
	default:
		if (!arguments.rebuild_cache)
			warning("What can I do for you?");
	}

	if (conf->enable_xterm_title)
		xterm_reset_title(conf->default_xterm_title);

	list_free(arguments.args, NULL);
	dict_free(repositories, repository_free);
	list_free(ports_list, port_free);
	list_free(packages, port_free);
	dict_free(ports_dict, NULL);
	list_free(drivers, free);
	conf_free(conf);
	dict_free(not_founds, port_free);

	return 0;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;

	switch (key) {
	case 'u':
		arguments->action = ACT_UPDATE;
		break;
	case 'l':
		arguments->action = ACT_LIST;
		break;
	case 's':
		arguments->action = ACT_SEARCH;
		break;
	case 'i':
		arguments->action = ACT_INFO;
		break;
	case 'r':
		arguments->action = ACT_README;
		break;
	case 'd':
		arguments->action = ACT_DIFF;
		break;
	case 'p':
		arguments->action = ACT_UPDATED;
		break;
	case 'D':
		arguments->action = ACT_DEPENDENCIES;
		break;
	case 'U':
		arguments->action = ACT_UPDATE_PKG;
		break;
	case 'T':
		arguments->action = ACT_DEPENDENTS;
		break;
	case 'R':
		arguments->action = ACT_REMOVE;
		break;
	case ACT_SEARCHDESC:
		arguments->action = ACT_SEARCHDESC;
		break;
	case OPT_CACHE:
		arguments->rebuild_cache = 1;
		break;
	case OPT_REPOSITORY_LIST:
		arguments->action = ACT_REPOSITORY_LIST;
		break;
	case OPT_NO_FAVOURITE_REPOSITORIES:
		arguments->no_favourite_repositories = 1;
		break;
	case OPT_NO_LOCKED_VERSIONS:
		arguments->no_locked_versions = 1;
		break;
	case OPT_ALL:
		arguments->all = 1;
		break;
	case OPT_FETCH_ONLY:
		arguments->fetch_only = 1;
		break;
	case OPT_JUST_INSTALL:
		arguments->just_install = 1;
		break;
	case OPT_NO_REPOSITORIES_HIERARCHY:
		arguments->no_repositories_hierarchy = 1;
		break;
	case OPT_TREE:
		arguments->tree = 1;
		break;
	case OPT_NO_ALIASES:
		arguments->no_aliases = 1;
		break;
	case OPT_NO_COLORS:
		arguments->no_colors = 1;
		break;
	case OPT_NO_NEVER_INSTALL:
		arguments->no_never_install = 1;
		break;
	case 'v':
		arguments->verbose = 1;
		break;
	case ARGP_KEY_ARG:
		list_append(arguments->args, arg);
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}
