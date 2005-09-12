/***************************************************************************
 *            drawtrayicon.h
 *
 *  Mon Sep 12 11:47:41 2005
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

#ifndef _DRAWTRAYICON_H
#define _DRAWTRAYICON_H

GtkWidget *draw_tray_icon ();
void set_updated_icon (GtkWidget * w_icon);
void set_toupdate_icon (GtkWidget * w_icon);

#endif /* _DRAWTRAYICON_H */