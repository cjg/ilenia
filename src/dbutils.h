/***************************************************************************
 *            dbutils.h
 *
 *  Mon Mar 06 13:40:16 2006
 *  Copyright 2006 - 2007 Coviello Giuseppe
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

#ifndef _DBUTILS_H
#define _DBUTILS_H
#define MAXFIELD 255

#include "pkglist.h"

typedef struct db_package {
	char name[MAXFIELD];
	char version[MAXFIELD];
	char repo[MAXFIELD];
	int n_deps;
	char d_key[MAXFIELD];
	//char deps[MAXFIELD][MAXFIELD];
} DB_PACKAGE;
//int pack(void **buffer, char *data[], int n);
//void unpack(void *databuff, char *data[], int n);
int open_database(void);
int safe_exit(int ret);
void dbports_insert(struct pkglist *p);
struct pkglist *db_get(char *name);

/*int package_pack(void **buffer, package pkg);
  package package_unpack(void *buffer);*/

#endif				/* _DBUTILS_H */

