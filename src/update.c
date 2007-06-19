/* update.c */

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
#include <stdarg.h>
#include <string.h>
#include "job.h"
#include "output.h"
#include "update.h"
#include "dependencies.h"
#include "str.h"

int ask_yn(char *s, ...)
{
	char *line = NULL;
	size_t n;
	va_list ap;
	va_start(ap, s);
	vprintf(s, ap);
	va_end(ap);
	printf(" [y/N] ");
	fflush(stdout);
	getline(&line, &n, stdin);
	if (n > 0 && strcasecmp(line, "y\n") == 0) {
		free(line);
		return 1;
	}
	free(line);
	return 0;
}

static int update_manage_not_founds(dict_t * not_founds, int not_found_policy)
{
	unsigned i;
	port_t *port;

	if (not_founds->length == 0)
		return 0;

	if (not_found_policy == ASK) {
		for (i = 0; i < not_founds->length; i++) {
			port = (port_t *) not_founds->elements[i]->value;
			if (ask_yn("%s not found, continue?", port->name) == 0)
				return 1;
		}
		return 0;
	}

	for (i = 0; i < not_founds->length; i++) {
		port = (port_t *) not_founds->elements[i]->value;
		warning("%s not found!", port->name);
	}

	if (not_found_policy == NEVERMIND)
		return 0;

	return 1;
}

static int update_manage_ask(list_t * jobs, int ask_for_update)
{
	unsigned i;

	assert(jobs != NULL);

	printf("Packages to update:\n");
	for (i = 0; i < jobs->length; i++) {
		if (((job_t *) jobs->elements[i])->port->status == PRT_OUTDATED)
			port_dump(((job_t *) jobs->elements[i])->port);
	}

	printf("\nPackages to install:\n");
	for (i = 0; i < jobs->length; i++) {
		if (((job_t *) jobs->elements[i])->port->status ==
		    PRT_NOTINSTALLED)
			port_dump(((job_t *) jobs->elements[i])->port);
	}

	if (ask_for_update && !ask_yn("Confirm update?"))
		return 1;

	return 0;
}

static void dump_report(list_t * jobs)
{
	unsigned i;
	job_t *job;

	assert(jobs != NULL);

	printf("\n\n--------\n");
	printf("Packages installed: ");
	for (i = 0; i < jobs->length; i++) {
		job = jobs->elements[i];
		if (job->port->status == PRT_NOTINSTALLED &&
		    job->result == SUCCESSED_RT)
			printf("%s ", job->port->name);
	}
	printf("\n");

	printf("Packages updated: ");
	for (i = 0; i < jobs->length; i++) {
		job = jobs->elements[i];
		if (job->port->status != PRT_NOTINSTALLED &&
		    job->result == SUCCESSED_RT)
			printf("%s ", job->port->name);
	}
	printf("\n");

	printf("Packages not installed or updated: ");
	for (i = 0; i < jobs->length; i++) {
		job = jobs->elements[i];
		if (job->result == NOT_EXECUTED_RT)
			printf("%s ", job->port->name);
	}
	printf("\n");

	printf("Packages failed to install or update: ");
	for (i = 0; i < jobs->length; i++) {
		job = jobs->elements[i];
		if (job->result == FAILED_RT)
			printf("%s ", job->port->name);
	}
	printf("\n");

	printf("Ports with pre-install: ");
	for (i = 0; i < jobs->length; i++) {
		job = jobs->elements[i];
		if (job->have_preinstall)
			printf("%s ", job->port->name);
	}
	printf("\n");

	printf("Ports with post-install: ");
	for (i = 0; i < jobs->length; i++) {
		job = jobs->elements[i];
		if (job->have_postinstall)
			printf("%s ", job->port->name);
	}
	printf("\n");

	printf("Ports with README: ");
	for (i = 0; i < jobs->length; i++) {
		job = jobs->elements[i];
		if (job->have_readme)
			printf("%s ", job->port->name);
	}
	printf("\n");
}

