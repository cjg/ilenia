/***************************************************************************
 *            update.c
 *
 *  Thu Jul 15 16:27:56 2004
 *  Copyright  2004 - 2006  Coviello Giuseppe
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "utils.h"
#include "manipulator.h"
#include "repolist.h"
#include "ilenia.h"
#include "output.h"

int cvsup(char *filename)
{
	char *args[] =
	    { "", "-g", "-L", "1", "-r", "0", "-c", ".cvsup", filename,
		NULL
	};
	return (exec("/etc/ports", "/usr/bin/cvsup", args));
}

int httpup(char *filename)
{
	char *args[] = {
		"", filename, NULL
	};
	return (exec("/etc/ports", "/etc/ports/drivers/httpup", args));
}

int cvs(char *filename)
{
	char *args[] = {
		"", filename, NULL
	};
	return (exec("/etc/ports", "/etc/ports/drivers/cvs", args));
}

int rsync(char *filename)
{
	char *args[] = { "", filename, NULL };
	return (exec("/etc/ports", "/etc/ports/drivers/rsync", args));
}


int update_repo(char *name)
{
	if (getuid() != 0)
		error("only root can update the ports tree");

	FILE *file;
	char *filename = NULL;
	if ((file = fopen(CACHE, "w"))) {
		fclose(file);
	}

	strprintf(&filename, "%s.local", name);
	if (is_file("/etc/ports/", filename) == EXIT_SUCCESS)
		return (EXIT_SUCCESS);

	strprintf(&filename, "%s.cvsup", name);
	if (is_file("/etc/ports/", filename))
		return (cvsup(filename));

	strprintf(&filename, "%s.httpup", name);
	if (is_file("/etc/ports/", filename))
		return (httpup(filename));

	strprintf(&filename, "%s.cvs", name);
	if (is_file("/etc/ports/", filename))
		return (cvs(filename));

	strprintf(&filename, "%s.rsync", name);
	if (is_file("/etc/ports/", filename))
		return (rsync(filename));

	error("%s not found!", name);
	return EXIT_FAILURE;
}

int update_all_repos()
{
	if (getuid() != 0)
		error("only root can update the ports tree!");

	int status = 0;
	DIR *dir;
	struct dirent *info_file;
	FILE *file;
	if ((file = fopen(CACHE, "w"))) {
		fclose(file);
	}

	dir = opendir("/etc/ports");
	while ((info_file = readdir(dir))) {
		if (strstr(info_file->d_name, ".") == NULL)
			continue;
		char *name = NULL;
		name =
		    mid(info_file->d_name, 0,
			strlen(info_file->d_name) -
			strlen(strstr(info_file->d_name, ".")));
		if (strlen(name))
			update_repo(name);
	}

	return status;
}
