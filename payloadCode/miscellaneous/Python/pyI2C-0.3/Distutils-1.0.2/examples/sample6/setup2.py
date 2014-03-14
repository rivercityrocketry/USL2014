#
# sample6, setup2
#
# test that 'package_dir' works with package names and the root package
#

from distutils.core import setup
setup (name = "sample 6",
       description = "Distutils Sample #6",
       packages = ['', 'pkg'],
       package_dir = {'': 'lib'},
       options = {'build': {'build_base': 'build2'}},
      )
