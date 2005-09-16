"""
    notify.py
    Fri Sep 16 11:30:02 2005
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

import socket

def _send(txt):
    try:
        _socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        _socket.connect("/tmp/ilenia-notifier")
        _socket.send(txt)
        _socket.close()
    except:
        pass
    
def notify_update():
    _send("update")

def notify_end():
    _send("end")
    
def notify_start():
    _send("start")
