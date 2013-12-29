#!/usr/bin/env python

"""Setup script to test/demonstrate extensibility of the Distutils
   by extending an existing command class."""

from distutils.command.build import build
from distutils.core import setup, Command


class mybuild (build):

    def run (self):
        self.run_peer ('greet')
        build.run (self)

# class mybuild


class greet (Command):

    user_options = [('name=', None,
                     "your first name"),
                    ('message=', 'm',
                     "message to print")]

    def initialize_options (self):
        self.name = None
        self.message = None

    def finalize_options (self):
        if self.message is None:
            if self.name is not None:
                self.message = "Hello, %s!" % self.name
            else:
                self.message = "Howdy, stranger"

    def run (self):
        self.announce (self.message)

# class greet


setup (# Distribution meta-data
       name = "sample",
       version = "1.0",
       description = "Distutils sample distribution #1 (alt. setup script)",

       # Description of the modules and packages in the distribution
       packages = [''],                 # empty string means root "package"

       # Overridden command classes
       cmdclass = {'build': mybuild,
                   'greet': greet}
      )
