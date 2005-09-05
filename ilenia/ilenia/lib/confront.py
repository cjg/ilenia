"""
    confront.py
    Mon Sep 05 19:27:15 2005
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

from vercmp import vercmp

def confront(parent):
    updated_packages = []
    repos_packages = []
    for l_pkg in parent.local_packages:
        for r_pkg in parent.repos_packages:
            if l_pkg["name"] == r_pkg["name"]:
                if vercmp(l_pkg["version"], r_pkg["version"]):
                    updated_packages.append({"name":l_pkg["name"],
                                             "l_version":l_pkg["version"],
                                             "r_version":r_pkg["version"],
                                             "repo":r_pkg["repo"]})

    return updated_packages
