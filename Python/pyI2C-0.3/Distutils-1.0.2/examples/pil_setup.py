#!/usr/bin/env python

"""Example setup.py for Fredrik Lundh's Python Imaging Library 
   (the Imaging distribution)."""

# created 1999/09/19, Greg Ward

__revision__ = "$Id: pil_setup.py,v 1.14 2000/06/30 03:46:28 gward Exp $"

import os
from distutils.core import setup, Extension
from distutils.ccompiler import new_compiler
from distutils.command.config import config
from glob import glob

lib_dir = 'libImaging'
lib_name = "Imaging"
lib_sources = glob (os.path.join (lib_dir, "[A-Z]*.c"))

# PIL can build against several optional libraries: jpeglib, zlib, and
# Tcl/Tk.  Since the Distutils hasn't yet tackled the job of rewriting
# Autoconf in Python, you have to supply some vital data here: which of
# these libraries to use, where to find their header files, and where to
# find the libraries themselves.
# 
# 'optional_libs' just lists the library names -- in a Unix environment,
# each of these strings will be translated to eg. "-ljpeg" on the
# link command line, meaning you have to have libjpeg.a somewhere.
#
# 'include_dirs' is a list of directories to search for header files for
# the optional libraries plus PIL's own internal support library
# (libImaging/libImaging.a under Unix).  You should only customize the
# entries after 'lib_dir'.  For a typical Unix installation
# "/usr/local/include" is probably what you want, since that's the
# traditional place to install third-party libraries.  (Your mileage may
# vary, so check with your system administrator if you're not sure
# whether these libraries are installed and where they are installed.)
# Most Linux distributions should include those libraries
# out-of-the-box, so the header files should be in /usr/include, meaning
# you don't have to add anything to include_dirs.
#
# Finally, 'library_dirs' lists the locations of PIL's internal support
# library as well as the optional third-party libraries.  Again, the
# traditional Unix location is "/usr/local/lib", and you probably don't
# need to list any extra library directories for most Linux
# distributions.

optional_libs = ['jpeg', 'z', 'tcl8.0', 'tk8.0']  # good sysadmin
#optional_libs = []                               # lazy sysadmin

include_dirs = [lib_dir, '/usr/local/include']     # typical Unix
#include_dirs = [lib_dir, '/depot/sundry/include'] # CNRI's setup
#include_dirs = [lib_dir]                          # most Linux distributions

library_dirs = ['/usr/local/lib']                  # typical Unix
#library_dirs = ['/depot/sundry/plat/lib']         # CNRI's setup
#library_dirs = []                                 # most Linux distributions



# Auto-configuration: this will obsolete the above manual editing soon, but
# not yet.  The problem is that there's no way to communicate what the
# "config" command finds out to the "build_clib" and "build_ext" commands.
# Thus, it's fun to play with auto-configuration, but it's not ready for
# prime-time yet.

class config_Imaging (config):

    user_options = config.user_options + [
        ('tcltk-include-dirs=', None,
         "directories to search for Tcl/Tk header files"),
        ('tcltk-library-dirs=', None,
         "directories to search for Tcl/Tk library files"),
        ('tcl-version', None,
         "version of Tcl library to look for"),
        ('tk-version', None,
         "version of Tk library to look for"),
        ('zlib-include-dirs=', None,
         "directories to search for zlib header file"),
        ('zlib-library-dirs=', None,
         "directories to search for zlib library file"),
        ]

    def initialize_options (self):
        config.initialize_options(self)
        self.tcltk_include_dirs = None
        self.tcltk_library_dirs = None
        self.tcl_version = None
        self.tk_version = None

        self.zlib_include_dirs = None
        self.zlib_library_dirs = None

    # No 'finalize_options()' method -- none of our options have default
    # values (other than None, that is).

    def run (self):
        
        have_tcl = self.check_lib("tcl" + (self.tcl_version or ""),
                                  self.tcltk_library_dirs,
                                  ["tcl.h"], self.tcltk_include_dirs)
                                  
        have_tk = self.check_lib("tk" + (self.tk_version or ""),
                                 self.tcltk_library_dirs,
                                 ["tk.h"], self.tcltk_include_dirs)

        have_zlib = self.check_lib("z", self.zlib_library_dirs,
                                   ["zlib.h"], self.zlib_include_dirs)


        print "have tcl? %d  have tk? %d  have zlib? %d" % \
              (have_tcl, have_tk, have_zlib)


# ------------------------------------------------------------------------
# You should't have to change anything below this point!

# Figure out macros to define based on whether Tcl/Tk are in the
# 'optional_libs' list.
use_tcl = use_tk = 0
for lib in optional_libs:
    if lib[0:3] == 'tcl':
        use_tcl = 1
    if lib[0:2] == 'tk':
        use_tk = 1

if use_tcl and use_tk:
    ext_macros = [('WITH_TKINTER', None)]
else:
    ext_macros = None

# arg -- still haven't written autoconf in Python, so we rely on
# the user to run the configure script
config_header = os.path.join (lib_dir, "ImConfig.h")
if not os.path.exists (config_header):
    raise SystemExit, \
          "%s not found -- did you run the configure script in %s?" % \
          (config_header, lib_dir)

setup (name = "Imaging",
       version = "1.0",
       description = "Python Imaging Library",
       author = "Fredrik Lundh",
       author_email = "fredrik@pythonware.com",
       url = "http://www.pythonware.com/downloads.htm",

       # Hmm, we don't install the sane module or PIL's "Scripts"
       # directory.  We could handle sane (in a slightly ugly way -- since
       # it's in a directory of its own, it would necessarily be the only
       # top-level module in the distribution), but Distutils as yet has no
       # facilities for installing scripts.

       cmdclass = {'config': config_Imaging},

       libraries = [(lib_name,
                     { 'sources': lib_sources,
                       'include_dirs': include_dirs,
                       'macros': [('HAVE_CONFIG_H', None)],
                     }
                   )],

       packages = ['PIL'],
       ext_modules = 
           [Extension('_imaging',
                      ['_imaging.c', 'decode.c', 'encode.c',
                       'map.c', 'display.c', 'outline.c', 'path.c'],
                      # This must include the directories with the JPEG,
                      # zlib, and Tcl/Tk header files (if installed)
                      include_dirs = include_dirs,

                      # Keep this for Tcl/Tk support
                      define_macros = ext_macros,

                      # This must include the directories with the JPEG, zlib,
                      # and Tcl/Tk libraries (whatever's available)
                      library_dirs = library_dirs,

                      # And this, of course, lists which of those external
                      # libraries to link against (plus libImaging, which
                      # *must* be included!)
                      libraries = optional_libs)]
      )
