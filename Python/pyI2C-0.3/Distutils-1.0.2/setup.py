#!/usr/bin/env python

# Setup script for the Python Module Distribution Utilities; this is the
# canonical example of a Distutils setup script.  (Although it does *not*
# exercise all of the Distutils' features!)

# created 1999/03 Greg Ward

__revision__ = "$Id: setup.py,v 1.30 2001/04/20 18:28:59 akuchling Exp $"

from distutils.core import setup

setup (name = "Distutils",
       version = "1.0.2",
       description = "Python Distribution Utilities",
       author = "Greg Ward",
       author_email = "gward@python.net",
       maintainer = "A.M. Kuchling",
       maintainer_email = 'amk1@bigfoot.com',
       url = "http://www.python.org/sigs/distutils-sig/",
       licence = "Python",
       long_description = """\
A collection of modules to aid in the distribution and installation of
Python modules, extensions, and (ultimately) applications.  A standard
part of Python 1.6 and 2.0, but also distributed separately for use with
Python 1.5.""",

       # This implies all pure Python modules in ./distutils/ and
       # ./distutils/command/
       packages = ['distutils', 'distutils.command'],
      )
