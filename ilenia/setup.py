import distutils
from distutils.core import setup, Extension

setup(name = "ilenia",
      version = "1.4-testing",
      ext_modules = [Extension("vercmp", ["src/vercmp.c"])])
