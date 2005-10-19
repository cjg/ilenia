/***************************************************************************
 *            lsports.c
 *
 *  Sat Jul 10 12:57:55 2004
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
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include "manipola.h"
#include "pkglist.h"
#include "deplist.h"
#include "repolist.h"
#include "aliaslist.h"
#include "ilenia.h"

FILE *cachefile;

#define PORTS_LOCATION "/usr/ports"

struct repolist *
parsa_cvsup (char *percorso, struct repolist *p)
{
  FILE *file;
  if ((file = fopen (percorso, "r")))
    {
      char riga[255];
      char prefix[255];
      char mad_prefix[255] = "";
      char collezione[255];
      while (fgets (riga, 255, file))
	{
	  char *value = "";
	  strcpy (riga, trim (riga));
	  if (riga[0] != '#')
	    {
	      if (riga[0] == '*')
		{
		  if (strlen (value = get_value (riga, "*default prefix")))
		    {
		      strcpy (prefix, value);
		      if (strncmp
			  (prefix, PORTS_LOCATION,
			   strlen (PORTS_LOCATION)) == 0
			  && strlen (prefix) > strlen (PORTS_LOCATION))
			{
			  strcpy (mad_prefix,
				  mid (prefix, strlen (PORTS_LOCATION) + 1,
				       FINE));
			  strcpy (mad_prefix, trim (mad_prefix));
			  if (mad_prefix[strlen (mad_prefix) - 1] != '/')
			    strcat (mad_prefix, "/");
			  strcpy (prefix, PORTS_LOCATION);
			}

		    }
		}
	      else if (strlen (riga) > 0)
		{
		  if (strlen (mad_prefix) > 0)
		    {
		      strcpy (collezione, mad_prefix);
		      strcat (collezione, riga);
		    }
		  else
		    strcpy (collezione, riga);
		  strcpy (collezione, strtok (collezione, " "));
		  p = repolist_add (collezione, prefix, p);
		}
	    }
	}
    }
  return (p);
}

struct repolist *
parsa_httpup (char *percorso, struct repolist *p)
{
  FILE *file;
  if ((file = fopen (percorso, "r")))
    {
      char riga[255];
      while (fgets (riga, 255, file))
	{
	  strcpy (riga, trim (riga));
	  char *value = "";
	  char *collezione = "";
	  char *prefix = "";
	  char *mad_prefix = "";
	  if (riga[0] != '#')
	    {
	      if (strlen (value = get_value (riga, "ROOT_DIR")) > 0)
		{
		  collezione = strdup (value);
		  collezione = strdup (rindex (collezione, '/'));
		  collezione = strdup (mid (collezione, 1, FINE));
		  prefix = strdup (value);
		  prefix =
		    mid (prefix, 0, strlen (prefix) - strlen (collezione));
		  if (strncmp
		      (prefix, PORTS_LOCATION, strlen (PORTS_LOCATION)) == 0
		      && strlen (prefix) > strlen (PORTS_LOCATION) + 1)
		    {
		      mad_prefix =
			mid (prefix, strlen (PORTS_LOCATION) + 1, FINE);
		      if (mad_prefix[strlen (mad_prefix) - 1] != '/')
			mad_prefix = strcat (mad_prefix, "/");
		      prefix = strdup (PORTS_LOCATION);
		      collezione = strcat (mad_prefix, collezione);
		    }
		  p = repolist_add (collezione, prefix, p);
		}
	    }
	}
    }
  return (p);
}

struct repolist *
parse_cvs (char *percorso, struct repolist *p)
{
  FILE *file;
  if ((file = fopen (percorso, "r")))
    {
      char riga[255];
      char *prefix = "";
      char *mad_prefix = "";
      char *collezione = "";
      char *value = "";
      while (fgets (riga, 255, file))
	{
	  strcpy (riga, trim (riga));
	  if (riga[0] != '#')
	    {
	      if (strncmp (riga, "LOCAL_PATH", 10) == 0)
		{
		  if (strlen (value = get_value (riga, "LOCAL_PATH")))
		    {
		      prefix = strdup (value);
		      if (strncmp
			  (prefix, PORTS_LOCATION,
			   strlen (PORTS_LOCATION)) == 0
			  && strlen (prefix) > strlen (PORTS_LOCATION))
			{
			  mad_prefix =
			    mid (prefix, strlen (PORTS_LOCATION) + 1, FINE);
			  if (mad_prefix[strlen (mad_prefix) - 1] != '/')
			    mad_prefix = strcat (mad_prefix, "/");
			  mad_prefix = strdup (mad_prefix);
			  prefix = strdup (PORTS_LOCATION);
			}
		    }
		}
	      if (strncmp (riga, "LOCAL_DIR", 9) == 0)
		if (strlen (value = get_value (riga, "LOCAL_DIR")))
		  collezione = strdup (value);
	    }
	  if ((strlen (prefix) * strlen (collezione)) > 0)
	    {
	      if (strlen (mad_prefix) > 0)
		collezione = strcat (mad_prefix, collezione);
	      p = repolist_add (collezione, prefix, p);
	      return (p);
	    }
	}
    }
  return (p);
}

struct repolist *
parse_local (char *path, struct repolist *p)
{
  FILE *file;
  if ((file = fopen (path, "r")))
    {
      char row[255];
      char *prefix = "";
      char *mad_prefix = "";
      char *collezione = "";
      char *value = "";
      while (fgets (row, 255, file))
	{
	  strcpy (row, trim (row));
	  if (row[0] != '#')
	    {
	      if (strncmp (row, "PATH", 4) == 0)
		{
		  if (strlen (value = get_value (row, "PATH")))
		    prefix = strdup (value);
		}
	    }
	}
      collezione = rindex (path, '/');
      collezione =
	mid (collezione, 0,
	     strlen (collezione) - strlen (index (collezione, '.')));
      mad_prefix = strdup ("local");
      collezione = strcat (mad_prefix, strdup (collezione));
      p = repolist_add (collezione, prefix, p);
    }
  return (p);
}

struct deplist *
deplist_from_deprow (char *deprow)
{
  struct deplist *d = NULL;
  if (strlen (deprow) > 0)
    {
      char deps[255][MASSIMO];
      int how_many_deps, i;
      deprow = sed (deprow, ",", " ");
      how_many_deps = split (deprow, " ", deps);
      for (i = 0; i < how_many_deps; i++)
	d = deplist_add (trim (deps[i]), d);
    }
  return (d);
}

int
parse_pkgfile (char *filename, char *repo)
{
  FILE *file;
  size_t n = 0;
  char *line = NULL;
  ssize_t nread;

  struct deplist *d = NULL;

  char *name = NULL, *version = NULL, *release = NULL;

  file = fopen (filename, "r");

  if (file == NULL)
    return (EXIT_FAILURE);

  while ((nread = getline (&line, &n, file)) != -1)
    {
      char *l;
      l = strdup (trim (line));

      if (l[0] == '#')
	{
	  l = mid (l, 1, FINE);
	  l = trim (l);

	  if (strncasecmp (l, "Depends", 7) == 0)
	    {
	      if (strstr (l, ":"))
		{
		  l = mid (strstr (l, ":"), 1, FINE);
		  l = trim (l);
		  d = deplist_from_deprow (l);
		}
	    }
	}
      else if (strncmp (l, "name", 4) == 0)
	name = get_value (l, "name");
      else if (strncmp (l, "version", 7) == 0)
	version = get_value (l, "version");
      else if (strncmp (l, "release", 7) == 0)
	release = get_value (l, "release");
    }

  if (!(name && version && release))
    return (EXIT_FAILURE);

  version = sed (version, " ", "_");

  fprintf (cachefile, "%s %s-%s %s", name, version, release, repo);

  while (d != NULL)
    {
      fprintf (cachefile, " %s", d->name);
      d = d->next;
    }

  fprintf (cachefile, "\n");

  if (line)
    free (line);
  fclose (file);
  return EXIT_SUCCESS;
}

int
read_from_dir (char *collezione, char *prefix)
{
  DIR *dir;
  struct dirent *info_file;
  struct stat tipo_file;
  char percorso[255];
  char nome_file[255];
  strcpy (percorso, prefix);
  if (strncmp (collezione, "local", 5) != 0)
    {
      strcat (percorso, "/");
      strcat (percorso, collezione);
    }

  dir = opendir (percorso);
  printf ("%s\n", percorso);
  if (dir == NULL)
    return (EXIT_FAILURE);

  while ((info_file = readdir (dir)))
    {
      strcpy (nome_file, percorso);
      strcat (nome_file, "/");
      strcat (nome_file, info_file->d_name);
      stat (nome_file, &tipo_file);
      if (S_ISDIR (tipo_file.st_mode) && info_file->d_name[0] != '.')
	{
	  strcat (nome_file, "/Pkgfile");
	  parse_pkgfile (nome_file, collezione);
	}
    }
  closedir (dir);
  return (EXIT_SUCCESS);
}

int
build_cache (struct repolist *r)
{
  printf ("Building cache!\n");
  if (!(cachefile = fopen (CACHE, "w")))
    {
      return (-1);
    }
  while (r != NULL)
    {
      printf ("rfd %s %i\n", r->name, read_from_dir (r->name, r->path));
      r = r->next;
    }
  fclose (cachefile);
  chmod (CACHE, S_IREAD | S_IWRITE | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  return (0);
}

struct pkglist *
lsports ()
{
  FILE *file;
  struct pkglist *p = NULL;
  file = fopen (CACHE, "r");
  if (file == NULL)
    return (NULL);

  size_t n = 0;
  char *line = NULL;
  int nread = getline (&line, &n, file);

  if (nread < 0)
    {
      if (build_cache (ilenia_repos) != EXIT_SUCCESS)
	return (NULL);
      nread = getline (&line, &n, file);
    }

  while (nread > 0)
    {
      int i, num = count (line, ' ');
      char *splitted_line[num];
      split2 (line, " ", splitted_line);
      struct deplist *d = NULL;
      for (i = 3; i < num; i++)
	{
	  if (strlen (trim (splitted_line[i])) > 0)
	    d = deplist_add (trim (splitted_line[i]), d);
	}
      p = pkglist_add_ordered (trim (splitted_line[0]),
			       trim (splitted_line[1]),
			       trim (splitted_line[2]), d, p);
      nread = getline (&line, &n, file);
    }

  fclose (file);
  return (p);
}

struct repolist *
build_repolist ()
{
  struct repolist *p = NULL;
  DIR *etc_ports;
  struct dirent *info_file;
  char filename[255];
  char extension[255];
  etc_ports = opendir ("/etc/ports");
  while ((info_file = readdir (etc_ports)))
    {
      if (strstr (info_file->d_name, "."))
	{
	  strcpy (extension, strstr (info_file->d_name, "."));
	  strcpy (extension, mid (extension, 1, FINE));
	  strcpy (filename, "/etc/ports/");
	  strcat (filename, info_file->d_name);
	  if (strcmp (extension, "cvsup") == 0)
	    p = parsa_cvsup (filename, p);
	  else if (strcmp (extension, "httpup") == 0)
	    p = parsa_httpup (filename, p);
	  else if (strcmp (extension, "cvs") == 0)
	    p = parse_cvs (filename, p);
	  else if (strcmp (extension, "local") == 0)
	    p = parse_local (filename, p);
	}
    }
  return (p);
}