int update_system(dict_t * ports, int fetch_only, conf_t *conf)
{
	unsigned i, j;
	int ret;
	port_t *port, *pport;
	list_t *jobs;
	list_t *dependencies;
	dict_t *seen, *not_founds;
	job_t *job;

	assert(ports != NULL && conf != NULL);

	jobs = list_new();
	seen = dict_new();
	not_founds = dict_new();

	for (i = 0; i < ports->length; i++) {
		port = ports->elements[i]->value;
		if (port->status != PRT_OUTDATED)
			continue;

		dependencies = list_new();
		dependencies_list(dependencies, port->name, ports, 
				  conf->aliases, not_founds, 
				  conf->enable_xterm_title);
		for (j = 0; dependencies != NULL && j < dependencies->length;
		     j++) {
			pport = dependencies->elements[j];
			if ((pport->status == PRT_OUTDATED || pport->status ==
			     PRT_NOTINSTALLED)
			    && dict_get(seen, pport->name) == NULL) {
				dict_add(seen, pport->name, "");
				if (pport->repository != NULL)
					list_append(jobs, job_new(pport,
								  fetch_only ?
								  FETCH_JOB :
								  UPDATE_JOB,
								  conf->post_pkgadd,
								  conf->enable_log));
			}
		}

		list_free(dependencies, NULL);
	}

	if (update_manage_not_founds(not_founds, conf->not_found_policy)) {
		dict_free(not_founds, port_free);
		dict_free(seen, NULL);
		list_free(jobs, NULL);
		return 2;
	}

	dict_free(not_founds, port_free);
	dict_free(seen, NULL);
	if (conf->enable_xterm_title)
		xterm_set_title("Enable update?");
	if (jobs->length && update_manage_ask(jobs, conf->ask_for_update)) {
		list_free(jobs, free);
		return 0;
	}

	for (ret = 0, i = 0; ret == 0 && i < jobs->length; i++) {
		job = jobs->elements[i];
		if (conf->enable_xterm_title)
			xterm_set_title("%s %s (%d on %d) ...",
					job->port->status ==
					PRT_NOTINSTALLED ? "Installing" :
					"Updating",
					job->port->name, i + 1, jobs->length);
		cprintf(stdout, "[CYAN]==> %s %s (%d on %d)[DEFAULT]\n",
			job->port->status ==
			PRT_NOTINSTALLED ? "Installing" : "Updating",
			job->port->name, i + 1, jobs->length);
		ret = job_execute(jobs->elements[i]);
	}

	dump_report(jobs);

	if (ret == 0)
		printf("The System is Up to date\n");

	list_free(jobs, free);

	return ret;
}

int update_package(list_t * ports_name, dict_t * ports, int fetch_only, 
		   conf_t * conf, int just_install)
{
	port_t *port;
	list_t *dependencies;
	list_t *jobs;
	dict_t *not_founds;
	job_t *job;
	unsigned i;
	int ret;

	assert(ports_name != NULL && ports != NULL && conf != NULL);

	not_founds = dict_new();
	dependencies = list_new();
	for (i = 0; i < ports_name->length; i++) {
		if ((port = dict_get(ports, ports_name->elements[i])) == NULL ||
		    port->repository == NULL) {
			error("%s not found!", ports_name->elements[i]);
			dict_free(not_founds, port_free);
			list_free(dependencies, NULL);
			return 1;
		}
		dependencies_list(dependencies, ports_name->elements[i], ports,
				  conf->aliases, not_founds, 
				  conf->enable_xterm_title);
		if (port->status != PRT_NOTINSTALLED)
			port->status = PRT_OUTDATED;
	}

	if (update_manage_not_founds(not_founds, conf->not_found_policy)) {
		dict_free(not_founds, port_free);
		list_free(dependencies, NULL);
		return 2;
	}

	jobs = list_new();
	for (i = 0; i < dependencies->length; i++) {
		port = dependencies->elements[i];
		if ((port->status != PRT_INSTALLED && port->status != PRT_DIFF)
		    && (port->status != PRT_OUTDATED || !just_install 
			|| list_get_position(ports_name, strequ, port->name) != -1)
		    && port->repository != NULL)
			list_append(jobs, job_new(port, fetch_only ? FETCH_JOB :
						  UPDATE_JOB,
						  conf->post_pkgadd,
					    conf->enable_log));
	}

	dict_free(not_founds, port_free);
	list_free(dependencies, NULL);
	if (conf->enable_xterm_title)
		xterm_set_title("Enable update?");
	if (update_manage_ask(jobs, 1)) {
		list_free(jobs, free);
		return 0;
	}

	for (ret = 0, i = 0; ret == 0 && i < jobs->length; i++) {
		job = jobs->elements[i];
		if (conf->enable_xterm_title)
			xterm_set_title("%s %s (%d on %d) ...",
					job->port->status ==
					PRT_NOTINSTALLED ? "Installing" :
					"Updating",
					job->port->name, i + 1, jobs->length);
		cprintf(stdout, "[CYAN]==> %s %s (%d on %d)[DEFAULT]\n",
			job->port->status ==
			PRT_NOTINSTALLED ? "Installing" : "Updating",
			job->port->name, i + 1, jobs->length);
		ret = job_execute(jobs->elements[i]);
	}

	dump_report(jobs);

	list_free(jobs, free);

	return 0;
}
