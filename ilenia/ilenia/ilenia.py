"""
    ilenia.py
    Mon Sep 05 19:25:19 2005
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

from packages import Packages
from confront import *
from ConfigParser import ConfigParser
import os, os.path

class Config(ConfigParser):
    def __init__(self, file_name="/etc/ilenia.conf"):
        ConfigParser.__init__(self)
        self.read(file_name)

    def get_value(self, section, key, value_type=0):
        section = section.lower()
        key = str(key)
        if value_type == 0:
            return self.get(section, key)
        elif value_type == 1:
            return self.getint(section, key)

class Ilenia:
    def __init__(self, options, args):
        self.config = Config("ilenia.conf")

        self.repos = []
        for section in self.config.sections():
            if section[:4] == "repo":
                self.repos.append((section[5:].strip(),
                                   self.config.get_value(section, "url")))
        if options.update:
            self.do_update()
            
        self.local_packages = Packages().list
        self.repos_packages = []
        for repo in self.repos:
            self.repos_packages = self.repos_packages + Packages(repo[0]).list
            
        if options.list:
            self.do_list()
        if options.updated:
            self.do_updated()

    def do_list(self):
        for pkg in self.repos_packages:
            print "%s %s %s %s" % (pkg["name"], pkg["version"], pkg["build"],
                                   pkg["repo"])
        
    def do_update(self):
        import urllib
        for repo in self.repos:
            if not os.path.isdir(repo[0]):
                os.mkdir(repo[0])
            print "Updating info about %s ..." % repo[0]
            for f in ["CHECKSUMS.md5", "PACKAGES.TXT", "FILELIST.TXT"]:
                url = "%s/%s" % (repo[1], f)
                filename = "%s/%s" % (repo[0], f)
                urllib.urlretrieve(url, filename)

    def do_updated(self):
        u_list = confront(self)
        if len(u_list):
            for pkg in u_list:
                print "%s \t %s \t %s \t %s" % (pkg["name"],
                                                pkg["l_version"],
                                                pkg["r_version"],
                                                pkg["repo"])
        else:
            print "The system is up to date"


if __name__ == "__main__":
    from optparse import OptionParser

    parser = OptionParser(usage="%prog",
                          version="ilenia 1.4-testing")
    parser.add_option("-u", "--update",
                      action="store_true", dest="update", default=False,
                      help="Update packages lists.")
    parser.add_option("-l", "--list",
                      action="store_true", dest="list", default=False,
                      help="Lists available  packages. When specified lists"+
                      "packages of the specified repository")
    parser.add_option("-p", "--updated",
                      action="store_true", dest="updated", default=False,
                      help = "Lists packages that could be updated")
    parser.add_option("-v", "--Version",
                      action="store_true", dest="version", default=False,
                      help="Print version and exit.")

    options, args = parser.parse_args()
    Ilenia(options, args)
