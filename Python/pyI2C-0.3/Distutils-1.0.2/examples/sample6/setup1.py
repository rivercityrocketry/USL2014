#
# sample6, setup1
#
# test that 'package_dir' works with individual module names and the
# root package
#

from distutils.core import setup
setup (name = "sample 6",
       description = "Distutils Sample #6",
       py_modules = ['mod1', 'pkg.mod2'],
       package_dir = {'': 'lib'},
       options = {'build': {'build_base': 'build1'}},
      )
