/***************************************************************************
 *            pkgutils.c
 *
 *  Wed Sep  1 18:55:42 2004
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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "pkglist.h"
#include "manipulator.h"
#include "confront.h"
#include "dependencies.h"
#include "ilenia.h"
//#include "lsports.h"
#include "lspkgs.h"
#include "repolist.h"
#include "output.h"

int
esegui_script (char *path, char *script)
{
  pid_t pid = fork ();
  int stato;
  if (pid == 0)
    {
      chdir (path);
      execl ("/bin/bash", "", script, 0);
    }
  else if (pid < 0)
    {
      stato = -1;
    }
  else
    {
      while ((waitpid (pid, &stato, 0) == 0))
	{
	}
    }
  return (stato);
}

int
compila_e (int aggiorna, char *port)
{
  int stato;
  char azione[255];
  char install_script[255];
  FILE *file;
  if (aggiorna)
    {
      strcpy (azione, "-u");
    }
  else
    {
      strcpy (azione, "-i");
    }
  strcpy (install_script, port);
  strcat (install_script, "/pre-install");
  if ((file = fopen (install_script, "r")))
    {
      fclose (file);
      if (esegui_script (port, "pre-install") != 0)
	return (-1);
    }

  pid_t pid = fork ();
  if (pid == 0)
    {
      chdir (port);
      execl ("/usr/bin/pkgmk", "", "-d", "-f", azione, 0);
    }
  else if (pid < 0)
    {
      stato = -1;
    }
  else
    {
      while ((waitpid (pid, &stato, 0) == 0))
	{
	}
    }
  if (stato != 0)
    return (stato);
  strcpy (install_script, port);
  strcat (install_script, "/post-install");
  if ((file = fopen (install_script, "r")))
    {
      fclose (file);
      if (esegui_script (port, "post-install") != 0)
	return (-1);
    }
  if (post_pkgadd)
    {
      FILE *post_pkgadd_sh;
      if ((post_pkgadd_sh = fopen ("/tmp/post_pkgadd.sh", "w")))
	{
	  fprintf (post_pkgadd_sh, "#!/bin/sh\n\n%s\n\n# End of file",
		   post_pkgadd);
	  fclose (post_pkgadd_sh);

	  pid = fork ();
	  if (pid == 0)
	    {
	      chdir (port);
	      execl ("/bin/sh", "", "/tmp/post_pkgadd.sh", 0);
	    }
	  else if (pid < 0)
	    stato = -1;
	  else
	    {
	      while ((waitpid (pid, &stato, 0) == 0))
		{
		}
	    }
	  if (stato != 0)
	    return (stato);
	}
    }
  return (0);
}

int
aggiorna_pacchetto_ (int opzioni_confronto, char *pacchetto)
{
  int aggiornare = 0;
  char collezione[255];
  char port[255];
  struct pkglist *p;
  if (pkglist_exists (pacchetto, ilenia_pkgs) == 0)
    aggiornare = 1;

  strcpy (collezione, pkglist_get_newer (pacchetto, ilenia_ports));

  if (opzioni_confronto != NO_FAVORITE_REPO
      && opzioni_confronto != NO_FAVORITES)
    {
      p = get_favorite (REPO);
      if ((p = pkglist_find (pacchetto, p)))
	{
	  strcpy (collezione, p->repo);
	}
    }

  if (opzioni_confronto != NO_FAVORITE_VERSION && opzioni_confronto !=
      NO_FAVORITES)
    {
      p = get_favorite (VERSION);
      if ((p = pkglist_find (pacchetto, p)))
	{
	  strcpy (collezione,
		  pkglist_get_from_version (pacchetto, p->version,
					    ilenia_ports));
	}
    }

  strcpy (port, repolist_find (collezione, ilenia_repos)->path);
  if (port[strlen (port)] != '/')
    strcat (port, "/");
  if (strncmp (collezione, "local", 5) != 0)
    strcat (port, collezione);
  strcat (port, "/");
  strcat (port, pacchetto);
  return (compila_e (aggiornare, port));
}


int
aggiorna_pacchetto (int opzioni_confronto, char *pacchetto)
{
  if (getuid () != 0)
    {
      printf ("ilenia: only root can update or install packages\n\n");
      return (-1);
    }
  struct pkglist *d = NULL;
  if (opzioni_confronto >= 0)
    {
      d = get_dependencies (pacchetto);
      while (d->next != NULL)
	{
	  printf ("%s [", d->name);
	  if (strcmp (d->repo, "not found") != 0)
	    {
	      if (pkglist_exists (d->name, ilenia_pkgs) != 0)
		{
		  printf ("install now]\n");
		  if (aggiorna_pacchetto_ (opzioni_confronto, d->name) != 0)
		    return (-1);
		}
	      else
		{
		  printf ("installed]\n");
		}
	    }
	  else
	    {
	      printf ("not found]\n");
	    }
	  d = d->next;
	}
      if (strcmp (d->repo, "not found") != 0)
	{
	  if (aggiorna_pacchetto_ (opzioni_confronto, d->name) != 0)
	    return (-1);
	}
      else
	{
	  printf ("%s [not found]\n", d->name);
	  return (-1);
	}
    }
  else
    {
      opzioni_confronto *= -1;
      if (pkglist_exists (pacchetto, ilenia_ports) == 0)
	{
	  if (aggiorna_pacchetto_ (opzioni_confronto, pacchetto) != 0)
	    return (-1);
	}
      else
	{
	  printf ("%s [not found]\n", pacchetto);
	  return (-1);
	}
    }
  return (0);
}


int
aggiorna_pacchetti (int opzioni_confronto)
{
  if (getuid () != 0)
    {
      printf ("ilenia: only root can update or install packages\n\n");
      return (-1);
    }
  struct pkglist *p = NULL;
  struct pkglist *q = NULL;
  p =
    pkglist_confront (ilenia_pkgs, ilenia_ports, UPDATED, opzioni_confronto,
		      0);
  while (p != NULL)
    {
      if (pkglist_exists (p->name, q) != 0)
	{
	  char *repo = pkglist_get_newer (p->name, p);
	  q =
	    pkglist_add_ordered (p->name,
				 pkglist_get_from_repo (p->name, repo, p),
				 repo, NULL, q);
	}
      p = p->next;
    }
  if (pkglist_len (q) < 1)
    {
      printf ("All packages are up-to-date\n");
      return (0);
    }
  /*
     someone wants that ilenia ask them if they're sure to update all packages,
     I hate this feature, then I've to add another feature that bypass this feature
   */
  if (ask_for_update)
    {
      print_db (q);
      if (!ask ("Are you sure to update the above packages? [y/n] "))
	return (1);
    }
  while (q != NULL)
    {
      if (aggiorna_pacchetto_ (opzioni_confronto, q->name) != 0)
	return (-1);
      q = q->next;
    }
  return (0);
}

