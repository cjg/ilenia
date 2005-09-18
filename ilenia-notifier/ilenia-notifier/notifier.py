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

import gtk, gettext, os, sys, thread, socket

gettext.NullTranslations()
gettext.install("ilenia-notifier")

from egg import trayicon

from ilenia.ilenia import Ilenia
from lib.details import Details

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

        w_update_repos = gtk.ImageMenuItem(gtk.STOCK_CONNECT,
                                           _("Update Repos"))
        w_update_repos.connect("button_press_event", self.on_update_repos)
        self.w_menu.add(w_update_repos)
        
        self.w_menu.add(gtk.SeparatorMenuItem())

        w_show_details = gtk.ImageMenuItem(gtk.STOCK_INFO,
                                           _("Show Details Window"))
        w_show_details.connect("button_press_event", self.on_show_details)
        self.w_menu.add(w_show_details)
        
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
        self.updating_repos = False
        self.updating_ilenia = False
        self.working = False

        self.w_details = Details()

        gtk.threads_init()
        thread.start_new_thread(self.init_ilenia, ())
        thread.start_new_thread(self.ipc_start, ())
        gtk.main()

    def set_icon(self, icon):
        self._icon = icon
        self.w_icon.set_from_stock(self._icon, gtk.ICON_SIZE_MENU)
        
    def init_ilenia(self):
        while self.updating_repos:
            pass
        self.updating_ilenia = True
        gtk.threads_enter()
        self.set_icon(gtk.STOCK_REFRESH)
        self.w_tooltips.set_tip(self, _("Ilenia-notifier: updating .."))
        gtk.threads_leave()
        self.ilenia = Ilenia()
        self.get_updated()
        self.updating_ilenia = False

    def get_updated(self):
        self.u_list = self.ilenia.do_updated(False)
        self.w_details.set_list(self.u_list)
        if len(self.u_list)>0:
            self.set_toupdate(len(self.u_list))
        else:
            self.set_noupdate()

    def set_toupdate(self, num):
        gtk.threads_enter()
        self.set_icon(gtk.STOCK_NO)
        self.w_tooltips.set_tip(self,
                                _("Ilenia-notifier: %i updates available") %
                                num)
        gtk.threads_leave()
    
    def set_noupdate(self):
        gtk.threads_enter()
        self.set_icon(gtk.STOCK_YES)
        self.w_tooltips.set_tip(self,
                                _("Ilenia-notifier: the system is up to date"))
        gtk.threads_leave()

    def _update_repos(self):
        while self.updating_ilenia:
            pass
        gtk.threads_enter()
        self.set_icon(gtk.STOCK_CONNECT)
        self.w_tooltips.set_tip(self,
                                _("Ilenia-notifier: updating info about repos ..."))
        gtk.threads_leave()
        stdout = os.popen("update-repos")
        stdout.close()
        self.updating_repos = False
    
    def update_repos(self):
        self.updating_repos = True
        thread.start_new_thread(self._update_repos, ())
    
    def on_event(self, w, event):
        if self.updating_repos or self.updating_ilenia or self.working:
            return
        if event.button == 1:
            self.w_details.show()
        if event.button == 3:
            self.w_menu.popup(None, None, None, 0, event.time)

    def on_show_details(self, w, event):
        if event.button == 1:
            self.w_details.show()
            
    def on_update(self, w, event):
        if event.button == 1:
            thread.start_new_thread(self.init_ilenia, ())

    def on_update_repos(self, w, event):
        if event.button == 1:
            thread.start_new_thread(self._update_repos, ())
            
    def on_quit(self, w, event):
        if event.button == 1:
            os.unlink("/tmp/ilenia-notifier")
            gtk.main_quit()

    def ipc_start(self):
        import socket
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        sock.bind('/tmp/ilenia-notifier')
        sock.listen(1)
        while 1:
            client = sock.accept()[0]
            client_command = client.recv(1024)
            if client_command == "start":
                gtk.threads_enter()
                self._picon = self._icon
                self.set_icon(gtk.STOCK_EXECUTE)
                self.working = True
                gtk.threads_leave()
            elif client_command == "update":
                self.working = False
                thread.start_new_thread(self.init_ilenia, ())
            elif client_command == "end":
                self.working = False
                gtk.threads_enter()
                self.set_icon(self._picon)
                gtk.threads_leave()
            client.close() 

if __name__ == "__main__":
    Notifier()

    
