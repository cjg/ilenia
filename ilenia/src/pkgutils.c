/***************************************************************************
 *            pkgutils.c
 *
 *  Wed Sep  1 18:55:42 2004
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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "db.h"
#include "manipola.h"
#include "confronto.h"
#include "dipendenze.h"
#include "ilenia.h"
#include "lsports.h"
#include "lspacchetti.h"

int esegui_script (char *path, char *script) 
{
  pid_t pid = fork ();
  int stato;
  if (pid == 0){
    chdir(path);
    execl ("/bin/bash", "", script, 0);
  } else if (pid < 0) {
    stato = -1;
  } else {
    while ((waitpid (pid, &stato, WNOHANG) == 0))
      {
      }
  }
  return (stato);
}

int compila_e (int aggiorna, char *port)
{
  int stato;
  char azione[255];
  char install_script[255];
  FILE *file;
  if (aggiorna) {
    strcpy (azione, "-u");
  } else {
    strcpy (azione, "-i");
  }
  strcpy (install_script, port);
  strcat (install_script, "/pre-install");
  if ((file = fopen (install_script, "r"))) {
    fclose (file);
    if (esegui_script (port, "pre-install") != 0)
      return (-1);
  }
  
  pid_t pid = fork ();
  if (pid == 0) {
    chdir (port);
    execl ("/usr/bin/pkgmk", "", "-d", "-f",azione, 0);
  } else if (pid < 0) {
    stato = -1;
  } else {
    while ((waitpid (pid, &stato, WNOHANG) == 0))
      {
      }
  }
  if(stato!=0)
    return(stato);
  strcpy (install_script, port);
  strcat (install_script, "/post-install");
  if ((file = fopen (install_script, "r"))) {
    fclose (file);
    if (esegui_script (port, "post-install") != 0)
      return (-1);
  }
  return (0);
}

int aggiorna_pacchetto_ (int opzioni_confronto, char *pacchetto)
{
  int aggiornare = 0;
  char collezione[255];
  char port[255];
  struct db *p;
  if (esiste (pacchetto, pacchetti)==0)
    aggiornare = 1;
  
  strcpy (collezione, il_piu_aggiornato (pacchetto, ports));

  if (opzioni_confronto != NO_REPO && opzioni_confronto != NO_FAVORITE) {
    p = prendi_favorite (REPO);
    if ((p = cerca (pacchetto, p))) {
      strcpy (collezione, p->collezione);
    }
  }
  
  if (opzioni_confronto != NO_VERSION && opzioni_confronto != NO_FAVORITE) {
    p = prendi_favorite (VERSIONE);
    if ((p = cerca (pacchetto, p))) {
      strcpy (collezione, questa_versione (pacchetto, p->versione, ports));
    }
  }
  
  strcpy (port, "/usr/ports/");
  strcat (port, collezione);
  strcat (port, "/");
  strcat (port, pacchetto);
  return (compila_e (aggiornare, port));
}


int aggiorna_pacchetto (int opzioni_confronto, char *pacchetto)
{
  if (getuid () != 0) {
    printf ("ilenia: only root can update or install packages\n\n");
    return (-1);
  }
  struct db *d = NULL;
  if(opzioni_confronto>=0){
    d = dipendenze (pacchetto);
    while (d->prossimo != NULL) {
      printf ("%s [", d->nome);
      if(strcmp(d->collezione, "not found")!=0){
	if (esiste (d->nome, pacchetti)!=0) {
	  printf ("install now]\n");
	  if (aggiorna_pacchetto_ (opzioni_confronto, d->nome) != 0)
	    return (-1);
	} else {
	  printf ("installed]\n");
	}
      } else {
	printf("not found]\n");
      }
      d = d->prossimo;
    }
    if (strcmp(d->collezione, "not found")!=0) {
      if (aggiorna_pacchetto_ (opzioni_confronto, d->nome) != 0)
	return (-1);
    } else {
      printf("%s [not found]\n", d->nome);
      return(-1);
    }
  } else {
    opzioni_confronto*=-1;
    if (esiste(pacchetto, ports)==0) {
      if (aggiorna_pacchetto_ (opzioni_confronto, pacchetto) != 0)
	return (-1);
    } else {
      printf("%s [not found]\n", pacchetto);
      return(-1);
    }
  }
  return (0);
}


int aggiorna_pacchetti (int opzioni_confronto)
{
  if (getuid () != 0) {
    printf ("ilenia: only root can update or install packages\n\n");
    return (-1);
  }
  struct db *p = NULL;
  p = confronta (pacchetti, ports, AGGIORNATI, opzioni_confronto, 0);
  while (p != NULL) {
    printf ("%s\n", p->nome);
    if (aggiorna_pacchetto_ (opzioni_confronto, p->nome) != 0)
      return (-1);
    p = p->prossimo;
  }
  return (0);
}

int do_pkgrm (char *pkg)
{
  int stato;
  pid_t pid = fork ();
  if (pid == 0) {
    printf("Removing %s\n", pkg);
    execl ("/usr/bin/pkgrm", "pkgrm", pkg, 0);
  } else if (pid < 0) {
    stato = -1;
  } else {
    while ((waitpid (pid, &stato, WNOHANG) == 0))
      {
      }
  }
  if(stato!=0)
    return(stato);
  return(0);
}

int pkgrm (char *pkg, int nocheckdeps, int removeall)
{
  if (getuid () != 0) {
    printf ("ilenia: only root can remove packages\n\n");
    return(-1);
  }
  struct db *p = NULL; 
  p=dipendenti(pkg, 0);
  if(removeall){
    while(p!=NULL){
      do_pkgrm(p->nome);
      p=p->prossimo;
    }
    return(0);
  }
  if(conta(p)>1){
    if(nocheckdeps){
      do_pkgrm(pkg);
      return(0);
    } else {
      printf("ilenia: there are some packages that depends from %s, use --remove-all or --no-deps, to remove all packages that depends from %s or to not check dependencies (use at your risk)\nYou can use ilenia -T %s to see a list of the packages that need %s.\n", pkg, pkg, pkg, pkg);
      return(-1);
    }
  }
  do_pkgrm(pkg);
  return(0);
} 
