#!/usr/bin/env python
#
# sample4 -- try out the "build_script" and "install_script" commands
#

from distutils.core import setup

setup (name = "sample4", version = "0",
       description = "Distutils Sample #4",
       scripts = ['script1', 'script2', 'script3', 'script4'],
      )
