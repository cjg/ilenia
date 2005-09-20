"""
    option.py
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

import sys, string
from copy import copy
from prettyprint import *

class Options(list):
    def __init__(self):
        self.options = []

    def add_option(self, option):
        self.append(option)

    def parse(self, args = sys.argv[1:]):
        options = []
        rargs = copy(args)
        for a in args:
            if a[0] == "-":
                rargs.remove(a)
                a_type = self.check_type(a)
                if a_type == "toggle":
                    options.append(a)
                elif a_type == "list":
                    a_args = self.get_args(rargs)
                    options.append({a:a_args})
                    args = args[len(a_args):]
                else:
                    print "Error: option %s not known!" % a

        return options

    def get_args(self, rargs):
        args = []
        for arg in rargs:
            if arg[0] == "-":
                return args
            args.append(arg)
        return args

    def check_type(self, option):
        for o in self:
            if option == o.option:
                return o.type
        return None

    def print_help(self):
        for option in self:
            prettyprint ([option.option, option.help_text], [18, 60])
            
    
class Option:
    def __init__(self, option, o_type="toggle", help_text=None):
        self.option = option
        self.type = o_type
        self.help_text = help_text

class IleniaOptions(Options):
    def __init__(self):
        Options.__init__(self)
        self.add_option(Option("-u", "list",
                               "Updates info about repos packages"))
        self.add_option(Option("-i", "toggle", "Lists installed packages"))
        self.add_option(Option("-l", "list",
                               "Lists packages of all repos or that of the specifies ones"))
        self.add_option(Option("-p", "toggle",
                               "Shows packages that must be updated"))
        self.add_option(Option("-U", "list",
                               "Upgrade/install specified packages or upgrade the system"))
        self.add_option(Option("--no-favoriterepo", "toggle",
                               "Nevermind about favoriterepo settings"))
        self.add_option(Option("-h", "toggle", "This quick reference"))


if __name__ == "__main__":
    print IleniaOptions().parse()
