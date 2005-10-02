"""
    packages.py
    Mon Sep 05 19:26:40 2005
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

import os, os.path, string, re, copy

class Packages(list):
    def __init__(self, repo = None):
        if not repo:
            self.build_local_infolist()
        else:
            self.build_repo_infolist(repo)
        self._unsorted = copy.copy(self)
        self.sort()
            
    def build_local_infolist(self):
        self._list = []
        for f in os.listdir("/var/log/packages/"):
            try:
                self._list.append(Package(f, "local"))
                self.append(self._list[len(self._list)-1].name)
            except:
                pass
            
    def build_repo_infolist(self, repos):
        self._list = []
        for repo in repos:
            try:
                f_io = file(os.path.join(os.path.sep, "var", "lib", "ilenia",
                                         repo, "PACKAGES.TXT"))
            except:
                print os.path.join(repo, "PACKAGES.TXT")
                return
            for line in f_io.readlines():
                if line[:12] == "PACKAGE NAME":
                    line = line[13:].strip()[:-4]
                    try:
                        self._list.append(Package(line, repo))
                        self.append(self._list[len(self._list)-1].name)
                    except:
                        pass

    def get_packages(self, name):
        if not name in self:
            return None
        
        packages = []
        index = self._unsorted.index(name)
        packages.append(self._list[index])
        while True:
            index += 1
            try:
                index = self._unsorted.index(name, index)
                packages.append(self._list[index])
            except:
                return packages

class Package:
    def __init__(self, pkgname, repo):
        self.name = ""
        self.version = ""
        self.arch = ""
        self.release = ""
        self.packager = ""
        self.repo = repo
        [self.name, self.version, self.arch, build] = pkgname.rsplit("-", 3)
        packager = re.search("[a-zA-Z]\w*", build)
        if packager:
            self.packager = packager.group(0)
            self.release = build[:-len(self.packager)]
        else:
            self.release = build

if __name__ == "__main__":
    p = Packages(["slackware", "freerock"])
    print p.get_packages("xchat")[0].name
