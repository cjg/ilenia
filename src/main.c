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
#define DIPENDENZE      8
#define RIMUOVI         9

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
  char opzione_d[argc][255];
  char opzione_l[argc][255];
  char opzione_r[argc][255];
  int azioni = -1;
  int opzioni = -1;
  int opzioni_p = -1;
  int opzioni_d = -1;
  int opzioni_l = -1;
  int opzioni_r = -1;
  int opzioni_confronto = 0;
  int controlla_dipendenze=0;
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
    } else if (strcmp (argv[i], "--no-deps") == 0) {
      controlla_dipendenze = NODEPS;
    } else if (strcmp (argv[i], "-U") == 0) {
      azioni++;
      azione[azioni] = AGGIORNA_P;
    } else if (strcmp (argv[i], "-D") == 0) {
      azioni++;
      azione[azioni]=DIPENDENZE;
    } else if (strcmp (argv[i], "-R") == 0) {
      azioni++;
      azione[azioni]=RIMUOVI;
    } else {
      if (azione[azioni] == AGGIORNA) {
	opzioni++;
	strcpy (opzione[opzioni], argv[i]);
      } else if (azione[azioni] == AGGIORNA_P) {
	opzioni_p++;
	strcpy (opzione_p[opzioni_p], argv[i]);
      } else if (azione[azioni] == DIPENDENZE) {
	opzioni_d++;
	strcpy (opzione_d[opzioni_d], argv[i]);
      } else if (azione[azioni] == LSPORTS) {
	opzioni_l++;
	strcpy (opzione_l[opzioni_l], argv[i]);
      } else if (azione[azioni] == RIMUOVI) {
	opzioni_r++;
	strcpy (opzione_r[opzioni_r], argv[i]);
      }
    } 
  }

  if(azioni<0){
    aiuto();
    return(0);
  }
  
  ports=lsports();
  pacchetti=lspacchetti();

  for (i = 0; i <= azioni; i++)	{
    switch (azione[i]) {
    case DIPENDENZE:
      if (opzioni_d != -1) {
	int j;
	for (j = 0; j <= opzioni_d; j++) {
	  stampa_dipendenze (opzione_d[j]);
	}
      }	else {
	printf("pacchetto\n");
      }
      break;
    case AGGIORNA_P:
      if(opzioni_confronto)
	opzioni=opzioni_confronto*controlla_dipendenze;
      else
	opzioni=controlla_dipendenze;
      if (opzioni_p != -1) {
	int j;
	/*int opz;
	if(opzioni_confronto)
	  opz=opzioni_confronto*controlla_dipendenze;
	else
	  opz=controlla_dipendenze;
	*/
	for (j = 0; j <= opzioni_p; j++) {
	  aggiorna_pacchetto (opzioni, opzione_p[j]);
	}
      }	else {
	aggiorna_pacchetti (opzioni);
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
      if(opzioni_l==-1)
	print_db (ports);
      else {
	struct db *p=NULL;
	int j;
	for (j = 0; j <= opzioni_l; j++) {
	  p=cerca(opzione_l[j], ports);
	  print_db(p);
	}
      }
      break;
    case DIFFERENZE:
      confronta (pacchetti, ports, DIFFERENZE, opzioni_confronto, 1);
      break;
    case RIMUOVI:
      if (opzioni_r != -1) {
	int j;
	for (j = 0; j <= opzioni_r; j++) {
	  stampa_dipendenti (opzione_r[j]);
	}
      }	else {
	printf("pacchetto\n");
      }
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
