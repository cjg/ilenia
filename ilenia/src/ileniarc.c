/***************************************************************************
 *            aggiorna.c
 *
 *  Tue Feb 15 19:02:36 2004
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
 *
 */

#include <stdio.h>
#include <string.h>
#include "manipola.h"
#include "ilenia.h"


int
parse_ileniarc()
{
  FILE * rc;
  if((rc=fopen("/etc/ilenia.rc", "r")))
    {
      char row[MASSIMO];
      while (fgets (row, MASSIMO, rc))
	{
	  strcpy(row, trim (row));
	  if (row[0] != '#')
	    {
	      if(strncmp(row, "POST_PKGADD", 11)==0)
		{
		  char splitted_row[2][MASSIMO];
		  split(row, "=", splitted_row);
		  strcpy(row, mid(splitted_row[1],1,FINE));
		  strcpy(row, trim(row));
		  strcpy(row, mid(row, 1, strlen(row)-2));
		  post_pkgadd=strdup(row);
		}
	    }
	}
    }
  else
    return(1);
  return(0);
}
