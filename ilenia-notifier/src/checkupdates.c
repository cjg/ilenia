/***************************************************************************
 *            checkupdates.c
 *
 *  Mon Sep 12 13:59:02 2005
 *  Copyright  2005  Coviello Giuseppe
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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "drawtrayicon.h"

void
set_icon (GtkWidget * w_icon)
{
	FILE *pipein;
	pipein = popen ("python ilenia.py -p", "r");
	size_t n = 0;
	char *ptr = NULL;
	getline (&ptr, &n, pipein);
	printf ("%s\n", ptr);
	if (strcmp ("The system is up to date", ptr))
		set_toupdate_icon (w_icon);
	else
		set_updated_icon (w_icon);
	free (ptr);
	// but i don't like timeout
	g_timeout_add(30*1000, set_icon, w_icon);
}
