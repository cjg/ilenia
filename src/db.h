/***************************************************************************
 *            db.h
 *
 *  Sat Jul 10 11:35:35 2004
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

struct db
{
  char nome[255];
  char versione[255];
  char collezione[255];
  struct deplist *depends;
  struct db *prossimo;
};

struct db *inserisci_elemento (char *_nome, char *_versione,
			       char *_collezione, struct deplist *d,
			       struct db *p);
struct db *inserisci_elemento_ordinato (char *_nome, char *_versione,
					char *_collezione, struct deplist *d,
					struct db *p);
struct db *inserisci_elemento_inverso (char *_nome, char *_versione,
				       char *_collezione, struct deplist *d,
				       struct db *p);
struct db *cerca (char *parametro, struct db *p);
int conta (struct db *p);
int esiste (char *qualcosa, struct db *p);
struct db *add_deplist (struct deplist *d, struct db *p);
struct db *db_like (char *delim, struct db *p);
