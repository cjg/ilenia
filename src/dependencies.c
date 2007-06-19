/* dependencies.c */

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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dependencies.h"
#include "memory.h"
#include "port.h"
#include "output.h"
#include "str.h"

static char *status[] = {
	"",
	"[GREEN]installed[DEFAULT]",
	"[GREEN]installed[DEFAULT]",
	"[YELLOW]outdated[DEFAULT]"
};

static char *not_found = "[RED]not found[DEFAULT]";

static void deptree_data_dump(port_t * port, unsigned level, dict_t * seen)
{
	unsigned i;
	assert(port != NULL && seen != NULL);

	for (i = 0; i < level * 2; i++)
		printf(" ");
	cprintf(stdout, "+--%s [%s]", port->name, status[port->status]);
	if (dict_get(seen, port->name) != NULL) {
		cprintf(stdout, " [BLUE](already seen)[DEFAULT]\n");
		return;
	}
	printf("\n");
	dict_add(seen, port->name, "");

	level++;
	if (port->dependencies_exploded == NULL) {
		printf("%s\n", port->name);
		abort();
	}
	for (i = 0; i < port->dependencies_exploded->length; i++)
		deptree_data_dump(port->dependencies_exploded->elements[i],
				  level, seen);
}

static void deptree_verbose_data_dump(port_t * port, unsigned level, dict_t *
				      seen)
{
	unsigned i;
	assert(port != NULL && seen != NULL);

	for (i = 0; i < level * 2; i++)
		printf(" ");
	cprintf(stdout, "+--%s [%s] (%s)", port->name, status[port->status],
		port->repository != NULL ? port->repository->name : not_found);
	if (dict_get(seen, port->name) != NULL) {
		cprintf(stdout, " [BLUE](already seen)[DEFAULT]\n");
		return;
	}
	printf("\n");
	dict_add(seen, port->name, "");

	level++;
	for (i = 0; i < port->dependencies_exploded->length; i++)
		deptree_verbose_data_dump(port->dependencies_exploded->
					  elements[i], level, seen);
}

static void deplist_dump_port(port_t * port)
{
	cprintf(stdout, "%s [%s]\n", port->name, status[port->status]);
}

static void deplist_verbose_dump_port(port_t * port)
{
	cprintf(stdout, "%s [%s] %s(%s)\n", port->name, status[port->status],
		port_have_readme(port) ? "[BLUE]README[DEFAULT] " : "",
		port->repository != NULL ? port->repository->name : not_found);
}

static void
dependencies_explode(port_t * port, dict_t * ports_dict, dict_t * aliases,
		     dict_t * not_founds)
{
	unsigned i, j;
	port_t *dport, *aport;
	list_t *list;
	assert(port != NULL);

	if (port->dependencies_exploded != NULL)
		return;

	port->dependencies_exploded = list_new();
	for (i = 0; i < port->dependencies->length; i++) {
		dport =
		    dict_get(ports_dict,
			     (char *)port->dependencies->elements[i]);
		if (dport == NULL && (dport = dict_get(not_founds,
						       (char
							*)port->dependencies->
						       elements[i])) == NULL) {
			dport =
			    port_new(xstrdup
				     ((char *)port->dependencies->elements[i]),
				     xstrdup(""), NULL, list_new(), NULL);
			dport->status = PRT_NOTINSTALLED;
			dict_add(not_founds, (char
					      *)port->dependencies->elements[i],
				 dport);
		}
		if (dport->status == PRT_NOTINSTALLED &&
		    (list = dict_get(aliases, dport->name))) {
			for (j = 0; j < list->length; j++) {
				aport = dict_get(ports_dict, list->elements[j]);
				if (aport == NULL)
					continue;
				if (aport->status != PRT_NOTINSTALLED) {
					dport = aport;
					break;
				}
			}
		}
		list_append(port->dependencies_exploded, dport);
		if (dport->dependencies_exploded == NULL)
			dependencies_explode(dport, ports_dict, aliases,
					     not_founds);
	}
}

