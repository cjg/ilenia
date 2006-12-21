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

int update(char *filename);

int update_repo(char *name)
{
	if (getuid() != 0)
		error("only root can update the ports tree");

	struct repolist *r = repolist_find(name, ilenia_repos);
	
	if(r==NULL) {
		error("%s not found!", name);
       		return EXIT_FAILURE;
	}
	return update(r->supfile);
}

int update(char *filename)
{
	char *extension = rindex(filename, '.');
	
	FILE *file;
	
	if ((file = fopen(CACHE, "w"))) {
		fclose(file);
	}
	
	if(!strcmp(extension, ".local"))
		return EXIT_SUCCESS;
	else if (!strcmp(extension, ".cvsup"))
		return cvsup(filename);
	else if (!strcmp(extension, ".httpup"))
		return httpup(filename);
	else if (!strcmp(extension, ".cvs"))
		return cvs(filename);
	else if (!strcmp(extension, ".rsync"))
		return rsync(filename);

	error("Driver for %s not found!", filename);
	return EXIT_FAILURE;
}

int update_all_repos()
{
	if (getuid() != 0)
		error("only root can update the ports tree!");
	struct repolist *r = ilenia_repos;
	while(r) {
		if(update(r->supfile)!=EXIT_SUCCESS)
			warning("cannot update %s!", r->name);
		r = r->next;
	}
	/*
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
	*/
	return EXIT_SUCCESS;
}
