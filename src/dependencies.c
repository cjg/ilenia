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

static void deptree_data_dump(port_t * port, hash_t *ports, unsigned level,
			      dict_t * seen)
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
	for (i = 0; i < port->dependencies->length; i++) {
		port_t *p = (port_t *) hash_get(ports, 
						list_get(port->dependencies,
							 i));
		deptree_data_dump(p, ports, level, seen);
	}
}

static void deptree_verbose_data_dump(port_t * port, hash_t *ports, 
				      unsigned level, dict_t *seen)
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
	for (i = 0; i < port->dependencies->length; i++) {
		port_t *p = (port_t *) hash_get(ports, 
						list_get(port->dependencies,
							 i));
		deptree_verbose_data_dump(p, ports, level, seen);
	}
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

static port_t *port_new_not_found(char *name)
{
	port_t *self = port_new(xstrdup(name), xstrdup(""), NULL, list_new(),
				NULL);
	self->status = PRT_NOTINSTALLED;
	return self;
}

/* = build recursively the adjacencies matrix representing the graph of
   dependencies of the port = */
static void build_adj_matrix(dict_t *G, port_t *port, hash_t *ports, 
			 dict_t *aliases,  dict_t *not_founds)
{
	/* == checking if the port went already inserted == */
	if(dict_get(G, port->name) != NULL)
		return;

	/* == add the port to the matrix == */
	list_t *adjacencies = list_new();
	dict_add(G, port->name, adjacencies);

	/* == fill the adjiacencies == */
	unsigned i;
	for(i = 0; i < port->dependencies->length; i++) {
		char *name = list_get(port->dependencies, i);

		/* === checking if the port is found in ports tree === */
		port_t *p = hash_get(ports, name);
		if(p == NULL) {
			p = port_new_not_found(name);
			dict_add(not_founds, name, p);
		}

		/* === substuting the port with an honourable alias === */
		p = port_alias(p, ports, aliases);

		/* === add the dependencies to the matrix === */
		list_append(adjacencies, p->name);
		build_adj_matrix(G, p, ports, aliases, not_founds);
	}
}

/* = allocate and initialize a dynamic int to be used in list_t and dict_t
   structures = */
int *int_new(int value)
{
	int *self = xmalloc(sizeof(int));
	*self = value;
	return self;
}

static char *black = "black";
static char *gray = "gray";
static char *white = "white";

/* = provide a dsf on the graph G starting from the node u, it's a subroutine
   for the topological sort algorithm = */
static void topological_dsf(dict_t *G, list_t *L, char *u, dict_t *color, 
		    dict_t *f, int *time)
{
	dict_add(color, u, gray);
	*time = *time + 1; 
	list_t *adj = dict_get(G, u);
	unsigned i;
	for(i = 0; i < adj->length; i++) {
		char *v = list_get(adj, i);
		if(dict_get(color, v) == white)
			topological_dsf(G, L, v, color, f, time);
	}
	dict_add(color, u, black);
	*time = *time + 1;
	dict_add(f, u, int_new(*time));
	list_append(L, u);
}

/* = after applying the topological sort on G a vector, f, containing the time
   of visit of each node; in a dag for each oriented edge (u, v) we have that 
   f[v] < f[u], so this property is here used to detect and find cycles = */
static void search_cycle(dict_t *G, dict_t *f, dict_t *cycles)
{
	list_t *V = dict_get_keys(G);
	unsigned i, j;
	for(i = 0; i < V->length; i++) {
		char *u = list_get(V, i);
		list_t *adj = dict_get(G, u);
		for(j = 0; j < adj->length; j++) {
			char *v = list_get(adj, j);
			if(*((int *)dict_get(f, v))
			   >= *((int *) dict_get(f, u))) {
				dict_add(cycles, u, xstrdup(v));
			}
		}
	}
}

/* = the topological sort algorithm is used to obtain a sorted list of
   dependencies from the dependencies graph = */
static void topological_sort(dict_t *G, list_t *L, dict_t *C)
{
	list_t *V = dict_get_keys(G);
	unsigned i;
	dict_t *color = dict_new();
	dict_t *f = dict_new();
	int time = 0;

	for(i = 0; i < V->length; i++) {
		dict_add(color, list_get(V, i), white);
		dict_add(f, list_get(V, i), int_new(0
));
	}
	
	for(i = 0; i < V->length; i++) {
		char *u = list_get(V, i);
		if(dict_get(color, u) == white)
			topological_dsf(G, L, u, color, f, &time);
	}

	search_cycle(G, f, C);
	dict_free(color, NULL);
	dict_free(f, free);
}

