#!/usr/bin/env python

import re
from os import system
from distutils.core import setup, Extension
from subprocess import Popen, PIPE

white = re.compile(r'\s+')
flags = "-ljson -lparsley -I/usr/include -I/usr/local/include -I/opt/local/include -L/usr/lib -L/usr/local/lib -L/opt/local/lib "
flags += Popen(["xml2-config", "--libs", "--cflags"], stdout=PIPE).communicate()[0]
flags += Popen(["xslt-config", "--libs", "--cflags"], stdout=PIPE).communicate()[0]
flags = white.sub(" ", flags)

libraries = re.findall(r'-l(\S+)', flags)
include_dirs = re.findall(r'-I(\S+)', flags)
lib_dirs = re.findall(r'-L(\S+)', flags)

setup(name="pyparsley", version="1.0",
	ext_modules=[Extension("pyparsley", ["pyparsleymodule.c"], 
		library_dirs = lib_dirs,
		include_dirs = include_dirs,
		libraries    = libraries
	)])