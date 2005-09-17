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
    def __init__(self, options=None):
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

        if "-u" in action or "-U" in action:
            notify_update()
        else:
            notify_end()

    def do_list_installed(self):
        for pkg in self.local_packages:
            pkg_info = self.local_packages.get_info(pkg)[0]
            print "Name: %s \t Version: %s" % (pkg_info["name"],
                                               pkg_info["version"])

    def do_list(self, args):
        if not args:
            args = self.repos
        for pkg in self.repos_packages:
            for pkg_info in self.repos_packages.get_info(pkg):
                if pkg_info["repo"] in args:
                    print "%s %s %s %s" % (pkg_info["name"],
                                           pkg_info["version"],
                                           pkg_info["build"], pkg_info["repo"])
        
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
            for pkg in u_list:
                print "%s \t %s \t %s \t %s" % (pkg["name"],
                                                pkg["l_version"],
                                                pkg["r_version"],
                                                pkg["repo"])
        else:
            print "The system is up to date"

    def do_update_pkg(self, args):
        from lib.download import download
        sys_update = False
        if not args:
            for pkg in confront(self):
                pkg_file = download(pkg, self.repos)
                if not pkg_file:
                    print "Error downloading %s from %s!" % (pkg["name"],
                                                             pkg["repo"])
                    return
                if not os.system("upgradepkg %s" % pkg_file) == 0:
                    print "Error upgrading %s" % pkg_file
                    return
                os.unlink(pkg_file)
            return
        
        for pkg_name in args:
            pkg = get_newer(self.repos_packages.get_info(pkg_name),
                            self.favoriterepo)
            if not pkg:
                if not sys_update:
                    print "Package %s not found!" % pkg_name
                continue
            pkg_file = download(pkg, self.repos)
            if not pkg_file:
                print "Error downloading %s from %s!" % (pkg["name"],
                                                         pkg["repo"])
                return
            if pkg_name in self.local_packages:
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
    Ilenia(IleniaOptions().parse())
