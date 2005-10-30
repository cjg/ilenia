/***************************************************************************
 *            update.c
 *
 *  Thu Jul 15 16:27:56 2004
 *  Copyright  2004 - 2005  Coviello Giuseppe
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

int
cvsup (char *filename)
{
  char *args[] = { "", "-g", "-L", "1", "-r", "0", "-c", ".cvsup", filename,
    NULL
  };
  return (exec (NULL, "/usr/bin/cvsup", args));
}

int
httpup (char *filename)
{
  FILE *file;
  if (!(file = fopen (filename, "r")))
    return (EXIT_FAILURE);
  size_t n = 0;
  char *line = NULL;
  int nread;
  char *root_dir = NULL;
  char *url = NULL;
  while ((nread = getline (&line, &n, file)) > 0)
    {
      trim (line);
      if (strncmp (line, "ROOT_DIR", 8) == 0)
	root_dir = get_value (line, "ROOT_DIR");
      else if (strncmp (line, "URL", 3) == 0)
	url = get_value (line, "URL");
    }

  if (!(strlen (root_dir) && strlen (url)))
    return (EXIT_FAILURE);

  char *args[] = {
    "", "sync", url, root_dir, NULL
  };

  int status = exec (NULL, "/usr/bin/httpup", args);

  if (root_dir)
    free (root_dir);
  if (url)
    free (url);

  return (status);
}

int
cvs (char *path)
{
  char *args[] = {
    "", path, NULL
  };
  return (exec (NULL, "/etc/ports/drivers/cvs", args));
}


int
update_repo (char *name)
{
  if (getuid () != 0)
    {
      printf ("ilenia: only root can update the ports tree\n\n");
      return (-1);
    }

  FILE *file;
  char filename[20 + strlen (name)];
  if ((file = fopen (CACHE, "w")))
    {
      fclose (file);
    }

  sprintf (filename, "/etc/ports/%s.local", name);
  if (is_file (filename) == EXIT_SUCCESS)
    return (EXIT_SUCCESS);

  sprintf (filename, "/etc/ports/%s.cvsup", name);
  if (is_file (filename) == EXIT_SUCCESS)
    return (cvsup (filename));

  sprintf (filename, "/etc/ports/%s.httpup", name);
  if (is_file (filename) == EXIT_SUCCESS)
    return (httpup (filename));

  sprintf (filename, "/etc/ports/%s.cvs", name);
  if (is_file (filename) == EXIT_SUCCESS)
    return (cvs (filename));

  printf ("ilenia: %s not found\n\n", name);
  return (EXIT_FAILURE);
}

int
update_all_repos ()
{
  if (getuid () != 0)
    {
      printf ("ilenia: only root can update the ports tree\n\n");
      return (EXIT_FAILURE);
    }

  int status = 0;
  DIR *dir;
  struct dirent *info_file;
  FILE *file;
  if ((file = fopen (CACHE, "w")))
    {
      fclose (file);
    }

  dir = opendir ("/etc/ports");
  while ((info_file = readdir (dir)))
    {
      if (strstr (info_file->d_name, ".") == NULL)
	continue;
      char *name = NULL;
      name =
	mid (info_file->d_name, 0,
	     strlen (info_file->d_name) -
	     strlen (strstr (info_file->d_name, ".")));
      if (strlen (name))
	update_repo (name);
    }

  return (status);
}
