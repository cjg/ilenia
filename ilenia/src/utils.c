/***************************************************************************
 *            utils.c
 *
 *  Wed Oct 26 12:58:00 2005
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

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int exec(char *wdir, char *command, char *args[])
{
	int status;
	pid_t pid = fork();
	if (pid == 0) {
		if (wdir)
			chdir(wdir);
		execvp(command, args);
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		status = EXIT_FAILURE;
	} else {
		while ((waitpid(pid, &status, 0) == 0)) {
		}
	}
	return (status);
}

int is_file(char *path, char *name)
{
	int file = 0;
	char filepath[strlen(path) + strlen(name) + 1];
	strcpy(filepath, path);
	strcat(filepath, "/");
	strcat(filepath, name);
	file = open(filepath, O_RDONLY);
	if (file == -1)
		return (EXIT_FAILURE);
	close(file);
	return (EXIT_SUCCESS);
}
