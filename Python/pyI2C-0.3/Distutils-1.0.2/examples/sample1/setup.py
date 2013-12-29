#!/usr/bin/env python

"""Setup script for the sample #1 module distribution: single top-level
   pure Python module, named explicitly in 'py_modules'."""

__revision__ = "$Id: setup.py,v 1.1 1999/12/16 00:54:21 gward Exp $"

from distutils.core import setup

setup (# Distribution meta-data
       name = "sample",
       version = "1.0",
       description = "Distutils sample distribution #1",

       # Description of the modules and packages in the distribution
       py_modules = ['sample'],
      )
