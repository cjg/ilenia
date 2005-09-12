/***************************************************************************
 *            drawtrayicon.c
 *
 *  Mon Sep 12 11:43:59 2005
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

#include <gtk/gtk.h>
#include "eggtrayicon.h"

static void
button_pressed_cb (GtkWidget * w_event, GtkWidget * w_icon)
{
	gtk_main_quit ();
}

GtkWidget *
draw_tray_icon ()
{
	EggTrayIcon *tray_icon;
	GtkWidget *w_icon =
		gtk_image_new_from_stock (GTK_STOCK_FILE, GTK_ICON_SIZE_MENU);
	GtkWidget *w_event = gtk_event_box_new ();
	gtk_widget_set_events (GTK_WIDGET (w_event), GDK_BUTTON_PRESS);
	g_signal_connect (w_event, "button-press-event",
			  G_CALLBACK (button_pressed_cb), w_icon);
	gtk_container_add (GTK_CONTAINER (w_event), w_icon);
	//gtk_image_set_from_stock (GTK_IMAGE (w_icon), GTK_STOCK_YES,
	//                        GTK_ICON_SIZE_MENU);
	tray_icon = egg_tray_icon_new ("ilenia-notifier");
	//button = gtk_button_new_with_label ("Quit");
	//g_signal_connect (button, "clicked",
	//        G_CALLBACK (first_button_pressed), tray_icon);
	gtk_container_add (GTK_CONTAINER (tray_icon), w_event);
	gtk_widget_show_all (GTK_WIDGET (tray_icon));
	return (w_icon);
	//gtk_main ();
}

//void
