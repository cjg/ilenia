/***************************************************************************
 *            aggiorna.c
 *
 *  Thu Jul 15 16:27:56 2004
 *  Copyright  2004 - 2005  Coviello Giuseppe
 *  slash@crux-it.org
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
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "manipola.h"
#include "ilenia.h"

int
cvsup (char *nome_file)
{
  int stato;
  pid_t pid = fork ();
  if (pid == 0)
    {
      execl ("/usr/bin/cvsup", "", "-g", "-L", "1", "-r", "0", "-c",
	     ".cvsup", nome_file, 0);
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
httpup (char *nome_file)
{
  FILE *file;
  if ((file = fopen (nome_file, "r")))
    {
      char riga[255];
      static char root_dir[255];
      static char url[255];
      while (fgets (riga, 255, file))
	{
	  strcpy (riga, trim (riga));
	  if (strcmp (mid (riga, 0, 8), "ROOT_DIR") == 0)
	    {
	      strcpy (root_dir, mid (riga, 9, FINE));
	    }
	  if (strcmp (mid (riga, 0, 3), "URL") == 0)
	    {
	      strcpy (url, mid (riga, 4, FINE));
	    }
	  if (strlen (root_dir) && strlen (url))
	    {
	      int stato;
	      pid_t pid = fork ();
	      if (pid == 0)
		{
		  execl ("/usr/bin/httpup", "", "sync", url, root_dir, 0);
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
	      root_dir[0] = '\0';
	      url[0] = '\0';
	      return (stato);
	    }
	}
    }
  return (-1);
}

int
cvs (char *filepath)
{
  int state;
  pid_t pid = fork ();
  if (pid == 0)
    {
      execl ("/etc/ports/drivers/cvs", "", filepath, 0);
    }
  else if (pid < 0)
    {
      state = -1;
    }
  else
    {
      while ((waitpid (pid, &state, 0) == 0))
	{
	}
    }
  return (state);
}


int
aggiorna_collezione (char *collezione)
{
  if (getuid () != 0)
    {
      printf ("ilenia: only root can update the ports tree\n\n");
      return (-1);
    }
  FILE *file;
  char nome_file[255] = "";
  strcpy (nome_file, CACHE);
  if ((file = fopen (nome_file, "w")))
    {
      fclose (file);
    }
  strcpy (nome_file, "/etc/ports/");
  strcat (nome_file, collezione);
  strcat (nome_file, ".cvsup");
  if ((file = fopen (nome_file, "r")))
    {
      fclose (file);
      cvsup (nome_file);
      return (0);
    }
  strcpy (nome_file, "/etc/ports/");
  strcat (nome_file, collezione);
  strcat (nome_file, ".httpup");
  if ((file = fopen (nome_file, "r")))
    {
      fclose (file);
      httpup (nome_file);
      return (0);
    }
  // supporting crux ppc
  strcpy (nome_file, "/etc/ports/");
  strcat (nome_file, collezione);
  strcat (nome_file, ".cvs");
  if ((file = fopen (nome_file, "r")))
    {
      fclose (file);
      httpup (nome_file);
      return (0);
    }
  printf ("ilenia: %s not found\n\n", collezione);
  return (-1);
}

int
aggiorna_ports ()
{
  if (getuid () != 0)
    {
      printf ("ilenia: only root can update the ports tree\n\n");
      return (-1);
    }
  DIR *etc_ports;
  struct dirent *info_file;
  char nome_file[255];
  char estensione[255];
  FILE *file;
  strcpy (nome_file, CACHE);
  if ((file = fopen (nome_file, "w")))
    {
      fclose (file);
    }
  etc_ports = opendir ("/etc/ports");
  while ((info_file = readdir (etc_ports)))
    {
      if (strstr (info_file->d_name, "."))
	{
	  strcpy (estensione, strstr (info_file->d_name, "."));
	  strcpy (estensione, mid (estensione, 1, FINE));
	  strcpy (nome_file, "/etc/ports/");
	  strcat (nome_file, info_file->d_name);
	  if (strcmp (estensione, "cvsup") == 0)
	    {
	      cvsup (nome_file);
	    }
	  else if (strcmp (estensione, "httpup") == 0)
	    {
	      httpup (nome_file);
	    }
	  // supporting crux ppc
	  else if (strcmp (estensione, "cvs") == 0)
	    {
	      cvs (nome_file);
	    }
	}
    }
  return (0);
}
