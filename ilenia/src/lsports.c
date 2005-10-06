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

struct pkglist *
parsa_pkgfile (char *percorso, char *collezione, struct pkglist *p)
{
  FILE *pkgfile;
  if ((pkgfile = fopen (percorso, "r")))
    {
      char riga[255] = "";
      char *nome = "";
      char *value = "";
      char versione[255] = "";
      int release = 0;
      struct deplist *d = NULL;
      while (fgets (riga, 255, pkgfile))
	{
	  strcpy (riga, trim (riga));
	  if (riga[0] == '#')
	    {
	      strcpy (riga, mid (riga, 1, FINE));
	      strcpy (riga, trim (riga));
	      if (strncasecmp (riga, "Depends", 7) == 0)
		{
		  char deprow[MASSIMO];
		  if (strstr (riga, ":"))
		    {
		      strcpy (riga, mid (strstr (riga, ":"), 1, FINE));
		      strcpy (deprow, trim (riga));
		      d = deplist_from_deprow (deprow);
		    }
		}
	    }
	  else if (strncmp (riga, "name", 4) == 0)
	    {
	      if (strlen (value = get_value (riga, "name")))
		nome = strdup (value);
	    }
	  else if (strncmp (riga, "version", 7) == 0)
	    {
	      if (strlen (value = get_value (riga, "version")))
		strcpy (versione, value);
	    }
	  else if (strncmp (riga, "release", 7) == 0)
	    {
	      if (strlen (value = get_value (riga, "release")))
		{
		  strcat (versione, "-");
		  strcat (versione, value);
		  release = 1;
		}
	    }
	  if (strlen (nome) && strlen (versione) && release)
	    {
	      strcpy (versione, sed (versione, " ", "_"));
	      p = pkglist_add_ordered (nome, versione, collezione, d, p);
	      char dependencies[MASSIMO] = "";
	      if (d != NULL)
		{
		  while (d != NULL)
		    {
		      strcat (dependencies, d->name);
		      strcat (dependencies, " ");
		      d = d->next;
		    }
		}

	      fprintf (cachefile, "%s %s %s %s\n", nome, versione,
		       collezione, dependencies);
	      strcpy (nome, "");
	      strcpy (versione, "");
	      release = 0;
	      fclose (pkgfile);
	      return (p);
	    }
	}
      fclose (pkgfile);
    }
  return (p);
}

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


struct pkglist *
leggi_dir (char *collezione, char *prefix, struct pkglist *p)
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
  if ((dir = opendir (percorso)))
    {
      while ((info_file = readdir (dir)))
	{
	  strcpy (nome_file, percorso);
	  strcat (nome_file, "/");
	  strcat (nome_file, info_file->d_name);
	  stat (nome_file, &tipo_file);
	  if (S_ISDIR (tipo_file.st_mode) && info_file->d_name[0] != '.')
	    {
	      strcat (nome_file, "/Pkgfile");
	      p = parsa_pkgfile (nome_file, collezione, p);
	    }
	}
    }
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

int
build_cache (struct repolist *r)
{
  struct pkglist *ports = NULL;
  printf ("Building cache!\n");
  if (!(cachefile = fopen (CACHE, "w")))
    {
      return (-1);
    }
  while (r != NULL)
    {
      ports = leggi_dir (r->name, r->path, ports);
      r = r->next;
    }
  fclose (cachefile);
  chmod (CACHE, S_IREAD | S_IWRITE | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  return (0);
}

struct pkglist *
lsports ()
{
  FILE *cachefile;
  struct pkglist *p = NULL;
  char riga[255];
  if ((cachefile = fopen (CACHE, "r")))
    {
      if (!fgets (riga, 255, cachefile))
	if (build_cache (ilenia_repos) != 0)
	  return (NULL);
      while (fgets (riga, 255, cachefile))
	{
	  int len;
	  char splitted_row[MASSIMO][MASSIMO];
	  len = split (riga, " ", splitted_row);
	  int i;
	  struct deplist *d = NULL;
	  for (i = 3; i < len; i++)
	    {
	      if (strlen (trim (splitted_row[i])) > 0)
		d = deplist_add (trim (splitted_row[i]), d);
	    }
	  p = pkglist_add_ordered (trim (splitted_row[0]),
				   trim (splitted_row[1]),
				   trim (splitted_row[2]), d, p);
	}
      fclose (cachefile);
      return (p);
    }
  return (NULL);
}
