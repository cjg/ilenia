/* job.c */

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
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "file.h"
#include "exec.h"
#include "output.h"
#include "memory.h"
#include "job.h"

#define LOG "/var/log/ilenia"

static void out_hook(char *line, void *data)
{
	fprintf(stdout, "%s", line);
	fflush(stdout);
}

static void err_hook(char *line, void *data)
{
	if (cprintf == uncoloredprintf)
		fprintf(stderr, "%s", line);
	else if (*line == '+')
		fprintf(stderr, "\033[0;34m%s\033[0;00m", line);
	else if (strcasestr(line, "warning") != NULL)
		fprintf(stderr, "\033[0;33m%s\033[0;00m", line);
	else
		fprintf(stderr, "\033[0;31m%s\033[0;00m", line);
	fflush(stderr);
}

static void do_log(const char *format, ...)
{
	FILE *stream;
	va_list ap;

	assert (format != NULL);

	if (is_file(LOG))
		stream = fopen(LOG, "a");
	else
		stream = fopen(LOG, "w");

	if (stream == NULL) {
		warning("Cannot open or create the log file!");
		return;
	}

	time_t t = time(NULL);
	char *times = ctime(&t);
	*(times + strlen(times) - 1) = 0;
	va_start (ap, format);
	fprintf (stream, "%s: ", times);
	vfprintf(stream, format, ap);
	fprintf (stream, "\n");
	va_end (ap);

	fclose(stream);
}

static int job_update_execute(job_t * self)
{

	int ret;

	assert(self);

	char *args[] = { "pkgmk", "-d", "-f",
		self->port->status == PRT_NOTINSTALLED ? "-i" : "-u",
		self->port->pkgmk_conf != NULL ? "-cf" : "",
		self->port->pkgmk_conf != NULL ? self->port->pkgmk_conf : "",
		NULL
	};
	char *preinstall_args[] = { "sh", "pre-install", NULL };
	char *postinstall_args[] = { "sh", "post-install", NULL };

	self->result = FAILED_RT;

	chdir(self->port->repository->path);
	chdir(self->port->name);

	if (self->have_preinstall) {
		cprintf(stdout, "[CYAN]==> Executing pre-install script "
			"...[DEFAULT]\n");
		ret =
		    exechook2("/bin/sh", preinstall_args, out_hook, NULL,
			      err_hook, NULL);
		if (ret) {
			self->result = FAILED_RT;
			return ret;
		}
	}

	ret = exechook2("/usr/bin/pkgmk", args, out_hook, NULL, err_hook, NULL);
	if (ret) {
		self->result = FAILED_RT;
		return ret;
	}

	if (self->have_postinstall) {
		cprintf(stdout, "[CYAN]==> Executing post-install script "
			"...[DEFAULT]\n");

		ret =
		    exechook2("/bin/sh", postinstall_args, out_hook, NULL,
			      err_hook, NULL);
		if (ret) {
			self->result = FAILED_RT;
			return ret;
		}
	}

	if (self->enable_log)
		do_log("%s %s-%s", 
		       self->port->status == PRT_NOTINSTALLED ? "Installed" : 
		       "Updated", self->port->name, self->port->version);

	if (self->post_pkgadd != NULL) {
		cprintf(stdout, "[CYAN]==> Executing post pkgadd command(s) "
			"...[DEFAULT]\n");

		char *post_pkgadd_args[] = { "sh", "-c", self->post_pkgadd,
			NULL
		};

		ret =
		    exechook2("/bin/sh", post_pkgadd_args, out_hook, NULL,
			      err_hook, NULL);

	}

	self->result = SUCCESSED_RT;

	return 0;
}

static int job_fetch_execute(job_t * self)
{

	int ret;

	assert(self != NULL);

	char *args[] = { "pkgmk", "-do", NULL };

	self->result = FAILED_RT;

	chdir(self->port->repository->path);
	chdir(self->port->name);

	ret = exechook2("/usr/bin/pkgmk", args, out_hook, NULL, err_hook, NULL);
	if (ret) {
		self->result = FAILED_RT;
		return ret;
	}

	self->result = SUCCESSED_RT;

	return 0;
}

static int job_remove_execute(job_t * self)
{

	int ret;

	assert(self);

	char *args[] = { "pkgrm", self->port->name, NULL };

	self->result = FAILED_RT;

	ret = exechook2("/usr/bin/pkgrm", args, out_hook, NULL, err_hook, NULL);

	if (ret) {
		self->result = FAILED_RT;
		return ret;
	}

	self->result = SUCCESSED_RT;

	if (self->enable_log)
		do_log("Removed %s-%s", self->port->name, self->port->version);

	return 0;
}

job_t *job_new(port_t * port, job_type_t type, char *post_pkgadd, int enable_log)
{
	job_t *self;
	char *path;

	assert(port != NULL);

	self = xmalloc(sizeof(job_t));
	self->port = port;
	self->type = type;
	self->result = NOT_EXECUTED_RT;

	if (type == REMOVE_JOB)
		return self;

	self->post_pkgadd = post_pkgadd;

	path =
	    xstrdup_printf("%s/%s/README", port->repository->path, port->name);
	self->have_readme = is_file(path);
	free(path);
	path =
	    xstrdup_printf("%s/%s/pre-install", port->repository->path,
			   port->name);
	self->have_preinstall = is_file(path);
	free(path);
	path = xstrdup_printf("%s/%s/post-install", port->repository->path,
			      port->name);

	self->have_postinstall = is_file(path);
	free(path);

	self->enable_log = enable_log;

	return self;
}

void job_dump(job_t * self)
{
	assert(self != NULL);
	printf("%s\n", self->port->name);
}

int job_execute(job_t * self)
{
	assert(self);

	switch (self->type) {
	case UPDATE_JOB:
		return job_update_execute(self);
		break;
	case FETCH_JOB:
		return job_fetch_execute(self);
		break;
	case REMOVE_JOB:
		return job_remove_execute(self);
		break;
	}

	return 0;
}
