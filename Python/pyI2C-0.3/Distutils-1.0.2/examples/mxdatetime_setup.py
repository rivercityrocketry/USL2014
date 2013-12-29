#!/usr/bin/env python

"""Example setup.py for Marc-André Lemburg's mxDateTime module
   distribution."""

# created 1999/09/19, Greg Ward

__revision__ = "$Id: mxdatetime_setup.py,v 1.9 2000/06/30 03:46:28 gward Exp $"

import string
from distutils.core import setup, Extension
from distutils.command.config import config

# Auto-configuration: this will obsolete manual editing of the setup script
# soon, but not yet.  The problem is that there's no way to communicate
# what the "config" command finds out to the "build_clib" and "build_ext"
# commands.  Thus, it's fun to play with auto-configuration, but it's not
# ready for prime-time yet.

class config_mxDateTime (config):

    def run (self):
        have = {}
        have['strftime'] = self.check_func('strftime', ['time.h'])
        have['strptime'] = self.check_func('strptime', ['time.h'])
        have['timegm'] = self.check_func('timegm', ['time.h'])

        define = []
        undef = []
        for name in have.keys(): # ('strftime', 'strptime', 'timegm'):
            macro_name = 'HAVE_' + string.upper(name)
            if have[name]:
                define.append((macro_name, None))
            else:
                undef.append(macro_name)

        print "macros to define:", define
        print "macros to undefine:", undef

        build = self.distribution.reinitialize_command('build')
        build.define = define
        build.undef = undef

    # run ()

# class config_mxDateTime
    

setup (name = "mxDateTime",
       version = "1.3.0",
       description = "",
       author = "Marc-André Lemburg",
       author_email = "mal@lemburg.com",
       url = "http://starship.python.net/~lemburg/mxDateTime.html",

       cmdclass = {'config': config_mxDateTime},
       packages = ['DateTime', 'DateTime.Examples', 'DateTime.mxDateTime'],
       package_dir = {'DateTime': ''},

       ext_modules = [Extension('DateTime.mxDateTime.mxDateTime',
                                ['mxDateTime/mxDateTime.c'],
                                include_dirs=['mxDateTime']),
                     ]
      )
