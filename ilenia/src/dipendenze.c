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
#include "deplist.h"
#include "lspacchetti.h"
#include "lsports.h"
#include "manipola.h"
#include "confronto.h"
#include "output.h"
#include "ilenia.h"

struct db * dipendenze_pacchetto(char * pacchetto, char * collezione)
{
  /*
  char pkgfile[255];
  struct db * dipendenze=NULL;
  FILE * file;
  strcpy (pkgfile, "/usr/ports/");
  strcat (pkgfile, collezione);
  strcat (pkgfile, "/");
  strcat (pkgfile, pacchetto);
  strcat (pkgfile, "/Pkgfile");
  if ((file = fopen (pkgfile, "r")))	{
    char riga[MASSIMO] = "";
    char dep[MASSIMO] = "";
    while (fgets (riga, MASSIMO, file)) {
      if (riga[0] == '#') {
	strcpy (riga, mid (riga, 1, FINE));
	strcpy (riga, trim (riga));
	if (strncasecmp (riga, "Depends", 7) == 0) {
	  strcpy (riga, mid (strstr (riga, ":"), 1, FINE));
	  strcpy (dep, trim (riga));
	  break;
	}
      }
    }
    if (strlen (dep) > 0) {
      char tmp[MASSIMO];
      strcpy (dep, sed (dep, " ", ","));
      while (strlen (dep) > 0) {
	if (strstr (dep, ",")) {
	  strcpy (tmp, strstr (dep, ","));
	  strcpy (tmp, mid (dep, 0, strlen (dep) - strlen (tmp)));
	  strcpy (dep, mid (strstr (dep, ","), 1, FINE));
	} else {
	  strcpy (tmp, dep);
	  strcpy (dep, "");
	}
	strcpy (tmp, trim (tmp));
	if(strlen(tmp)>0)
	  dipendenze=inserisci_elemento_inverso(tmp,"", il_piu_aggiornato(tmp, ports),
						NULL, dipendenze);
	strcpy (riga, trim (riga));
      }
    }
    fclose(file);
  } else {
    dipendenze=inserisci_elemento(pacchetto, "", "not found", NULL, dipendenze);
  }
  return(dipendenze);
  */
  struct db * thispkg = NULL;
  struct db * dependencies = NULL;
  thispkg = cerca (pacchetto, ports);
  thispkg = cerca (collezione, thispkg);
  while (thispkg->depends!=NULL){
    dependencies=inserisci_elemento_inverso(thispkg->depends->pkg, "",
					    il_piu_aggiornato(thispkg->depends->pkg, 
							      ports), NULL, dependencies);
    thispkg->depends=thispkg->depends->next;
  }
  return(dependencies);
}

struct db * cerca_dipendenze(struct db *_pacchetti)
{
  struct db *dipendenze=NULL;
  while(_pacchetti!=NULL){
    struct db *d=NULL;
    d=dipendenze_pacchetto(_pacchetti->nome, _pacchetti->collezione);
    d=inserisci_elemento_inverso(_pacchetti->nome, "", _pacchetti->collezione, NULL, d);
    while(d!=NULL){
      if(esiste(d->nome, dipendenze)!=0)
	dipendenze=inserisci_elemento_inverso(d->nome, "", d->collezione, NULL, 
					      dipendenze);
      d=d->prossimo;
    }
    _pacchetti=_pacchetti->prossimo;
  }
  return(dipendenze);
}

struct db * dipendenze (char *pacchetto)
{
  struct db *d = NULL;
  char collezione[255];
  int i=-10;
  strcpy (collezione, il_piu_aggiornato (pacchetto, ports));
  d = inserisci_elemento_inverso (pacchetto, "", collezione, NULL, d);

  while (i != conta (d)) {
    i = conta (d);
    d = cerca_dipendenze (d);
  }
  return (d);
}

struct db * cerca_dipendenti(struct db *_pacchetti)
{
  struct db * dependents=NULL;
  dependents=_pacchetti;
  while(_pacchetti!=NULL){
    struct db * p=NULL;
    p=pacchetti;
    //printf("_pacchetti %s\n", _pacchetti->nome);
    while(p!=NULL){
      //printf("p %s\n", p->nome);
      char repo[255];
      strcpy(repo, il_piu_aggiornato(p->nome,  ports));
      //printf("repo %s\n", repo);
      struct db * tmp=NULL;
      if((tmp=cerca(repo, cerca(p->nome, ports)))){
	if(exists(_pacchetti->nome, tmp->depends)) {
	  if(cerca(p->nome, dependents)==NULL)
	    dependents=inserisci_elemento(p->nome, "", repo, NULL, dependents);
	}
      }
      p=p->prossimo;
    }
    _pacchetti=_pacchetti->prossimo;
  }
  return(dependents);
}

struct db * dipendenti (char *pacchetto, int all)
{
  struct db *d = NULL;
  char collezione[255];
  int i=-10;
  strcpy(collezione, il_piu_aggiornato(pacchetto, ports));
  d = inserisci_elemento(pacchetto, "", collezione, NULL, d);

  while(i != conta(d)){
    i=conta(d);
    d=cerca_dipendenti(d);
    if(all==0)
      return(d);
  }
  return(d);
}
 
void stampa_dipendenze (char *pacchetto)
{
  struct db *d = NULL;
  d = dipendenze (pacchetto);
  while (d != NULL) {
    if(strcmp(d->collezione, "not found")!=0) {
      printf("%s [", d->nome);
      if (esiste(d->nome, pacchetti)!=0)
	printf (" ]\n");
      else
	printf ("installed]\n");
    } else {
      printf("%s [not found]\n", d->nome);
    }
    d = d->prossimo;
  }
}

void stampa_dipendenti (char *pacchetto, int all)
{
  struct db *d = NULL;
  d = dipendenti (pacchetto, all);
  while (d != NULL) {
    if(strcmp(d->collezione, "not found")!=0) {
      printf ("%s\n", d->nome);
    }
    d = d->prossimo;
  }
}
