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

from lib.packages import Packages
from lib.confront import *
from lib.option import IleniaOptions
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
    def __init__(self, options):
        self.config = Config("ilenia.conf")
        self.options = options
        
        self.repos = []
        for section in self.config.sections():
            if section[:4] == "repo":
                self.repos.append((section[5:].strip(),
                                   self.config.get_value(section, "url")))
           
        self.local_packages = Packages().list
        self.repos_packages = []
        for repo in self.repos:
            self.repos_packages = self.repos_packages + Packages(repo[0]).list
        self.parse_options()
            
    def parse_options(self):
        for option in self.options:
            if "dict" in str(type(option)):
                action = option.keys()[0]
                args = option.values()[0]
                self.do_action(action, args)
            else:
                self.do_action(option)

    def do_action(self, action, args=None):
        if action == "-u":
            self.do_update(args)
        elif action == "-l":
            self.do_list(args)
        elif action == "-p":
            self.do_updated()
        elif action == "-U":
            self.do_update_pkg(args)

    def do_list(self, args):
        if not args:
            args = []
            for repo in self.repos:
                args.append(repo[0])
        print args
        for pkg in self.repos_packages:
            if pkg["repo"] in args:
                print "%s %s %s %s" % (pkg["name"], pkg["version"],
                                       pkg["build"], pkg["repo"])
        
    def do_update(self, args):
        import urllib
        repos = []
        if not args:
            repos = self.repos
        else:
            for arg in args:
                for repo in self.repos:
                    if arg == repo[0]:
                        repos.append(repo)
        for repo in repos:
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
    Ilenia(IleniaOptions().parse())
