#include <stdio.h>
#include <string.h>
#include "db.h"
#include "lspacchetti.h"
#include "lsports.h"
#include "confronto.h"
#include "aggiorna.h"
#include "output.h"
#include "pkgutils.h"
#include "dipendenze.h"
#include "ilenia.h"

#define AGGIORNA	1
#define LSPORTS		2
#define AIUTO		6
#define AGGIORNA_P	7

int main (int argc, char *argv[])
{
  if (argc < 2) {
    aiuto ();
    return (0);
  }
  int i;
  int azione[argc];
  char opzione[argc][255];
  char opzione_p[argc][255];
  int azioni = -1;
  int opzioni = -1;
  int opzioni_p = -1;
  int opzioni_confronto = 0;
  for (i = 1; i < argc; i++) {
    if (strcmp (argv[i], "-u") == 0 || strcmp (argv[i], "--update") == 0) {
      azioni++;
      azione[azioni] = AGGIORNA;
    } else if (strcmp (argv[i], "-l") == 0 || strcmp (argv[i], "--list") == 0) {
      azioni++;
      azione[azioni] = LSPORTS;
    } else if (strcmp (argv[i], "-d") == 0 || strcmp (argv[i], "--diff") == 0) {
      azioni++;
      azione[azioni] = DIFFERENZE;
    } else if (strcmp (argv[i], "-p") == 0 || strcmp (argv[i], "--updated") == 0) {
      azioni++;
      azione[azioni] = AGGIORNATI;
    } else if (strcmp (argv[i], "-v") == 0 || strcmp (argv[i], "--version") == 0) {
      azioni++;
      azione[azioni] = VERSIONE;
    } else if (strcmp (argv[i], "-h") == 0 || strcmp (argv[i], "--help") == 0) {
      azioni++;
      azione[azioni] = AIUTO;
    } else if (strcmp (argv[i], "--no-favorite-repo") == 0) {
      opzioni_confronto += NO_REPO;
    } else if (strcmp (argv[i], "--no-favorite-version") == 0) {
      opzioni_confronto += NO_VERSION;
    } else if (strcmp (argv[i], "-U") == 0) {
      azioni++;
      azione[azioni] = AGGIORNA_P;
    } else if (strcmp (argv[i], "-z") == 0) {
      //dipendenze (argv[i + 1]);
      //printf("%s\n", sed(argv[i+1], '-', 'Q'));
      //struct db *a=NULL;
      //a=lspacchetti();
      //print_db(a);
      //a=rimuovi_elemento("xchat",a);
      //print_db(a);
      stampa_dipendenze(argv[i+1]);
      //printf("%s\n", il_piu_aggiornato(argv[i+1], lsports()));
      return (0);
    } else {
      if (azione[azioni] == AGGIORNA) {
	opzioni++;
	strcpy (opzione[opzioni], argv[i]);
      } else if (azione[azioni] == AGGIORNA_P) {
	opzioni_p++;
	strcpy (opzione_p[opzioni_p], argv[i]);
      }
    }
  }

  ports=lsports();
  pacchetti=lspacchetti();

  for (i = 0; i <= azioni; i++)	{
    switch (azione[i]) {
    case AGGIORNA_P:
      if (opzioni_p != -1) {
	int j;
	for (j = 0; j <= opzioni_p; j++) {
	  aggiorna_pacchetto (opzioni_confronto, opzione_p[j]);
	}
      }	else {
	aggiorna_pacchetti (opzioni_confronto);
      }
      break;
    case AGGIORNA:
      if (opzioni < 0) {
	aggiorna_ports ();
      }	else {
	int j;
	for (j = 0; j <= opzioni; j++) {
	  aggiorna_collezione (opzione[j]);
	}
      }
      break;
    case LSPORTS:
      print_db (ports);
      break;
    case DIFFERENZE:
      confronta (pacchetti, ports, DIFFERENZE, opzioni_confronto, 1);
      break;
    case AGGIORNATI:
      confronta (pacchetti, ports, AGGIORNATI, opzioni_confronto, 1);
      break;
    case VERSIONE:
      versione ();
      break;
    case AIUTO:
      aiuto ();
      break;
    }
  }
  return (0);
}