static void manage_cycles(dict_t *G, dict_t *C, hash_t *ports)
{
	if(C->length == 0)
		return;
	char *u = C->elements[0]->key;
	char *v = C->elements[0]->value;
	port_t *p = hash_get(ports, v);

	if(p->status != PRT_NOTINSTALLED) {
		/* remove the v dependence from u and retry with
		   dependencies_list */
		warning("Breackable dependencies cycle found: %s <-> %s. "
			"Removing the %s dependence from %s.", u, v, v, u);
		list_t *adj = dict_get(G, u);
		list_remove(adj, list_get_position(adj, strequ, v), NULL);
		return;
	}

	/* giving up if u and v are the same */
	if(strcmp(u, v) == 0)
		return;

	/* adds to all ports that dependes from v the dependencies of v, then
	   remove all dependencies to v and retry with dependencies_list */
	warning("Breackable dependencies cycle found: %s <-> %s. "
		"Replacing all accurences of %s with its dependencies.",
		u, v, u);
	list_t *v_adj = dict_get(G, v);
	list_t *G_keys = dict_get_keys(G);
	unsigned j;
	for(j = 0; j < G_keys->length; j++) {
		char *z = list_get(G_keys, j);
		list_t *adj = dict_get(G, z);
		if(list_get_position(adj, strequ, v) < 0)
			continue;
		list_cat(adj, v_adj, NULL);
	}
	list_free(v_adj, NULL);
	dict_add(G, v, list_new());
	list_free(G_keys, NULL);
}

list_t *dependencies_list(list_t * self, char *port_name, hash_t * ports_hash,
			  dict_t * aliases, dict_t * not_founds, int
			  enable_xterm_title)
{
	port_t *port;

	assert(port_name != NULL && ports_hash != NULL && aliases != NULL &&
	       self != NULL);

	if ((port = hash_get(ports_hash, port_name)) == NULL) {
		warning("%s not found!", port_name);
		return NULL;
	}

	dict_t *G = dict_new();
	build_adj_matrix(G, port, ports_hash, aliases, not_founds);

	list_t *L = NULL;;
	dict_t *C = NULL;
	char *previous_u = xstrdup("");
	char *previous_v = xstrdup("");
	
	do {
		if(C != NULL && C->length > 0) {
			free(previous_u);
			free(previous_v);
			previous_u = xstrdup(C->elements[0]->key);
			previous_v = xstrdup(C->elements[0]->value);
		}
		if(L != NULL)
			list_free(L, NULL);
		if(C != NULL)
			dict_free(C, free);
		L = list_new();
		C = dict_new();
		topological_sort(G, L, C);
		manage_cycles(G, C, ports_hash);
	} while(C->length > 0 
		&& (strcmp(previous_u, C->elements[0]->key) != 0
		    || strcmp(previous_v, C->elements[0]->value) != 0));

	if(C->length != 0) {
		/* unbreackable cycles found, giving up */
		unsigned i;
		for(i = 0; i < C->length; i++)
			error("Unbreackable dependencies cycle found: "
			      "%s <-> %s.", C->elements[i]->key,
			      (char *) C->elements[i]->value);
		list_free(L, NULL);
		dict_free(C, free);
		return NULL;
	}

	unsigned i;
	for(i = 0; i < L->length; i++) {
		port_t *p = hash_get(ports_hash, (char *) list_get(L, i));
		if(p == NULL) 
			p = dict_get(not_founds, (char *) list_get(L, i));
		if(list_get_position(self, port_equ, p) >= 0)
			continue;
		list_append(self, p);
	}

	list_free(L, NULL);
	dict_free(C, free);

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
	}

	if (tree) {
		for (i = 0; i < ports_name->length; i++) {
			port = hash_get(ports_hash,
					(char *)ports_name->elements[i]);
			seen = dict_new();
			if (verbose)
				deptree_verbose_data_dump(port, ports_hash, 
							  0, seen);
			else
				deptree_data_dump(port, ports_hash, 0, seen);
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
