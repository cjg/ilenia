"""
    favorite.py
    Thu Sep 08 16:59:35 2005
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

import string

class Favoriterepo(list):
    def __init__(self, config):
        self.list = []
        try:
            items = config.items("favoriterepo")
        except:
            return

        for item in items:
            repo, pkgs = item
            if "," in pkgs:
                pkgs = pkgs.split(",")
                for pkg in pkgs:
                    pkg=pkg.strip()
                    self.list.append((pkg, repo))
                    if not pkg in self:
                        self.append(pkg)
            else:
                self.list.append((pkgs, repo))
                if not pkgs in self:
                    self.append(pkgs)

    def get_repo(self, pkg_name):
        try:
            return self.list[self.index(pkg_name)][1]
        except:
            return None
                
