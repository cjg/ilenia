/***************************************************************************
 *            dbutils.c
 *
 *  Mon Mar 06 13:41:32 2006
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

#include <db.h>

#include <stdlib.h>
#include <string.h>
#include "dbutils.h"
#include "pkglist.h"
#include "ilenia.h"
#include "list.h"
#include "deplist.h"

DB *dbdeps;
/*
int pack_string(char *buffer, char *string, int start_pos)
{
	int string_size = strlen(string)+1;
    	memcpy(buffer+start_pos, string, string_size);
	return start_pos+string_size;
}

int pack(char **buffer, struct list *data)
{
	int buffsize=0, bufflen=0, n=list_len(data);
	char *databuff;

	buffsize=sizeof(int)+500;

	databuff=malloc(400);
	memset(databuff, 0, buffsize);
	
	memcpy(databuff, &(n), sizeof(int));
	bufflen = sizeof(int);
	while(data){
		buffsize+=strlen(data->data)+1;
		printf("-> %s %d <-\n", data->data, bufflen);
		//databuff = realloc(databuff, buffsize);
		//memset(databuff+bufflen, 0, buffsize-bufflen);
		memcpy(databuff+bufflen, data->data, strlen(data->data)+1);
		bufflen+=strlen(data->data)+1;
		data=data->next;
	}

	*buffer = databuff;
	
	return bufflen;
}

int old_pack(char **buffer, char *data[], int n)
{
	int buffsize, bufflen, i;
	char *databuff;
	buffsize=sizeof(int)+n;
	for(i=0;i<n;i++)
		buffsize += strlen(data[i]);

	databuff = malloc(buffsize);
	memset(databuff, 0, buffsize);

	memcpy(databuff, &(n), sizeof(int));
	bufflen = sizeof(int);

	for(i=0; i<n; i++){
		memcpy(databuff + bufflen, data[i],
		       strlen(data[i]) + 1);
		bufflen += strlen(data[i]) + 1;
	}

	*buffer = databuff;
	return bufflen;
}

void unpack(char *databuff, char *data[])
{
	char *buffer, i, l, n;
	buffer = databuff;
	n = *((int *)databuff);
	l=sizeof(int);
	for(i=0; i<n; i++){
		//printf("%s\n", data[i]);
		data[i] = buffer + l;
		l+=(strlen(data[i])+1);
	}
}

int get_nfields(char *databuff)
{
	return *((int *)databuff);
}
*/
int open_database(void)
{
	u_int32_t open_flags;
	int ret;

	ret = db_create(&ilenia_dbports, NULL, 0);
	ret = db_create(&dbdeps, NULL, 0);

	ilenia_dbports->set_flags(ilenia_dbports, DB_DUPSORT);
	dbdeps->set_flags(dbdeps, DB_DUPSORT);

	open_flags = DB_CREATE;
	ret = ilenia_dbports->open(ilenia_dbports, NULL, "cache.db", NULL, 
				   DB_BTREE, open_flags, 0);
	ret = dbdeps->open(dbdeps, NULL, "deps.db", NULL, DB_BTREE, open_flags, 
			   0);

	return ret;
}

int safe_exit(int ret)
{
	ilenia_dbports->close(ilenia_dbports, 0);
	dbdeps->close(dbdeps, 0);
	return ret;
}

void dbports_put(DB_PACKAGE pkg)
{
	DBT key, data;
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = &(pkg.name);
	key.size = strlen(pkg.name)+1;
	data.data = &pkg;
	data.size = sizeof(DB_PACKAGE);
	ilenia_dbports->put(ilenia_dbports, NULL, &key, &data, 0);
}

void dbdeps_put(char *d_key, int n_deps, char deps[][MAXFIELD])
{
	DBT key, data;
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = d_key;
	key.size = strlen(d_key)+1;
	data.data = deps;
	data.size = 255*n_deps+n_deps;
	dbdeps->put(dbdeps, NULL, &key, &data, 0);
}

void dbports_insert(struct pkglist *p)
{
	int i;
	DB_PACKAGE pkg;

	strncpy(pkg.name, p->name, strlen(p->name)+1);
	strncpy(pkg.version, p->version, strlen(p->version)+1);
	strncpy(pkg.repo, p->repo, strlen(p->repo)+1);

	pkg.n_deps = list_len(p->depends);

	char deps[pkg.n_deps][MAXFIELD];
	i=0;
	while(p->depends){
		strncpy(deps[i], p->depends->data, 
			strlen(p->depends->data)+1);
		p->depends=p->depends->next;
		i++;
	}

	snprintf(pkg.d_key, MAXFIELD, "%s_%s", pkg.name, pkg.repo);

	dbports_put(pkg);
	dbdeps_put(pkg.d_key, pkg.n_deps, deps);
}

struct list *get_deps(char *d_key, int n_deps)
{
	int i;
	struct list *d = NULL;
	DBT key, data;
	char deps[n_deps][MAXFIELD];
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = d_key;
	key.size = strlen(d_key)+1;
	data.data = &deps;
	data.ulen = n_deps*MAXFIELD+n_deps;;
	data.flags = DB_DBT_USERMEM;
	if(dbdeps->get(dbdeps, NULL, &key, &data, 0))
		return NULL;
	for(i=0; i<n_deps; i++)
		d=list_add(deps[i], d);
	return d;
}

struct pkglist *db_get(char *name)
{
	int ret, flag;
	struct pkglist *p=NULL;
	struct list *d=NULL;
	DBC *cursor;
	DBT key, data;
	DB_PACKAGE pkg;

	ilenia_dbports->cursor(ilenia_dbports, NULL, &cursor, 0);
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = name;
	key.size = strlen(name)+1;
	data.data = &pkg;
	data.ulen = sizeof(DB_PACKAGE);
	data.flags = DB_DBT_USERMEM;

	flag=DB_SET;
	while((ret = cursor->c_get(cursor, &key, &data, flag))==0){
		flag=DB_NEXT_DUP;
		d = NULL;
		d = get_deps(pkg.d_key, pkg.n_deps);
		p = pkglist_add(pkg.name, pkg.version, pkg.repo, d, p);
	}
	return p;
}
