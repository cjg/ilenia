/***************************************************************************
 *            dipendenze.c
 *
 *  Sat Sep 11 18:04:21 2004
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
#include "db.h"
#include "lspacchetti.h"
#include "lsports.h"
#include "manipola.h"
#include "confronto.h"
#include "output.h"

struct db *
cerca_dipendenze (struct db *p, struct db *s)
{
	struct db *d = NULL;
	FILE *file;
	char pkgfile[255];
	d = p;
	while (p != NULL)
	{
		strcpy (pkgfile, "/usr/ports/");
		strcat (pkgfile, p->collezione);
		strcat (pkgfile, "/");
		strcat (pkgfile, p->nome);
		strcat (pkgfile, "/Pkgfile");
		//char queste_dipendenze[255];
		if ((file = fopen (pkgfile, "r")))
		{
			char riga[255] = "";
			char dep[255] = "";
			while (fgets (riga, 255, file))
			{
				if (riga[0] == '#')
				{
					strcpy (riga, mid (riga, 1, FINE));
					strcpy (riga, trim (riga));
					if (strncasecmp (riga, "Depends", 7)
					    == 0)
					{
						strcpy (riga,
							mid (strstr
							     (riga, ":"), 1,
							     FINE));
						strcpy (dep, trim (riga));
						break;
					}
				}
			}
				if (strlen (dep) > 0)
			{
				char tmp[255];
				strcpy (dep, sed (dep, " ", ","));
				while (strlen (dep) > 0)
				{
					if (strstr (dep, ","))
					{
						strcpy (tmp,
							strstr (dep, ","));
						strcpy (tmp,
							mid (dep, 0,
							     strlen (dep) -
							     strlen (tmp)));
						strcpy (dep,
							mid (strstr
							     (dep, ","), 1,
							     FINE));
					}
					else
					{
						strcpy (tmp, dep);
						strcpy (dep, "");
					}

					strcpy (tmp, trim (tmp));
					if ((!(cerca (tmp, d)))&&strlen(tmp))
					/*
					{
						d = rimuovi_elemento (tmp, d);

					}*/
					
					d = inserisci_elemento_inverso (tmp,
								"a",
								il_piu_aggiornato
								(tmp, s), d);
//printf("%s\n", tmp);
							

					strcpy (riga, trim (riga));
				}
			}
		}
		p = p->prossimo;
	}

	return (d);
}

struct db *
dipendenze (char *pacchetto)
{
	//struct db *p = NULL;
	struct db *s = NULL;
	struct db *d = NULL;
	char collezione[255];
	s = lsports ();
	strcpy (collezione, il_piu_aggiornato (pacchetto, s));
	d = inserisci_elemento_inverso (pacchetto, "", collezione, d);
	int i;
	while (i != conta (d))
	{
		i = conta (d);
		d = cerca_dipendenze (d, s);
		//print_db(d);
	}
	//print_db(d);
	//d = rimuovi_duplicati (d);
	return (d);
}

void
stampa_dipendenze (char *pacchetto)
{
	struct db *p = NULL;
	struct db *d = NULL;
	d = dipendenze (pacchetto);
	p = lspacchetti ();
	while (d != NULL)
	{
		printf ("%s [", d->nome);
		if (!(cerca (d->nome, p)))
			printf ("installa]\n");
		else
			printf ("installato]\n");
		d = d->prossimo;
	}
}
