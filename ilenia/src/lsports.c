/***************************************************************************
 *            lsports.c
 *
 *  Sat Jul 10 12:57:55 2004
 *  Copyright  2004  Coviello Giuseppe
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
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include "manipola.h"
#include "db.h"


struct db *
lsports_classico ()
{
	DIR *ports_dir, *repo_dir;
	FILE *pkgfile, *cachefile;
	struct dirent *info_file;
	struct stat tipo_file;
	struct db *p = NULL;
	char nome_file[255] = "";
	char nome_dir[255] = "";
	char collezione[255] = "";
	int conta;
	if (!(cachefile = fopen ("/tmp/ilenia.cache", "w")))
	{
		return p;
	}
	fprintf (cachefile, "%ld \n", time (NULL));
	ports_dir = opendir ("/usr/ports");
	while ((info_file = readdir (ports_dir)))
	{
		strcpy (nome_file, "/usr/ports/");
		strcat (nome_file, info_file->d_name);
		stat (nome_file, &tipo_file);
		if (S_ISDIR (tipo_file.st_mode)
		    && info_file->d_name[0] != '.')
		{
			strcpy (collezione, info_file->d_name);
			strcpy (nome_dir, "/usr/ports/");
			strcat (nome_dir, info_file->d_name);
			repo_dir = opendir (nome_dir);
			while ((info_file = readdir (repo_dir)))
			{
				strcpy (nome_file, nome_dir);
				strcat (nome_file, "/");
				strcat (nome_file, info_file->d_name);
				stat (nome_file, &tipo_file);
				if (S_ISDIR (tipo_file.st_mode)
				    && info_file->d_name[0] != '.')
				{
					strcat (nome_file, "/Pkgfile");
					if ((pkgfile =
					     fopen (nome_file, "r")))
					{
						char riga[255] = "";
						char nome[255] = "";
						char versione[255] = "";
						int release = 0;
						while (fgets
						       (riga, 255, pkgfile))
						{
							strcpy (riga,
								trim (riga));
							if (strncmp
							    (riga, "name",
							     4) == 0)
							{
								strcpy (nome,
									mid
									(riga,
									 5,
									 strlen
									 (riga)
									 -
									 5));
							}
							if (strncmp
							    (riga, "version",
							     7) == 0)
							{
								strcpy (versione, mid (riga, 8, strlen (riga) - 8));
							}
							if (strncmp
							    (riga, "release",
							     7) == 0)
							{
								strcat (versione, "-");
								strcat (versione, mid (riga, 8, strlen (riga) - 8));
								release = 1;
							}
							if (strlen (nome)
							    &&
							    strlen (versione)
							    && release)
							{
								p = inserisci_elemento_ordinato (nome, versione, collezione, p);
								fprintf (cachefile, "%s %s %s \n", nome, versione, collezione);
								conta++;
								strcpy (nome,
									"");
								strcpy (versione, "");
								release = 0;
							}
						}
						fclose (pkgfile);
					}
				}
			}
			closedir (repo_dir);
		}
	}
	closedir (ports_dir);
	fclose (cachefile);
	return p;
}

struct db * lsports ()
{
  FILE *cachefile;
  struct db *p = NULL;
  int update, cache;
  char riga[255];

  if ((cachefile = fopen ("/var/cache/ilenia", "r"))) {
    fgets (riga, 255, cachefile);
    fclose (cachefile);
    update = atoi (riga);
  } else {
    p = lsports_classico ();
    return p;
  }

  if ((cachefile = fopen ("/tmp/ilenia.cache", "r"))) {
    fgets (riga, 255, cachefile);
    cache = atoi (riga);
    if (cache >= update) {
      while (fgets (riga, 255, cachefile)) {
	char nome[255];
	char versione[255];
	char collezione[255];
	strcpy (nome, riga);
	strtok (nome, " ");
	strcpy (riga, mid (riga, strlen (nome), FINE));
	strcpy (riga, trim (riga));

	strcpy (versione, riga);
	strtok (versione, " ");
	strcpy (riga, mid (riga, strlen (versione), -1));
	strcpy (riga, trim (riga));
	strcpy (collezione, riga);
	//printf("%s %s %s \n", nome, versione, collezione);
	p = inserisci_elemento_ordinato (nome, versione, collezione, p);
      }
    } else {
      p = lsports_classico ();
    }
    fclose (cachefile);
  }
  return p;
}
