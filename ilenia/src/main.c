/* Created by Anjuta version 1.2.2 */
/*	This file will not be overwritten */

#include <stdio.h>
#include <string.h>
#include "db.h"
#include "lspacchetti.h"
#include "lsports.h"
#include "confronto.h"
#include "aggiorna.h"
#include "output.h"

#define AGGIORNA	1
#define LSPORTS		2
#define VERSIONE	5
#define AIUTO		6

int main (int argc, char *argv[]) {
  if (argc < 2) {
    aiuto ();
    return (0);
  }
  int i;
  int azione[argc];
  char opzione[argc][255];
  int azioni = -1;
  int opzioni = -1;
  int opzioni_confronto = 0;
  for (i = 1; i < argc; i++) {
    if (strcmp (argv[i], "-u") == 0 || strcmp (argv[i], "--update") == 0) {
      azioni++;
      azione[azioni] = AGGIORNA;
    }
    else if (strcmp (argv[i], "-l") == 0 || strcmp (argv[i], "--list") == 0) {
      azioni++;
      azione[azioni] = LSPORTS;
    }
    else if (strcmp (argv[i], "-d") == 0 || strcmp (argv[i], "--diff") == 0) {
      azioni++;
      azione[azioni] = DIFFERENZE;
    }
    else if (strcmp (argv[i], "-p") == 0 || strcmp (argv[i], "--updated") == 0) {
      azioni++;
      azione[azioni] = AGGIORNATI;
    }
    else if (strcmp (argv[i], "-v") == 0 || strcmp (argv[i], "--version") == 0)	{
      azioni++;
      azione[azioni] = VERSIONE;
    }
    else if (strcmp (argv[i], "-h") == 0 || strcmp (argv[i], "--help") == 0) {
      azioni++;
      azione[azioni] = AIUTO;
    }
    else if (strcmp (argv[i], "--no-favorite-repo") == 0) {
      opzioni_confronto += NO_REPO;
    }
    else if (strcmp (argv[i], "--no-favorite-version") == 0) {
      opzioni_confronto += NO_VERSION;
    }
    else {
      opzioni++;
      strcpy (opzione[opzioni], argv[i]);
    }
  }
  
  for (i = 0; i <= azioni; i++)	{
    switch (azione[i]) {
    case AGGIORNA:
      if (opzioni < 0) {
	aggiorna_ports ();
      }
      else {
	int j;
	for (j = 0; j <= opzioni; j++) {
	  aggiorna_collezione (opzione[j]);
	}
      }
      break;
    case LSPORTS:
      print_db (lsports ());
      break;
    case DIFFERENZE:
      confronta (lspacchetti (), lsports (), DIFFERENZE, opzioni_confronto);
      break;
    case AGGIORNATI:
      confronta (lspacchetti (), lsports (), AGGIORNATI, opzioni_confronto);
      break;
    case VERSIONE:
      versione();
      break;
    case AIUTO:
      aiuto ();
      break;
    }
  }
  return (0);
}
