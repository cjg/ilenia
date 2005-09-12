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
    for pkg_name in parent.local_packages:
        pkg = parent.local_packages.get_info(pkg_name)[0]
        newer_pkg = get_newer(parent.repos_packages.get_info(pkg_name))
        if not newer_pkg:
            continue
        if not parent.no_favoriterepo and pkg_name in parent.favoriterepo:
            if not newer_pkg["repo"] == parent.favoriterepo.get_repo(pkg_name):
                continue
        if vercmp(pkg["version"], newer_pkg["version"]):
            updated_packages.append({"name":pkg["name"],
                                     "l_version":pkg["version"],
                                     "r_version":newer_pkg["version"],
                                     "repo":newer_pkg["repo"]})
    return updated_packages

def get_newer(pkglist, favoriterepo=None):
    if not pkglist:
        return None
    if favoriterepo and not pkglist[0]["name"] in favoriterepo:
        favoriterepo = None
    newer_pkg = None
    for pkg in pkglist:
        try:
            if favoriterepo:
                if not pkg["repo"] == favoriterepo.get_repo(pkg["name"]):
                    continue
            if vercmp(newer_pkg["version"], pkg["version"]):
                newer_pkg = pkg
        except:
            if favoriterepo:
                if not pkg["repo"] == favoriterepo.get_repo(pkg["name"]):
                    continue
            newer_pkg = pkg
    return newer_pkg
        
