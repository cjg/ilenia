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
from lib.repos import Repos
from lib.favorite import Favoriterepo
from lib.confront import *
from lib.progdownload import *
from lib.option import IleniaOptions
from lib.notify import *
from lib.prettyprint import *
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
    def __init__(self, options=None, use_ipc=False):
        self.use_ipc = use_ipc

        if self.use_ipc:
            notify_start()
        
        self.config = Config()
        self.options = options
        
        self.repos = Repos(self.config)
           
        self.local_packages = Packages()
        self.repos_packages = Packages(self.repos)

        self.favoriterepo = Favoriterepo(self.config)
        self.no_favoriterepo = False

        if self.options:
            self.parse_options()
    
    def parse_options(self):
        if "--no-favoriterepo" in self.options:
            self.no_favoriterepo = True
            self.options.remove("--no-favoriterepo")
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
        elif action == "-i":
            self.do_list_installed()
        elif action == "-l":
            self.do_list(args)
        elif action == "-p":
            self.do_updated()
        elif action == "-U":
            self.do_update_pkg(args)
        elif action == "-h":
            self.do_help()

        if self.use_ipc:
            if "-u" in action or "-U" in action:
                notify_update()
            else:
                notify_end()

    def do_list_installed(self):
        for p in self.local_packages:
            pkg = self.local_packages.get_packages(p)[0]
            prettyprint(["Name:", pkg.name, "Version:",
                         pkg.version], [5,25,8,10])

    def do_list(self, args):
        if not args:
            args = self.repos
        prettyprint (["Package", "Version", "Build", "Repo"],
                     [25, 10, 6, 30])
        for p in self.repos_packages:
            for pkg in self.repos_packages.get_packages(p):
                if pkg.repo in args:
                    prettyprint ([pkg.name, pkg.version,
                                  pkg.release+pkg.packager, pkg.repo],
                                 [25, 10, 6, 30])
        
    def do_update(self, args):
        if not args:
            args = self.repos

        for repo_name in args:
            repo_path = os.path.join(os.path.sep, "var", "lib", "ilenia",
                                     repo_name)
            if not os.path.isdir(repo_path):
                os.mkdir(repo_path)
            print "Updating info about %s ..." % repo_name
            for f in ["CHECKSUMS.md5", "PACKAGES.TXT", "FILELIST.TXT"]:
                url = "%s/%s" % (self.repos.get_url(repo_name), f)
                filename = "%s/%s" % (repo_path, f)
                ProgressiveDownload(url, filename)

    def do_updated(self, print_output=True):
        u_list = confront(self)

        if not print_output:
            return u_list
        
        if len(u_list):
            prettyprint (["Package", "Local Ver", "Repo Ver", "Repo"],
                         [25, 15, 15, 20])

            for u in u_list:
                prettyprint ([u[0].name,
                              u[0].version+"-"+u[0].release+u[0].packager,
                              u[1].version+"-"+u[1].release+u[1].packager,
                              u[1].repo], [25, 15, 15, 20])
        else:
            print "The system is up to date"

    def do_update_pkg(self, args):
        from lib.download import download
        sys_update = False
        if not args:
            for u in confront(self):
                pkg_file = download(u[1], self.repos)
                if not pkg_file:
                    print "Error downloading %s from %s!" % (u[1].name,
                                                             u[1].repo)
                    return
                if not os.system("upgradepkg %s" % pkg_file) == 0:
                    print "Error upgrading %s" % pkg_file
                    return
                os.unlink(pkg_file)
            return
        
        for p in args:
            pkg = get_newer(self.repos_packages.get_packages(p),
                            self.favoriterepo)
            if not pkg:
                if not sys_update:
                    print "Package %s not found!" % p
                continue
            pkg_file = download(pkg, self.repos)
            if not pkg_file:
                print "Error downloading %s from %s!" % (pkg.name,
                                                         pkg.repo)
                return
            if p in self.local_packages:
                if not os.system("upgradepkg %s" % pkg_file) == 0:
                    print "Error upgrading %s" % pkg_file
                    return
            else:
                if not os.system("installpkg %s" % pkg_file) == 0:
                    print "Error installing %s" % pkg_file
                    return

            os.unlink(pkg_file)

    def do_help(self):
        IleniaOptions().print_help()

if __name__ == "__main__":
    Ilenia(IleniaOptions().parse(), True)
