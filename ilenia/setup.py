"""
    setup.py
    Mon Sep 12 19:01:25 2005
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

import distutils, os, os.path
from distutils import cmd
from distutils.core import setup, Extension
from distutils.command.install import install as _install
from distutils.util import change_root
from distutils.dir_util import mkpath
from distutils.file_util import copy_file

class install(_install):
    def __init__(self, *a, **kw):
        self.sub_commands = self.sub_commands + [
            ('install_var', None),
            ("install_etc", None),
        ]
        _install.__init__(self, *a, **kw)

class install_var(cmd.Command):
    description = 'Install directory'

    def initialize_options(self):
        self.install_dir = None
        self.root = None
        self.prefix = None

    def finalize_options (self):
        if self.install_dir is None:
            self.set_undefined_options('install',
                    ('root', 'root'))
            prefix = "/"
            if self.root is not None:
                prefix = change_root(self.root, prefix)
            self.install_dir = os.path.join(prefix, 'var', 'lib')

    def run(self):
        mkpath(os.path.join(self.install_dir, "ilenia"))

class install_etc(cmd.Command):
    description = 'Install conf file'

    def initialize_options(self):
        self.install_dir = None
        self.root = None
        self.prefix = None

    def finalize_options (self):
        if self.install_dir is None:
            self.set_undefined_options('install',
                    ('root', 'root'))
            prefix = "/"
            if self.root is not None:
                prefix = change_root(self.root, prefix)
            self.install_dir = os.path.join(prefix, 'etc')

    def run(self):
        mkpath(os.path.join(self.install_dir))
        copy_file(os.path.join("conf", "ilenia.conf.new"),
                  os.path.join(self.install_dir, "ilenia.conf.new"))
        

setup(name='ilenia',
      version='1.4-testing',
      description='Package Manager for Slackware',
      author='Coviello Giuseppe',
      author_email='immigrant@email.it',
      url='http://ilenia.berlios.de',
      license='GNU General Public License v2',
      packages=['ilenia', 'ilenia.lib'],
      scripts=["ilenia.py"],
      ext_modules = [Extension("ilenia/lib/vercmp", ["src/vercmp.c"])],
      cmdclass={'install': install,
                "install_var":install_var,
                "install_etc":install_etc}
      )
