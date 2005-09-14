/*
 *  update-repos.c
 *  Wed Sep 14 11:40:57 2005
 *  Copyright 2005 - 2006 Coviello Giuseppe
 *  immigrant@email.it
 *
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

#define _GNU_SOURCE

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include "list.h"
#define ILENIA_CONF "/etc/ilenia.conf"


char *
trim (char *s)
{
  int n;
  while (*s && *s <= 32)
    ++s;
  for (n = strlen (s) - 1; n >= 0 && s[n] <= 32; --n)
    s[n] = 0;
  return s;
}

char *
mid (char *s, int start, int length)
{
  char *local_s;
  local_s = strdup(s);
  int x, z = 0;
  if (length == -1)
    length = strlen (local_s) - start;
  if ((start + length) <= strlen (local_s))
    {
      for (x = 0; x <= strlen (local_s); x++)
	{
	  if ((x >= start))
	    {
	      local_s[z] = local_s[x];
	      z++;
	    }
	}
    }
  local_s[length] = '\0';
  return (local_s);
}

int
split (char *s, char *delim, char *splitted[])
{
  char *local_s;
  char *tmp;
  local_s = strdup(s);
  int i = 0;
  while (strlen (local_s) > 0)
    {
      tmp = strdup(local_s);
      strtok (tmp, delim);
      
      splitted[i] = strdup(tmp);
      splitted[i] = trim(splitted[i]);
      if(splitted[i][0] == delim[0])
	{
	  splitted[i] = mid(strdup(splitted[i]), 1, -1);
	  splitted[i] = trim(splitted[i]);
	}
      local_s = mid (local_s, strlen (tmp), -1);
      i++;
    }
  return (i);
}

int count_separators (char *s)
{
  int c = 0;
  char *tmp = strdup(s);
  while((tmp=strchr(tmp, ',')))
    {
      c++;
      tmp = index(tmp, ' ');
    }
  return(c);
}

struct list *
string_to_list (char *s)
{
  struct list *l = NULL;
  char *tmp = index(s, '=');
  tmp = trim(index(tmp, ' '));
  char *splits[count_separators(tmp)+1];
  int i, n = split(tmp, ",", splits);
  for(i=0;i<n;i++)
      l = add(splits[i], l);
  return (l);
}


struct list *
get_updaters ()
{
  struct list *l = NULL;
  FILE *conf;
  if ((conf = fopen (ILENIA_CONF, "r")))
    {
      size_t n = 0;
      char *line = NULL;
      int nread = getline (&line, &n, conf);

      while (nread>0)
	{
	  line = trim(line);
	  if(strncmp (line, "updaters", 8) == 0)
	      l = string_to_list(line);
	  if(nread>1)
	    free(line);
	  n = 0;
	  nread = getline (&line, &n, conf);
	}
    }
  else
    printf ("Warning you don't have a ilenia.conf file.\n");
  return (l);
}



int main(int argc, char *argv)
{
  struct list *l = NULL;
  struct passwd *p = NULL;
  l = get_updaters();
  p = getpwuid(getuid());
  if(!exists(p->pw_name, l)&&getuid()!=0)
    {
      printf("User %s isn't a trusted updater.\n", p->pw_name);
      return(-1);
    }
  if(setuid(0)!=0)
    {
      printf("No setuid.\n");
      return(-1);
    }
    
  int status = execl("/usr/bin/ilenia.py", "", "-u", 0);
  
  return(status);
}
