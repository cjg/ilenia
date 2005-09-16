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

import os, os.path, string

class Packages(list):
    def __init__(self, repo = None):
        if not repo:
            self.build_local_infolist()
        else:
            self.build_repo_infolist(repo)
        self.sort()
            
    def build_local_infolist(self):
        self.infolist = []
        self.list = []
        for f in os.listdir("/var/log/packages/"):
            f_splitted = f.rsplit("-", 3)
            try:
                self.infolist.append({"name":f_splitted[0],
                                      "version":f_splitted[1],
                                      "build":f_splitted[3]})
            
                self.list.append(f_splitted[0])
                if not f_splitted[0] in self:
                    self.append(f_splitted[0])
            except:
                pass
                #print f
            
    def build_repo_infolist(self, repos):
        self.infolist = []
        self.list = []
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
                    line_splitted = line.rsplit("-", 3)
                    try:
                        self.infolist.append({"name":line_splitted[0],
                                              "version":line_splitted[1],
                                              "build":line_splitted[3],
                                              "repo":repo})
                        self.list.append(line_splitted[0])
                        if not line_splitted[0] in self:
                            self.append(line_splitted[0])
                    except:
                        pass

    def get_info(self, pkg_name):
        if not pkg_name in self:
            return None
        infos = []
        index = self.list.index(pkg_name)
        infos.append(self.infolist[index])
        while True:
            index += 1
            try:
                index = self.list.index(pkg_name, index)
                infos.append(self.infolist[index])
            except:
                return infos

if __name__ == "__main__":
    p = Packages(["slackware", "freerock"])
    print p.get_info("xchat")
