#include <stdio.h>
#include <string.h>
#include "pkglist.h"
#include "deplist.h"
#include "lspacchetti.h"
#include "lsports.h"
#include "confront.h"
#include "update.h"
#include "output.h"
#include "pkgutils.h"
#include "dipendenze.h"
#include "repolist.h"
#include "aliaslist.h"
#include "ilenia.h"

#define AGGIORNA	1
#define LSPORTS		2
#define AIUTO		6
#define AGGIORNA_P	7
#define DIPENDENZE      8
#define RIMUOVI         9
#define DEPENDENT      10
#define REPOLIST       11
#define SEARCH         12

int
main (int argc, char *argv[])
{
  if (argc < 2)
    {
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
  char opzione_t[argc][255];
  char opzione_s[argc][255];
  int azioni = -1;
  int opzioni = -1;
  int opzioni_p = -1;
  int opzioni_d = -1;
  int opzioni_l = -1;
  int opzioni_r = -1;
  int opzioni_t = -1;
  int opzioni_s = -1;
  int opzioni_confronto = 0;
  int controlla_dipendenze = 0;
  int all = 0;
  int rebuild_cache = 0;
  for (i = 1; i < argc; i++)
    {
      if (strcmp (argv[i], "-u") == 0 || strcmp (argv[i], "--update") == 0)
	{
	  azioni++;
	  azione[azioni] = AGGIORNA;
	}
      else if (strcmp (argv[i], "-l") == 0 || strcmp (argv[i], "--list") == 0)
	{
	  azioni++;
	  azione[azioni] = LSPORTS;
	}
      else if (strcmp (argv[i], "-s") == 0
	       || strcmp (argv[i], "--search") == 0)
	{
	  azioni++;
	  azione[azioni] = SEARCH;
	}
      else if (strcmp (argv[i], "-d") == 0 || strcmp (argv[i], "--diff") == 0)
	{
	  azioni++;
	  azione[azioni] = DIFF;
	}
      else if (strcmp (argv[i], "-p") == 0
	       || strcmp (argv[i], "--updated") == 0)
	{
	  azioni++;
	  azione[azioni] = UPDATED;
	}
      else if (strcmp (argv[i], "-v") == 0
	       || strcmp (argv[i], "--version") == 0)
	{
	  azioni++;
	  azione[azioni] = VERSIONE;
	}
      else if (strcmp (argv[i], "-h") == 0 || strcmp (argv[i], "--help") == 0)
	{
	  azioni++;
	  azione[azioni] = AIUTO;
	}
      else if (strcmp (argv[i], "--repository-list") == 0)
	{
	  azioni++;
	  azione[azioni] = REPOLIST;
	}
      else if (strcmp (argv[i], "--no-favorite-repo") == 0)
	{
	  opzioni_confronto += NO_FAVORITE_REPO;
	}
      else if (strcmp (argv[i], "--no-favorite-version") == 0)
	{
	  opzioni_confronto += NO_FAVORITE_VERSION;
	}
      else if (strcmp (argv[i], "--no-deps") == 0)
	{
	  controlla_dipendenze = NODEPS;
	}
      else if (strcmp (argv[i], "--all") == 0)
	{
	  all = 1;
	}
      else if (strcmp (argv[i], "-U") == 0)
	{
	  azioni++;
	  azione[azioni] = AGGIORNA_P;
	}
      else if (strcmp (argv[i], "-D") == 0)
	{
	  azioni++;
	  azione[azioni] = DIPENDENZE;
	}
      else if (strcmp (argv[i], "-R") == 0)
	{
	  azioni++;
	  azione[azioni] = RIMUOVI;
	}
      else if (strcmp (argv[i], "-T") == 0)
	{
	  azioni++;
	  azione[azioni] = DEPENDENT;
	}
      else if (strcmp (argv[i], "--cache") == 0)
	{
	  azioni++;
	  rebuild_cache = 1;
	}
      else
	{
	  if (azione[azioni] == AGGIORNA)
	    {
	      opzioni++;
	      strcpy (opzione[opzioni], argv[i]);
	    }
	  else if (azione[azioni] == AGGIORNA_P)
	    {
	      opzioni_p++;
	      strcpy (opzione_p[opzioni_p], argv[i]);
	    }
	  else if (azione[azioni] == DIPENDENZE)
	    {
	      opzioni_d++;
	      strcpy (opzione_d[opzioni_d], argv[i]);
	    }
	  else if (azione[azioni] == LSPORTS)
	    {
	      opzioni_l++;
	      strcpy (opzione_l[opzioni_l], argv[i]);
	    }
	  else if (azione[azioni] == SEARCH)
	    {
	      opzioni_s++;
	      strcpy (opzione_s[opzioni_s], argv[i]);
	    }
	  else if (azione[azioni] == RIMUOVI)
	    {
	      opzioni_r++;
	      strcpy (opzione_r[opzioni_r], argv[i]);
	    }
	  else if (azione[azioni] == DEPENDENT)
	    {
	      opzioni_t++;
	      strcpy (opzione_t[opzioni_t], argv[i]);
	    }
	}
    }

  if (azioni < 0)
    {
      aiuto ();
      return (0);
    }
  if (parse_ileniarc () != 0)
    return (1);

  repository = build_repolist ();
  if (rebuild_cache)
    {
      FILE *file;
      if ((file = fopen (CACHE, "w")))
	fclose (file);
    }

  aliases = aliaseslist_build ();
  ports = lsports ();
  pacchetti = lspacchetti ();

  for (i = 0; i <= (azioni - rebuild_cache); i++)
    {
      switch (azione[i])
	{
	case REPOLIST:
	  while (repository != NULL)
	    {
	      printf ("%s\n", repository->name);
	      repository = repository->next;
	    }
	  break;
	case DIPENDENZE:
	  if (opzioni_d != -1)
	    {
	      int j;
	      for (j = 0; j <= opzioni_d; j++)
		{
		  stampa_dipendenze (opzione_d[j]);
		}
	    }
	  else
	    {
	      printf ("pacchetto\n");
	    }
	  break;
	case AGGIORNA_P:
	  if (opzioni_confronto)
	    opzioni = opzioni_confronto * controlla_dipendenze;
	  else
	    opzioni = controlla_dipendenze;
	  if (opzioni_p != -1)
	    {
	      int j;
	      for (j = 0; j <= opzioni_p; j++)
		{
		  aggiorna_pacchetto (opzioni, opzione_p[j]);
		}
	    }
	  else
	    {
	      aggiorna_pacchetti (opzioni);
	    }
	  break;
	case AGGIORNA:
	  if (opzioni < 0)
	    {
	      update_all_repos ();
	    }
	  else
	    {
	      int j;
	      for (j = 0; j <= opzioni; j++)
		{
		  update_repo (opzione[j]);
		}
	    }
	  break;
	case LSPORTS:
	  if (opzioni_l == -1)
	    print_db (ports);
	  else
	    {
	      int j;
	      for (j = 0; j <= opzioni_l; j++)
		{
		  if (repolist_exists (opzione_l[j], repository))
		    print_db (pkglist_select_from_repo (opzione_l[j], ports));
		}
	    }
	  break;
	case SEARCH:
	  if (opzioni_s == -1)
	    printf ("What can I search?\n");
	  else
	    {
	      struct pkglist *p = NULL;
	      int j;
	      for (j = 0; j <= opzioni_s; j++)
		{
		  p = pkglist_find_like (opzione_s[j], ports);
		  print_db (p);
		}
	    }
	  break;
	case DIFF:
	  pkglist_confront (pacchetti, ports, DIFF, opzioni_confronto, 1);
	  break;
	case RIMUOVI:
	  if (opzioni_r != -1)
	    {
	      int j;
	      for (j = 0; j <= opzioni_r; j++)
		{
		  pkgrm (opzione_r[j], controlla_dipendenze, all);
		}
	    }
	  else
	    {
	      printf ("package(s)\n");
	    }
	  break;
	case DEPENDENT:
	  if (opzioni_t != -1)
	    {
	      int j;
	      for (j = 0; j <= opzioni_t; j++)
		{
		  stampa_dipendenti (opzione_t[j], all);
		}
	    }
	  else
	    {
	      printf ("package(s)\n");
	    }
	  break;
	case UPDATED:
	  pkglist_confront (pacchetti, ports, UPDATED, opzioni_confronto, 1);
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
