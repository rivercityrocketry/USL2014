#!/usr/bin/env python

# Template for creating your own setup.py.  See the USAGE file in
# the Distutils source distribution for descriptions of all the
# options shown below.  Brief instructions on what to do:
#   - change XXX to the name of your module distribution; note that
#     this will be used to generate a filename, so keep it short and
#     filesystem-friendly (ie. no spaces or punctuation, except maybe
#     underscore)
#   - set the other metadata: version, description, author, author_email
#     and url.  All of these except 'description' are required, although
#     you may supply 'maintainer' and 'maintainer_email' in place of (or in
#     addition to) 'author' and 'author_email' as appropriate.
#   - fill in or delete the 'packages', 'package_dir', 'py_modules',
#     and 'ext_modules' options as appropriate -- see USAGE for details
#   - delete this comment and change '__revision__' to whatever is
#     appropriate for your revision control system of choice (just make
#     sure it stores the revision number for your distribution's setup.py
#     script, *not* the examples/template_setup.py file from Distutils!)

"""Setup script for the XXX module distribution."""

__revision__ = "$Id: template_setup.py,v 1.2 2000/05/31 02:29:52 gward Exp $"

from distutils.core import setup, Extension

setup (# Distribution meta-data
       name = "XXX",
       version = "",
       description = "",
       author = "",
       author_email = "",
       url = "",

       # Description of the modules and packages in the distribution
       packages = [''],
       package_dir = {'': ''},
       py_modules = [''],
       ext_modules = 
           [Extension('my_ext', ['my_ext.c', 'file1.c', 'file2.c'],
                      include_dirs=[''],
                      library_dirs=[''],
                      libraries=[''],),
           ]
      )
