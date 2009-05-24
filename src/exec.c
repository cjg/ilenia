/* exec.c */

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

#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include "memory.h"

// TODO: split from ilenia 

typedef struct {
	int fd;
	void (*hook) (char *line, void *data);
	void *data;
} hookmngr_t;

static void *hookmngr(void *args)
{
	hookmngr_t *hookmngr_args;
	char *line;
	size_t n;
	int nread = 0;
	FILE *stream;

	hookmngr_args = (hookmngr_t *) args;

	stream = fdopen(hookmngr_args->fd, "r");

	line = NULL;

	while ((nread = getline(&line, &n, stream)) >= 0)
		hookmngr_args->hook(line, hookmngr_args->data);

	fclose(stream);
	close(hookmngr_args->fd);

	free(line);

	return NULL;
}

static void stderr_hook(char *line, void *data)
{
	fprintf(stderr, line);
	fflush(stderr);
}

static void stdout_hook(char *line, void *data)
{
	fprintf(stdout, line);
	fflush(stdout);
}

static void store_hook(char *line, void *data)
{
	char **output = (char **)data;
	static int storing = 0;
	while (storing) ;
	storing = 1;
	*output = (char *)xrealloc(*output, strlen(*output) + strlen(line) + 1);
	memcpy(*output + strlen(*output), line, strlen(line) + 1);
	storing = 0;
}

static void storeprint_hook(char *line, void *data)
{
	store_hook(line, data);
	printf(line);
}

pid_t execpipe(char *command, char *args[], int pipefd[3])
{
	pid_t pid;
	int infd[2], outfd[2], errfd[2];
	pipe(infd);
	pipe(outfd);
	pipe(errfd);

	pid = fork();
	if (pid == 0) {
		close(infd[1]);
		close(outfd[0]);
		close(errfd[0]);
		dup2(infd[0], STDIN_FILENO);
		dup2(outfd[1], STDOUT_FILENO);
		dup2(errfd[1], STDERR_FILENO);
		execvp(command, args);
		exit(1);
	} else if (pid < 0) {
		return -1;
	}

	close(infd[0]);
	close(outfd[1]);
	close(errfd[1]);

	pipefd[0] = infd[1];
	pipefd[1] = outfd[0];
	pipefd[2] = errfd[0];

	return pid;
}

int
exechook2(char *command, char *args[],
	  void (*out_hook) (char *line, void *data), void *out_data,
	  void (*err_hook) (char *line, void *data), void *err_data)
{
	int pipefd[3];
	pid_t pid;
	int status;
	hookmngr_t *out;
	hookmngr_t *err;

	pthread_t outid;
	pthread_t errid;

	pid = execpipe(command, args, pipefd);

	if (pid == -1)
		return -1;

	close(pipefd[0]);

	out = xmalloc(sizeof(hookmngr_t));
	out->fd = pipefd[1];
	out->hook = out_hook;
	out->data = out_data;

	err = xmalloc(sizeof(hookmngr_t));
	err->fd = pipefd[2];
	err->hook = err_hook;
	err->data = err_data;

	pthread_create(&outid, NULL, &hookmngr, out);
	pthread_create(&errid, NULL, &hookmngr, err);

	pthread_join(outid, NULL);
	pthread_join(errid, NULL);
	waitpid(pid, &status, 0);

	free(out);
	free(err);

	return status == 0 ? 0 : status - 255;
}

int
exechook(char *command, char *args[],
	 void (*exec_hook) (char *line, void *data), void *data)
{
	return exechook2(command, args, exec_hook, data, stderr_hook, NULL);
}

int execlogp2(char *command, char *args[], char **out_log, char **err_log)
{
	*out_log = xstrdup("");
	*err_log = xstrdup("");
	return exechook2(command, args, storeprint_hook, out_log,
			 storeprint_hook, err_log);
}

int execlogp(char *command, char *args[], char **log)
{
	*log = xstrdup("");
	return exechook2(command, args, store_hook, log, storeprint_hook, log);
}

int execlog2(char *command, char *args[], char **out_log, char **err_log)
{
	*out_log = xstrdup("");
	*err_log = xstrdup("");
	return exechook2(command, args, store_hook, out_log, store_hook,
			 err_log);
}

int execlog(char *command, char *args[], char **log)
{
	**log = 0;
	return exechook2(command, args, store_hook, log, store_hook, log);
}

int exec(char *command, char *args[])
{
	return exechook2(command, args, stdout_hook, NULL, stderr_hook, NULL);
}
