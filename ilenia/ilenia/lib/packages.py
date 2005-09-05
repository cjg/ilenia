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

class Packages:
    def __init__(self, repo = None):
        if not repo:
            self.build_local_list()
        else:
            self.build_repo_list(repo)
            
    def build_local_list(self):
        self.list = []
        for f in os.listdir("/var/log/packages/"):
            f_splitted = f.rsplit("-", 3)
            self.list.append({"name":f_splitted[0],
                              "version":f_splitted[1],
                              "build":f_splitted[3]})

    def build_repo_list(self, repo):
        self.list = []
        try:
            f_io = file(os.path.join(repo, "PACKAGES.TXT"))
        except:
            print os.path.join(repo, "PACKAGES.TXT")
            return
        for line in f_io.readlines():
            if line[:12] == "PACKAGE NAME":
                line = line[13:].strip()[:-4]
                line_splitted = line.rsplit("-", 3)
                try:
                    self.list.append({"name":line_splitted[0],
                                      "version":line_splitted[1],
                                      "build":line_splitted[3],
                                      "repo":repo})
                except:
                    pass
                    #print line
                                  


if __name__ == "__main__":
    for pkg in Packages("/var/swaret/repos/Slacky.it/PACKAGES.TXT").list:
        print "Name %s Version %s Build %s" % (pkg["name"], pkg["version"],
                                               pkg["build"])
