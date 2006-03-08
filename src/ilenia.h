/***************************************************************************
 *            ilenia.h
 *
 *  Tue Sep 28 11:06:51 2004
 *  Copyright  2004 - 2006  Coviello Giuseppe
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

#ifndef _ILENIA_H
#define _ILENIA_H

struct pkglist *ilenia_ports;
struct pkglist *ilenia_pkgs;
struct repolist *ilenia_repos;
struct pkglist *ilenia_favoriterepo;
struct pkglist *ilenia_favoriteversion;
struct aliaseslist *ilenia_aliases;
struct pkgmklist *ilenia_favoritepkgmk;

char *post_pkgadd;
int not_found_policy;
int ask_for_update;

#define CACHE "/var/cache/ilenia"
#define ALIAS_FILE "/etc/ports/alias"
#define ASK_POLICY 0
#define STOP_POLICY 1
#define NEVERMIND_POLICY 2
#define PATH_MAX 4096

char *get_value(char s[], char *var);
int parse_ileniarc();
int ask(char *question, ...);

#endif				/* _ILENIA_H */
