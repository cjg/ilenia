/***************************************************************************
 *            main.c
 *
 *  Mon Sep 12 11:31:23 2005
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

#include <unistd.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <gtk/gtk.h>
#include "drawtrayicon.h"

gint
main (gint argc, gchar ** argv)
{
	FILE *pipein;
	gtk_init (&argc, &argv);
	GtkWidget *w_icon = draw_tray_icon ();
	chdir ("/home/slash/Desktop/ilenia/ilenia");
	pipein = popen ("python ilenia.py -p", "r");
	size_t *n = 0;
	char *ptr = NULL;
	getline (&ptr, &n, pipein);
	printf ("%s\n", ptr);
	gtk_main ();
	return 0;
}
