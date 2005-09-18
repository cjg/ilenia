"""
    details.py
    Sun Sep 18 15:52:02 2005
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

import gtk

class Details(gtk.Window):
    def __init__(self):
        gtk.Window.__init__(self)
        self.set_title("Ilenia-notifier: details")
        vbox = gtk.VBox(spacing=0)
        self.w_label = gtk.Label()
        self.w_label.set_padding(0, 18)
        vbox.pack_start(self.w_label, 0, 0)
        self.pc = PkginfoContainer()
        vbox.pack_start(self.pc)
        self.add(vbox)
        self.resize(300, 200)
        self.connect("delete_event", self.on_delete)

    def set_list(self, list):
        self.pc.clear()
        for pkg in list:
            self.pc.add(Pkginfo(pkg))
        if len(list)==0:
            self.w_label.set_markup("<b>The System is Up To Date!</b>")
        else:
            self.w_label.set_markup("<b>There are %s updates available:</b>" %
                                    len(list))

    def show(self):
        self.show_all()

    def hide(self):
        self.hide_all()
    
    def on_delete(self, w, data):
        self.hide()
        return True

class PkginfoContainer(gtk.ScrolledWindow):
    def __init__(self):
        gtk.ScrolledWindow.__init__(self)
        self._vbox = gtk.VBox(spacing=24)
        self.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        self.set_border_width(2)
        self.set_shadow_type(gtk.SHADOW_NONE)
        gtk.ScrolledWindow.add_with_viewport(self, self._vbox)

    def add(self, pkginfo):
        self._vbox.pack_start(pkginfo, 0, 0)
        self._vbox.pack_start(gtk.HSeparator(), 0, 0)

    def clear(self):
        for w in self._vbox.get_children():
            self._vbox.remove(w)

class Pkginfo(gtk.Frame):
    def __init__(self, pkg):
        gtk.Frame.__init__(self)
        label = gtk.Label()
        label.set_markup("<b>%s</b>" % pkg["name"])
        self.set_label_widget(label)
        self.set_label_align(0.0, 0.5)
        self.set_border_width(0)
        self.set_shadow_type(gtk.SHADOW_NONE)
        alignment = gtk.Alignment(0.5, 0.5, 1.0, 1.0)
        gtk.Frame.add(self, alignment)
        alignment.set_padding(0, 0, 12, 0)
        vbox = gtk.VBox(spacing = 6)
        label = gtk.Label()
        label.set_markup("<b>Installed version</b>: %s" % pkg["l_version"])
        vbox.pack_start(label, 0, 0)
        label.set_alignment(0, 0.5)
        label = gtk.Label()
        label.set_markup("<b>Repository version</b>: %s" % pkg["r_version"])
        vbox.pack_start(label, 0, 0)
        label.set_alignment(0, 0.5)
        label = gtk.Label()
        label.set_markup("<b>Repository</b>: %s" % pkg["repo"])
        vbox.pack_start(label, 0, 0)
        label.set_alignment(0, 0.5)
        alignment.add(vbox)
        self.show_all()
        
if __name__ == "__main__":
    Details([{"name":"ilenia-notifier","l_version":"0","r_version":"0",
              "repo":"none"}])
    gtk.main()
