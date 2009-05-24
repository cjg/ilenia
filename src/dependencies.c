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
#define PORT(port) ((port_t *)(port))

static char *status[] = {
	"",
	"[GREEN]installed[DEFAULT]",
	"[GREEN]installed[DEFAULT]",
	"[YELLOW]outdated[DEFAULT]",
	"[RED]never install[DEFAULT]"
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

static inline void dependencies_explode(port_t * port, hash_t * ports_hash, 
					dict_t * aliases, dict_t * not_founds)
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
		    hash_get(ports_hash,
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
				aport = hash_get(ports_hash, list->elements[j]);
				if (aport == NULL)
					continue;
				dport = aport;
				if (aport->status != PRT_NOTINSTALLED) 
					break;
			}
		}
		list_append(port->dependencies_exploded, dport);
		dependencies_explode(dport, ports_hash, aliases, not_founds);
	}
}

static int dependencies_compact_and_check(list_t *stack, hash_t *ports_hash,
					  dict_t *aliases, dict_t *not_founds,
					  dict_t *seen, unsigned deep)
{
	unsigned i;
	port_t *port;

	assert(stack != NULL);

	if(stack->length == 0)
		return 0;
	
	port = list_get_port(stack, 0);

	dependencies_explode(port, ports_hash, aliases, not_founds);

	deep++;

	for(i = 0; i < port->dependencies_exploded->length; i++) {
		if(list_get_port(port->dependencies_exploded, i)->deep == 0)
			dict_add(seen,
				 list_get_port(port->dependencies_exploded,
					       i)->name,
				 list_get_port(port->dependencies_exploded, i));
		if(list_get_port(port->dependencies_exploded, i)->deep < deep)
			list_get_port(port->dependencies_exploded, i)->deep =
				deep;
		if(list_get_port(port->dependencies_exploded,i)->cyclic_dependencies_status
		   == CYCLIC_DEPENDENCIES_EXIST 
		   || (list_get_port(port->dependencies_exploded,i)->cyclic_dependencies_status
		   == CYCLIC_DEPENDENCIES_NOT_CHECKED 
		       && list_get_position(stack, port_query_by_name, 
					    list_get_port(port->dependencies_exploded, 
							  i)->name)!= -1)) {
			error("Found a cyclic dependence in: %s (%s<==>%s)!", 
			      list_get_port(stack, 0)->name,
			      list_get_port(stack, 0)->name,
			      list_get_port(port->dependencies_exploded, i)->name);
			
			return 1;
		}
		
		list_prepend(stack, list_get_port(port->dependencies_exploded, 
						  i));
		if (dependencies_compact_and_check(stack, ports_hash, aliases,
						   not_founds, seen, deep) != 0)
						  {
			list_get_port(stack, 0)->cyclic_dependencies_status =
				CYCLIC_DEPENDENCIES_EXIST;
			return 1;
		}
		list_get_port(stack, 0)->cyclic_dependencies_status =
			CYCLIC_DEPENDENCIES_CHECKED;
		list_remove(stack, 0, NULL);
	}
	
	return 0;
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

static int port_deep_cmp(void *port1, void *port2)
{
	return ((int)PORT(port2)->deep) - ((int)PORT(port1)->deep);
}


static inline list_t *dependencies_list_from_dict(dict_t *seen)
{
	unsigned i;
	list_t *dependencies;
	
	dependencies = list_new_with_size(seen->length);
	for(i=0;i<seen->length;i++)
		list_append(dependencies, dict_get_port_at(seen, i));
	return list_sort(dependencies, port_deep_cmp);
}

list_t *dependencies_list(list_t * self, char *port_name, hash_t * ports_hash,
			  dict_t * aliases, dict_t * not_founds, int
			  enable_xterm_title)
{
	port_t *port;
	list_t *deplist;
	list_t *stack;
	dict_t *seen;

	assert(port_name != NULL && ports_hash != NULL && aliases != NULL &&
	       self != NULL);

	if ((port = hash_get(ports_hash, port_name)) == NULL) {
		warning("%s not found!", port_name);
		return NULL;
	}
	
	if (enable_xterm_title)
		xterm_set_title("Calculating dependencies ...");
	stack = list_new();
	list_append(stack, port);
	seen = dict_new();
	if(dependencies_compact_and_check(stack, ports_hash, aliases,
					  not_founds, seen, 0)) {
		list_free(stack, NULL);
		return NULL;
	}
	list_free(stack, NULL);
	dict_add(seen, port->name, port);
	deplist = dependencies_list_from_dict(seen);
	dict_free(seen, NULL);
	dependencies_list_merge(self, deplist);
	list_free(deplist, NULL);
	return self;
}

list_t *dependencies_multiple_list(list_t *ports_name, 
				   hash_t * ports_hash, dict_t * aliases, 
				   dict_t * not_founds, int enable_xterm_title)
{
	unsigned i;
	port_t *port;
	list_t *self;

	assert(ports_name != NULL && ports_hash != NULL && aliases != NULL);

	for (i = 0; i < ports_name->length; i++) {
		if ((port = hash_get(ports_hash,
				     (char *)ports_name->elements[i])) ==
		    NULL) {
			warning("%s not found!", ports_name->elements[i]);
			list_remove(ports_name, i--, NULL);
			continue;
		}
		dependencies_explode(port, ports_hash, aliases, not_founds);
	}
	
	self = list_new();
	for (i = 0; i < ports_name->length; i++) {
		dependencies_list(self, ports_name->elements[i],
				  ports_hash, aliases, not_founds,
				  enable_xterm_title);
	}
	return self;
}

void
dependencies_dump(list_t * ports_name, hash_t * ports_hash, dict_t * aliases,
		  dict_t * not_founds, int tree, int verbose, 
		  int enable_xterm_title)
{
	unsigned i;
	port_t *port;
	list_t *deplist;
	dict_t *seen;

	assert(ports_name != NULL && ports_hash != NULL && aliases != NULL &&
	       not_founds != NULL);
	
	for (i = 0; i < ports_name->length; i++) {
		if ((port = hash_get(ports_hash,
				     (char *)ports_name->elements[i])) ==
		    NULL) {
			warning("%s not found!", ports_name->elements[i]);
			list_remove(ports_name, i--, NULL);
			continue;
		}
		dependencies_explode(port, ports_hash, aliases, not_founds);
	}

	if (tree) {
		for (i = 0; i < ports_name->length; i++) {
			port = hash_get(ports_hash,
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
					  ports_hash, aliases, not_founds,
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

static void dependents_list_append(list_t * self, port_t * port, hash_t *
				   ports_hash, dict_t * aliases,
				   dict_t * seen, int all)
{
	unsigned i, j;
	port_t *pport;
	hashiterator_t *iter;

	assert(port != NULL && seen != NULL && self != NULL);

	if (dict_get(seen, port->name) == NULL)
		list_append(self, port);
	else
		return;

	dict_add(seen, port->name, "");

	iter = hashiterator_new(ports_hash);
	while(hashiterator_next(iter)) {
		pport = hashiterator_get(iter);
		if (all != 1 && pport->status == PRT_NOTINSTALLED)
			continue;
		for (j = 0; j < pport->dependencies->length; j++) {
			if (strcmp(pport->dependencies->elements[j],
				   port->name) == 0) {
				dependents_list_append(self, pport, ports_hash,
						       aliases, seen, all);
			}
		}
	}
	hashiterator_free(iter);

	for (i = 0; i < aliases->length; i++) {
		if (list_get_position((list_t *) aliases->elements[i]->value,
				      strequ, port->name) < 0)
			continue;
		pport = hash_get(ports_hash, aliases->elements[i]->key);
		dependents_list_append(self, pport, ports_hash, aliases,
				       seen, all);
	}
}

list_t *dependents_list(char *port_name, hash_t * ports_hash, dict_t * aliases,
			int all, int enable_xterm_title)
{
	list_t *self;
	dict_t *seen;
	port_t *port;

	assert(port_name != NULL && ports_hash != NULL && aliases != NULL);

	if ((port = hash_get(ports_hash, port_name)) == NULL) {
		warning("%s not found!", port_name);
		return NULL;
	}

	if (enable_xterm_title)
		xterm_set_title("Calculating dependents ...");

	self = list_new();
	seen = dict_new();

	dependents_list_append(self, port, ports_hash, aliases, seen, all);

	dict_free(seen, NULL);

	return self;
}

static void dependents_tree_dump(port_t * port, hash_t * ports_hash, dict_t
				 * aliases,
				 unsigned level, dict_t * seen, int
				 verbose, int all)
{
	unsigned i, j;
	port_t *pport;
	hashiterator_t *iter;

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
	iter = hashiterator_new(ports_hash);
	while(hashiterator_next(iter)) {
		pport = hashiterator_get(iter);
		if (all != 1 && pport->status == PRT_NOTINSTALLED)
			continue;
		for (j = 0; j < pport->dependencies->length; j++) {
			if (strequ(pport->dependencies->elements[j],
				   port->name)) {
				dependents_tree_dump(pport, ports_hash, aliases,
						     level + 1, seen,
						     verbose, all);
			}
		}
	}

	for (i = 0; i < aliases->length; i++) {
		if (list_get_position((list_t *) aliases->elements[i]->value,
				      strequ, port->name) < 0)
			continue;
		pport = hash_get(ports_hash, aliases->elements[i]->key);
		dependents_tree_dump(pport, ports_hash, aliases, level,
				     seen, verbose, all);
	}
}

void dependents_list_dump(port_t * port, hash_t * ports_hash,
			  dict_t * aliases, int verbose, int all, 
			  int enable_xterm_title)
{
	list_t *detlist;

	detlist = dependents_list(port->name, ports_hash, aliases, all, 
				  enable_xterm_title);
	if (detlist == NULL)
		return;
	if (verbose)
		list_dump(detlist, deplist_verbose_dump_port);
	else
		list_dump(detlist, deplist_dump_port);
	list_free(detlist, NULL);
}

void dependents_dump(char *port_name, hash_t * ports_hash,
		     dict_t * aliases, int tree, int verbose, int all, int enable_xterm_title)
{
	port_t *port;
	dict_t *seen;
	assert(port_name != NULL && ports_hash != NULL && aliases != NULL);
	if ((port = hash_get(ports_hash, port_name)) == NULL)
		return warning("%s not found!", port_name);
	if (tree) {
		seen = dict_new();
		dependents_tree_dump(port, ports_hash, aliases, 0,
				     seen, verbose, all);
		dict_free(seen, NULL);
	} else
		dependents_list_dump(port, ports_hash, aliases, verbose, all, enable_xterm_title);
}