int
do_pkgrm (char *pkg)
{
  int stato;
  pid_t pid = fork ();
  if (pid == 0)
    {
      printf ("Removing %s\n", pkg);
      execl ("/usr/bin/pkgrm", "pkgrm", pkg, 0);
    }
  else if (pid < 0)
    {
      stato = -1;
    }
  else
    {
      while ((waitpid (pid, &stato, 0) == 0))
	{
	}
    }
  if (stato != 0)
    return (stato);
  return (0);
}

int
pkgrm (char *pkg, int nocheckdeps, int removeall)
{
  if (getuid () != 0)
    {
      printf ("ilenia: only root can remove packages\n\n");
      return (-1);
    }
  struct pkglist *p = NULL;
  if (removeall)
    {
      p = get_dependents (pkg, 1);
      while (p != NULL)
	{
	  do_pkgrm (p->name);
	  p = p->next;
	}
      return (0);
    }
  p = get_dependents (pkg, 0);
  if (pkglist_len (p) > 1)
    {
      if (nocheckdeps)
	{
	  do_pkgrm (pkg);
	  return (0);
	}
      else
	{
	  printf
	    ("ilenia: there are some packages that depends from %s, use --all or --no-deps, to remove all packages that depends from %s or to not check dependencies (use at your risk)\nYou can use ilenia -T %s to see a list of the packages that need %s.\n",
	     pkg, pkg, pkg, pkg);
	  return (-1);
	}
    }
  do_pkgrm (pkg);
  return (0);
}
