#!/usr/bin/env python

#
# sample5 -- try out the "install_data" command
#

from distutils.core import setup

setup (name = "sample5",
       description = "Distutils Sample #5",
       data_files = [("share", ["data"]),
                     ("etc", ["config"]),
                     ("/etc", ["sys_config"])])
