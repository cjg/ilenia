/***************************************************************************
 *            lsports.c
 *
 *  Sat Jul 10 12:57:55 2004
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
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include "manipola.h"
#include "db.h"
#include "deplist.h"

FILE *cachefile;

struct deplist *deplist_from_deprow(char *deprow){
  struct deplist *d=NULL;
  if (strlen (deprow) > 0) {
    char tmp[MASSIMO];
    strcpy (deprow, sed (deprow, " ", ","));
    while (strlen (deprow) > 0) {
      if (strstr (deprow, ",")) {
	strcpy (tmp, strstr (deprow, ","));
	strcpy (tmp, mid (deprow, 0, strlen (deprow) - strlen (tmp)));
	strcpy (deprow, mid (strstr (deprow, ","), 1, FINE));
      } else {
	strcpy (tmp, deprow);
	strcpy (deprow, "");
      }
      strcpy (tmp, trim (tmp));
      if(strlen(trim(tmp))>0)
	d=add (trim(tmp), d);
      strcpy (deprow, trim (deprow));
    }
  }
  return(d);    
}

struct db * parsa_pkgfile(char *percorso, char *collezione, struct db *p){
  FILE *pkgfile;
  if ((pkgfile = fopen (percorso, "r"))) {
    char riga[255] = "";
    char nome[255] = "";
    char versione[255] = "";
    int release = 0;
    struct deplist *d=NULL;
    while (fgets (riga, 255, pkgfile)) {
      strcpy (riga, trim (riga));
      if (riga[0] == '#') {
	strcpy (riga, mid (riga, 1, FINE));
	strcpy (riga, trim (riga));
	if (strncasecmp (riga, "Depends", 7) == 0) {
	  char deprow[MASSIMO];
	  if(strstr(riga, ":")){
	    strcpy (riga, mid (strstr (riga, ":"), 1, FINE));
	    strcpy (deprow, trim (riga));
	    d=deplist_from_deprow(deprow);
	  }
	}
      }
      if (strncmp (riga, "name", 4) == 0) {
	strcpy (nome, mid (riga, 5, strlen (riga) - 5));
      }
      if (strncmp (riga, "version", 7) == 0) {
	strcpy (versione, mid (riga, 8, strlen (riga) - 8));
      }
      if (strncmp (riga, "release", 7) == 0) {
	strcat (versione, "-");
	strcat (versione, mid (riga, 8, strlen (riga) - 8));
	release = 1;
      }
      if (strlen (nome) && strlen (versione) && release) {
	p = inserisci_elemento_ordinato (nome, versione, collezione, d, p);
	char dependencies[MASSIMO];
	strcpy(dependencies, "");
	if(d!=NULL){
	  while(d!=NULL){
	    strcat(dependencies, d->pkg);
	    strcat(dependencies, " ");
	    d=d->next;
	  }
	}
	fprintf (cachefile, "%s %s %s %s\n", nome, versione, collezione, dependencies);
	strcpy (nome, "");
	strcpy (versione, "");
	release = 0;
      }
    }
    fclose (pkgfile);
  }
  return(p);
}

struct db * parsa_cvsup (char *percorso) {
  FILE *file;
  struct db * p=NULL;
  if((file=fopen(percorso, "r"))){
    char riga[255];
    char prefix[255];
    char collezione[255];
    char pre_collezione[255];
    while (fgets (riga, 255, file)) {
      strcpy(riga, trim(riga));
      if(strncmp(riga, "*default prefix=", 16)==0){
	strcpy(prefix, mid(riga, 16, FINE));
	if(strncmp(prefix,"/usr/ports/",11)==0){
	  strcpy(pre_collezione, mid(prefix, 11,FINE));
	  strcpy(prefix, "/usr/ports/");
	  if(pre_collezione[strlen(pre_collezione)]!='/')
	    strcat(pre_collezione, "/");
	}
      }
      if(riga[0]!='#' && riga[0]!='*' && strlen(riga)>0){
	if(strstr(riga, " "))
	  strcpy(riga, mid(riga, 0, strlen(riga)-strlen(strstr(riga, " "))));
	strcpy(collezione, pre_collezione);
	strcat(collezione, riga);
	p=inserisci_elemento_ordinato(prefix, collezione, "", NULL, p);
      }
    }
  }
  return(p);
}

struct db * parsa_httpup (char *percorso){
  FILE *file;
  struct db * p=NULL;
  if((file=fopen(percorso, "r"))){
    char riga[255];
    char prefix[255];
    char collezione[255];
    char pre_collezione[255];
    while (fgets (riga, 255, file)) {
      strcpy(riga, trim(riga));
      if(strncmp(riga, "ROOT_DIR=", 9)==0){
	strcpy(prefix, mid(riga, 9, FINE));
	strcpy(prefix, mid(prefix, 0, strlen(prefix)-strlen(rindex(prefix, '/'))));
	if(strncmp(prefix,"/usr/ports/",11)==0){
	  strcpy(pre_collezione, mid(prefix, 11,FINE));
	  strcpy(prefix, "/usr/ports/");
	  if(pre_collezione[strlen(pre_collezione)]!='/')
	    strcat(pre_collezione, "/");
	}
	strcpy(riga, rindex(riga, '/'));
	strcpy(riga, mid(riga, 1, FINE));
	strcpy(collezione, pre_collezione);
	strcat(collezione, riga);
	p=inserisci_elemento_ordinato(prefix, collezione, "", NULL, p);
      }
    }
  }
  return(p);
}

struct db * leggi_dir(char *collezione, char *prefix, struct db *p){
  DIR *dir;
  struct dirent *info_file;
  struct stat tipo_file; 
  char percorso[255];
  char nome_file[255];
  strcpy(percorso, prefix);
  strcat(percorso, "/");
  strcat(percorso, collezione);
  if((dir=opendir(percorso))){
    while ((info_file = readdir (dir))) {
      strcpy (nome_file, prefix);
      strcat (nome_file, "/");
      strcat (nome_file, collezione);
      strcat (nome_file, "/");
      strcat (nome_file, info_file->d_name);
      stat (nome_file, &tipo_file);
      if (S_ISDIR (tipo_file.st_mode) && info_file->d_name[0] != '.') {
	strcat(nome_file, "/Pkgfile");
	p=parsa_pkgfile(nome_file, collezione, p);
      }
    }
  }
  return(p);
}

struct db * lsports_acrux_way () {
  DIR *etc_ports;
  struct dirent *info_file;
  struct db *p=NULL;
  struct db *ports=NULL;
  char nome_file[255];
  char estensione[255];
  if (!(cachefile = fopen ("/var/cache/ilenia", "w"))) {
    return ports;
  }
  etc_ports = opendir ("/etc/ports");
  while ((info_file = readdir (etc_ports))) {
    if (strstr (info_file->d_name, ".")) {
      strcpy (estensione, strstr (info_file->d_name, "."));
      strcpy (estensione, mid (estensione, 1, FINE));
      strcpy (nome_file, "/etc/ports/");
      strcat (nome_file, info_file->d_name);
      if (strcmp (estensione, "cvsup") == 0) {
	p=parsa_cvsup(nome_file);
	while(p!=NULL){
	  ports=leggi_dir(p->versione, p->nome, ports);
	  p=p->prossimo;
	}
      }	else if (strcmp (estensione, "httpup") == 0) {
	p=parsa_httpup(nome_file);
	ports=leggi_dir(p->versione, p->nome, ports);
      }
    }
  }
  fclose(cachefile);
  chmod("/var/cache/ilenia", S_IREAD | S_IWRITE | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  return(ports);
}

struct db * lsports ()
{
  FILE *cachefile;
  struct db *p = NULL;
  char riga[255];
    
  if ((cachefile = fopen ("/var/cache/ilenia", "r"))) {
    if(fgets (riga, 255, cachefile)){
      while (fgets (riga, 255, cachefile)) {
	int len;
	char splitted_row[MASSIMO][MASSIMO];
	len = split(riga, " ", splitted_row);
	int i;
	struct deplist * d=NULL;
	for(i=3;i<len;i++){
	  if(strlen(trim(splitted_row[i]))>0)
	    d=add(trim(splitted_row[i]), d);
	}
	p = inserisci_elemento_ordinato (trim(splitted_row[0]), trim(splitted_row[1]), 
					 trim(splitted_row[2]), d, p);
      }
      fclose(cachefile);
      return(p);
    }
  }
  printf("Building cache!\n");
  p = lsports_acrux_way ();
  return(p);
}