static unsigned dependencies_compact(port_t * port, unsigned deep,
				     dict_t * seen)
{
	unsigned i, ndeep, mdeep;

	if (port->dependencies_exploded->length == 0)
		return deep;
	deep++;
	mdeep = deep;
	for (i = 0; i < port->dependencies_exploded->length; i++) {
		((port_t *)
		 port->dependencies_exploded->elements[i])->deep = deep;
		if (dict_get(seen, ((port_t *)
				    port->dependencies_exploded->elements[i])->
			     name) != NULL)
			continue;
		dict_add(seen, ((port_t *)
				port->dependencies_exploded->elements[i])->name,
			 port->dependencies_exploded->elements[i]);
		ndeep =
		    dependencies_compact(port->dependencies_exploded->
					 elements[i], deep, seen);
		if (ndeep > mdeep)
			mdeep = ndeep;
	}
	return mdeep;
}

static void dependencies_append_at_deep(list_t * list, dict_t * dict, unsigned
					deep)
{
	unsigned i;
	port_t *port;
	for (i = 0; i < dict->length; i++) {
		port = dict->elements[i]->value;
		if (port->deep == deep)
			list_append(list, port);
	}
}

static list_t *dependencies_list_merge(list_t * list1, list_t * list2)
{
	unsigned i;
	dict_t *dict1;
	port_t *port;

	dict1 = dict_new();

	for (i = 0; i < list1->length; i++) {
		port = list1->elements[i];
		dict_add(dict1, port->name, port);
	}

	for (i = 0; i < list2->length; i++) {
		port = list2->elements[i];
		if (dict_get(dict1, port->name) == NULL) {
			dict_add(dict1, port->name, "");
			list_append(list1, port);
		}
	}

	dict_free(dict1, NULL);

	return list1;
}

list_t *dependencies_list(list_t * self, char *port_name, dict_t * ports_dict,
			  dict_t * aliases, dict_t * not_founds, int
			  enable_xterm_title)
{
	port_t *port;
	list_t *deplist;
	dict_t *seen;
	unsigned deep;
	int i;

	assert(port_name != NULL && ports_dict != NULL && aliases != NULL &&
	       self != NULL);

	if ((port = dict_get(ports_dict, port_name)) == NULL) {
		warning("%s not found!", port_name);
		return NULL;
	}
	
	if (enable_xterm_title)
		xterm_set_title("Calculating dependencies ...");
	dependencies_explode(port, ports_dict, aliases, not_founds);
	seen = dict_new();
	deep = dependencies_compact(port, 0, seen);
	deplist = list_new();
	dict_add(seen, port->name, port);
	port->deep = 0;
	for (i = deep; i >= 0; i--)
		dependencies_append_at_deep(deplist, seen, i);
	dict_free(seen, NULL);
	dependencies_list_merge(self, deplist);
	list_free(deplist, NULL);
	return self;
}

void
dependencies_dump(list_t * ports_name, dict_t * ports_dict, dict_t * aliases,
		  dict_t * not_founds, int tree, int verbose, 
		  int enable_xterm_title)
{
	unsigned i;
	port_t *port;
	list_t *deplist;
	dict_t *seen;

	assert(ports_name != NULL && ports_dict != NULL && aliases != NULL &&
	       not_founds != NULL);
	
	for (i = 0; i < ports_name->length; i++) {
		if ((port = dict_get(ports_dict,
				     (char *)ports_name->elements[i])) ==
		    NULL) {
			warning("%s not found!", ports_name->elements[i]);
			list_remove(ports_name, i--, NULL);
			continue;
		}
		dependencies_explode(port, ports_dict, aliases, not_founds);
	}

	if (tree) {
		for (i = 0; i < ports_name->length; i++) {
			port = dict_get(ports_dict,
					(char *)ports_name->elements[i]);
			seen = dict_new();
			if (verbose)
				deptree_verbose_data_dump(port, 0, seen);
			else
				deptree_data_dump(port, 0, seen);
			dict_free(seen, NULL);
		}
	} else {
		deplist = list_new();
		for (i = 0; i < ports_name->length; i++)
			dependencies_list(deplist, ports_name->elements[i],
					  ports_dict, aliases, not_founds,
				enable_xterm_title);
		if (deplist == NULL)
			return;
		if (verbose)
			list_dump(deplist, deplist_verbose_dump_port);
		else
			list_dump(deplist, deplist_dump_port);
		list_free(deplist, NULL);
	}
}

