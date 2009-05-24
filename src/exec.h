/* exec.h */

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

#ifndef _EXEC_H
#define _EXEC_H

pid_t execpipe(char *command, char *args[], int pipefd[3]);
int exechook2(char *command, char *args[],
	      void (*out_hook) (char *line, void *data), void *out_data,
	      void (*err_hook) (char *line, void *data), void *err_data);
int exechook(char *command, char *args[],
	     void (*exec_hook) (char *line, void *data), void *data);
int execlogp2(char *command, char *args[], char **out_log, char **err_log);
int execlogp(char *command, char *args[], char **log);
int execlog2(char *command, char *args[], char **out_log, char **err_log);
int execlog(char *command, char *args[], char **log);
int exec(char *command, char *args[]);

#endif
