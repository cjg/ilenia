/* remove.c */

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
#include "output.h"
#include "remove.h"
#include "job.h"
#include "str.h"

int ask_yn(char *s, ...);

static void dump_report(list_t * jobs)
{
	unsigned i;
	job_t *job;

	assert(jobs != NULL);

	printf("\n\n--------\n");
	printf("Packages removed: ");
	for (i = 0; i < jobs->length; i++) {
		job = jobs->elements[i];
		if (job->result == SUCCESSED_RT)
			printf("%s ", job->port->name);
	}
	printf("\n");

	printf("Packages not removed: ");
	for (i = 0; i < jobs->length; i++) {
		job = jobs->elements[i];
		if (job->result == NOT_EXECUTED_RT)
			printf("%s ", job->port->name);
	}
	printf("\n");

	printf("Packages failed to remove: ");
	for (i = 0; i < jobs->length; i++) {
		job = jobs->elements[i];
		if (job->result == FAILED_RT)
			printf("%s ", job->port->name);
	}
	printf("\n");
}

int remove_packages(list_t * packages_name, list_t * packages, dict_t * ports,
		    conf_t * conf, int all)
{
	unsigned i, j;
	int ret;
	int position;
	list_t *jobs, *dependents;
	dict_t *seen;
	job_t *job;

	assert(packages_name != NULL && packages != NULL && ports != NULL &&
	       conf != NULL);

	jobs = list_new();
	seen = dict_new();

	for (i = 0; i < packages_name->length; i++) {
		if (dict_get(seen, packages_name->elements[i]) != NULL)
			continue;
		dict_add(seen, packages_name->elements[i], "");

		if ((position = list_get_position(packages, port_query_by_name,
						  packages_name->elements[i])) <
		    0) {
			warning("the package %s is not installed!",
				packages_name->elements[i]);
			continue;
		}

		list_append(jobs, job_new(packages->elements[position],
					  REMOVE_JOB, ""));

		if (!all)
			continue;

		dependents = dependents_list(packages_name->elements[i],
					     ports, conf->aliases, 0);

		for (j = 1; dependents != NULL && j < dependents->length; j++) {
			if (dict_get
			    (seen,
			     ((port_t *) dependents->elements[j])->name) !=
			    NULL)
				continue;
			dict_add(seen,
				 ((port_t *) dependents->elements[j])->name,
				 "");

			list_append(jobs, job_new(dependents->elements[j],
						  REMOVE_JOB, ""));
		}

		list_free(dependents, NULL);
	}

	dict_free(seen, NULL);

	if (jobs->length == 0) {
		list_free(jobs, free);
		return 0;
	}

	printf("Packages to remove:\n");
	for (i = 0; i < jobs->length; i++)
		printf("%s\n", ((job_t *) jobs->elements[i])->port->name);
	if (!all)
		warning("some other packages may depend on this ones; run "
			"'ilenia -T' to see a list of them!");
	if (!ask_yn("Are you sure to remove this packages?")) {
		list_free(jobs, free);
		return 0;
	}

	for (ret = 0, i = 0; ret == 0 && i < jobs->length; i++) {
		job = jobs->elements[i];
		cprintf(stdout, "[CYAN]==> Removing %s (%d on %d)[DEFAULT]\n",
			job->port->name, i + 1, jobs->length);
		ret = job_execute(jobs->elements[i]);
	}

	dump_report(jobs);

	list_free(jobs, free);

	return ret;
}