static void dependents_list_append(list_t * self, port_t * port, dict_t *
				   ports_dict, dict_t * aliases,
				   dict_t * seen, int all)
{
	unsigned i, j;
	port_t *pport;
	assert(port != NULL && seen != NULL && self != NULL);

	if (dict_get(seen, port->name) == NULL)
		list_append(self, port);
	else
		return;

	dict_add(seen, port->name, "");

	for (i = 0; i < ports_dict->length; i++) {
		pport = ports_dict->elements[i]->value;
		if (all != 1 && pport->status == PRT_NOTINSTALLED)
			continue;
		for (j = 0; j < pport->dependencies->length; j++) {
			if (strcmp(pport->dependencies->elements[j],
				   port->name) == 0) {
				dependents_list_append(self, pport, ports_dict,
						       aliases, seen, all);
			}
		}
	}

	for (i = 0; i < aliases->length; i++) {
		if (list_get_position((list_t *) aliases->elements[i]->value,
				      strequ, port->name) < 0)
			continue;
		pport = dict_get(ports_dict, aliases->elements[i]->key);
		dependents_list_append(self, pport, ports_dict, aliases,
				       seen, all);
	}
}

list_t *dependents_list(char *port_name, dict_t * ports_dict, dict_t * aliases,
			int all, int enable_xterm_title)
{
	list_t *self;
	dict_t *seen;
	port_t *port;

	assert(port_name != NULL && ports_dict != NULL && aliases != NULL);

	if ((port = dict_get(ports_dict, port_name)) == NULL) {
		warning("%s not found!", port_name);
		return NULL;
	}

	if (enable_xterm_title)
		xterm_set_title("Calculating dependents ...");

	self = list_new();
	seen = dict_new();

	dependents_list_append(self, port, ports_dict, aliases, seen, all);

	dict_free(seen, NULL);

	return self;

}

static void dependents_tree_dump(port_t * port, dict_t * ports_dict, dict_t
				 * aliases,
				 unsigned level, dict_t * seen, int
				 verbose, int all)
{
	unsigned i, j;
	port_t *pport;
	assert(port != NULL && seen != NULL);

	for (i = 0; i < level * 2; i++)
		printf(" ");
	if (verbose)
		cprintf(stdout, "+--%s [%s] (%s)", port->name,
			status[port->status],
			port->repository !=
			NULL ? port->repository->name : not_found);
	else
		cprintf(stdout, "+--%s [%s]", port->name, status[port->status]);
	if (dict_get(seen, port->name) != NULL) {
		cprintf(stdout, " [BLUE](already seen)[DEFAULT]\n");
		return;
	}
	printf("\n");
	dict_add(seen, port->name, "");

	level++;
	for (i = 0; i < ports_dict->length; i++) {
		pport = ports_dict->elements[i]->value;
		if (all != 1 && pport->status == PRT_NOTINSTALLED)
			continue;
		for (j = 0; j < pport->dependencies->length; j++) {
			if (strequ(pport->dependencies->elements[j],
				   port->name)) {
				dependents_tree_dump(pport, ports_dict, aliases,
						     level + 1, seen,
						     verbose, all);
			}
		}
	}

	for (i = 0; i < aliases->length; i++) {
		if (list_get_position((list_t *) aliases->elements[i]->value,
				      strequ, port->name) < 0)
			continue;
		pport = dict_get(ports_dict, aliases->elements[i]->key);
		dependents_tree_dump(pport, ports_dict, aliases, level,
				     seen, verbose, all);
	}
}

void dependents_list_dump(port_t * port, dict_t * ports_dict,
			  dict_t * aliases, int verbose, int all, 
			  int enable_xterm_title)
{
	list_t *detlist;

	detlist = dependents_list(port->name, ports_dict, aliases, all, 
				  enable_xterm_title);
	if (detlist == NULL)
		return;
	if (verbose)
		list_dump(detlist, deplist_verbose_dump_port);
	else
		list_dump(detlist, deplist_dump_port);
	list_free(detlist, NULL);
}

void dependents_dump(char *port_name, dict_t * ports_dict,
		     dict_t * aliases, int tree, int verbose, int all, int enable_xterm_title)
{
	port_t *port;
	dict_t *seen;
	assert(port_name != NULL && ports_dict != NULL && aliases != NULL);
	if ((port = dict_get(ports_dict, port_name)) == NULL)
		return warning("%s not found!", port_name);
	if (tree) {
		seen = dict_new();
		dependents_tree_dump(port, ports_dict, aliases, 0,
				     seen, verbose, all);
		dict_free(seen, NULL);
	} else
		dependents_list_dump(port, ports_dict, aliases, verbose, all, enable_xterm_title);
}
