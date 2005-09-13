"""
    notifier.py
    Tue Sep 13 12:02:28 2005
    Copyright 2005 - 2006 Coviello Giuseppe
    immigrant@email.it
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
"""

import gtk, gettext, os, sys, thread

gettext.NullTranslations()
gettext.install("ilenia-notifier")

from egg import trayicon

from ilenia.ilenia import Ilenia

class Notifier(trayicon.TrayIcon):
    def __init__(self):
        trayicon.TrayIcon.__init__(self, "Ilenia-notifier")

        self.ilenia = None
        
        w_event = gtk.EventBox()
        self.w_icon = gtk.Image()
        self.w_icon.set_from_stock(gtk.STOCK_DIALOG_WARNING,
                                   gtk.ICON_SIZE_MENU) 
    
        w_event.set_events(gtk.gdk.BUTTON_PRESS_MASK | 
                           gtk.gdk.POINTER_MOTION_MASK | 
                           gtk.gdk.POINTER_MOTION_HINT_MASK |
                           gtk.gdk.CONFIGURE)
    
        w_event.add(self.w_icon)
        self.w_menu = gtk.Menu()

        self.w_update = gtk.ImageMenuItem(gtk.STOCK_REFRESH, _("Update"))
        self.w_update.connect("button_press_event", self.on_update)
        self.w_menu.add(self.w_update)

        self.w_menu.add(gtk.SeparatorMenuItem())
        
        self.w_quit = gtk.ImageMenuItem(gtk.STOCK_QUIT, _("Quit"))
        self.w_quit.connect("button_press_event", self.on_quit)
        self.w_menu.add(self.w_quit)
                        
        self.w_menu.show_all()
        w_event.connect("button_press_event", self.on_event);

        self.w_tooltips = gtk.Tooltips()
        self.w_tooltips.set_tip(self, "Ilenia-notifier")
        self.w_tooltips.enable()
        self.add(w_event)
        self.show_all()
        gtk.threads_init()
        thread.start_new_thread(self.init_ilenia, ())
        gtk.main()

    def init_ilenia(self):
        gtk.threads_enter()
        self.w_icon.set_from_stock(gtk.STOCK_REFRESH, gtk.ICON_SIZE_MENU)
        gtk.threads_leave()
        self.ilenia = Ilenia()
        self.get_updated()

    def get_updated(self):
        u_list = self.ilenia.do_updated(False)
        if len(u_list)>0:
            self.set_toupdate(len(u_list))
        else:
            self.set_noupdate()

    def set_toupdate(self, num):
        gtk.threads_enter()
        self.w_icon.set_from_stock(gtk.STOCK_NO, gtk.ICON_SIZE_MENU)
        self.w_tooltips.set_tip(self,
                                _("Ilenia-notifier: %i updates available") %
                                num)
        gtk.threads_leave()
    
    def set_noupdate(self):
        gtk.threads_enter()
        self.w_icon.set_from_stock(gtk.STOCK_YES, gtk.ICON_SIZE_MENU)
        gtk.threads_leave()
    
    def on_event(self, w, event):
        if event.button == 3:
            self.w_menu.popup(None, None, None, 0, event.time)

    def on_update(self, w, event):
        if event.button == 1:
            thread.start_new_thread(self.init_ilenia, ())
            
    def on_quit(self, w, event):
        if event.button == 1:
            gtk.main_quit()

if __name__ == "__main__":
    Notifier()

    
