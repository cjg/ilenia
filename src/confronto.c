/***************************************************************************
 *            confronto.c
 *
 *  Sat Jul 10 13:33:11 2004
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
#include "manipola.h"
#include "vercmp.h"
#include "confronto.h"

void
print_formattato (char *str1, char *str2, char *str3, char *str4)
{
	char tmp[255];
	strcpy (tmp, str1);
	strcat (tmp, spazi (19 - strlen (str1)));
	strcat (tmp, spazi (2));
	strcat (tmp, spazi (19 - strlen (str2)));
	strcat (tmp, str2);
	strcat (tmp, spazi (2));
	strcat (tmp, str3);
	strcat (tmp, spazi (10 - strlen (str3)));
	strcat (tmp, spazi (2));
	strcat (tmp, spazi (13 - strlen (str4)));
	strcat (tmp, str4);
	printf ("%s\n", tmp);
}

struct db *
confronta (struct db *pacchetti_db, struct db *ports_db, int tipo,
	   int opzioni, int stampa)
{
	struct db *p = NULL;
	struct db *c = NULL;
	if (stampa)
		printf ("Name                   Installed Version  Repository  Port Version \n");
	while (pacchetti_db != NULL)
	{
		p = ports_db;
		while (p != NULL)
		{
			if (strcmp (p->nome, pacchetti_db->nome) == 0)
			{
				int test;
				int skip = 0;
				if (tipo == DIFFERENZE)
				{
					test = strcmp (pacchetti_db->versione,
						       p->versione);
				}
				else
				{
					test = vercmp (pacchetti_db->versione,
						       p->versione);
				}
				if ((pacchetti_db->collezione[0] == 'R') &&
				    (opzioni != NO_REPO
				     && opzioni != NO_FAVORITE))
				{
					if (strcmp
					    (mid
					     (pacchetti_db->collezione, 2,
					      FINE), p->collezione) != 0)
						skip = 1;
				}
				if ((pacchetti_db->collezione[0] == 'V') &&
				    (opzioni != NO_VERSION
				     && opzioni != NO_FAVORITE))
				{
					if (strcmp
					    (mid
					     (pacchetti_db->collezione, 2,
					      FINE),
					     pacchetti_db->versione) == 0)
						skip = 1;
				}
				if (test != 0 && skip != 1)
				{
					c = inserisci_elemento_ordinato
						(pacchetti_db->nome,
						 p->versione, p->collezione,
						 c);
					if (stampa)
						print_formattato
							(pacchetti_db->nome,
							 pacchetti_db->
							 versione,
							 p->collezione,
							 p->versione);
				}
			}
			p = p->prossimo;
		}
		pacchetti_db = pacchetti_db->prossimo;
	}
	return (c);
}

char *
il_piu_aggiornato (char *pacchetto, struct db *p)
{
	char _versione[255] = "";
	static char _collezione[255] = "";
	while (p != NULL)
	{
		if (strcmp (pacchetto, p->nome) == 0)
		{
			if (strcmp (_versione, "") == 0)
			{
				strcpy (_versione, p->versione);
				strcpy (_collezione, p->collezione);
			}
			else
			{
				if (vercmp (_versione, p->versione))
				{
					strcpy (_versione, p->versione);
					strcpy (_collezione, p->collezione);
				}
			}
		}
		p = p->prossimo;
	}
	return ((char *) _collezione);
}

char *
questa_versione (char *nome, char *versione, struct db *p)
{
	static char collezione[255] = "";
	while (p != NULL)
	{
		if (strcmp (nome, p->nome) == 0)
		{
			if (strcmp (versione, p->versione) == 0)
			{
				strcpy (collezione, p->collezione);
				return (p->collezione);
			}
		}
		p = p->prossimo;
	}
	return ((char *) collezione);
}

char *
questa_collezione (char *nome, char *collezione, struct db *p)
{
	static char versione[255] = "";
	while (p != NULL)
	{
		if (strcmp (nome, p->nome) == 0)
		{
			if (strcmp (collezione, p->collezione) == 0)
			{
				strcpy (versione, p->versione);
				return (p->versione);
			}
		}
		p = p->prossimo;
	}
	return ((char *) versione);
}
