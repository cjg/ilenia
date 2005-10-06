/***************************************************************************
 *            ilenia.h
 *
 *  Tue Sep 28 11:06:51 2004
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

struct pkglist *ilenia_ports;
struct pkglist *ilenia_pkgs;
struct repolist *ilenia_repos;
struct aliaseslist *ilenia_aliases;

char *post_pkgadd;
int ask_for_update;

#define CACHE "/var/cache/ilenia"
#define ALIAS_FILE "/etc/ports/alias"

char *get_value (char *s, char *var);
int parse_ileniarc ();
int ask (char *question, ...);
