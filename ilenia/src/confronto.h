/***************************************************************************
 *            confronto.h
 *
 *  Sat Jul 10 13:27:54 2004
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

#define DIFFERENZE	3
#define AGGIORNATI	4

#define NORMALE		0
#define NO_REPO		1
#define NO_VERSION	2
#define NO_FAVORITE	3


struct db *confronta (struct db *pacchetti_db, struct db *ports_db,
		      int tipo, int opzioni, int stampa);
char *il_piu_aggiornato (char *pacchetto, struct db *p);
char *questa_versione (char *nome, char *versione, struct db *p);
char *questa_collezione (char *nome, char *collezione, struct db *p);
