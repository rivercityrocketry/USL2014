#!/usr/bin/env python

"""Alternate Setup script for the sample #1 module distribution: single
   top-level pure Python module, found implicitly through 'packages'."""

__revision__ = "$Id: setup.py,v 1.1 1999/12/16 00:55:13 gward Exp $"

from distutils.core import setup

setup (# Distribution meta-data
       name = "sample",
       version = "1.0",
       description = "Distutils sample distribution #1 (alt. setup script)",

       # Description of the modules and packages in the distribution
       packages = [''],                 # empty string means root "package"
      )
