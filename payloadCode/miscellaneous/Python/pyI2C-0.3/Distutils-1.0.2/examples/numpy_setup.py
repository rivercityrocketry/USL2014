#!/usr/bin/env python

# Setup script for building the Numeric extension to Python.

# To use:
#   * run this to build Numerical Python:
#       python setup.py build
#     or this to build and install it
#       python setup.py install


# created 1999/08 Perry Stoll
# modified 2000/01 Paul Dubois 
import os
from glob import glob
from distutils.command.install import install
from distutils.core import setup, Extension
from distutils.sysconfig import get_python_inc

headers = glob (os.path.join ("Include","*.h"))

setup (name = "Numerical",
       version = "15.3",
       maintainer = "Paul Dubois",
       maintainer_email = "dubois@users.sourceforge.net",
       description = "Numerical Extension to Python",
       url = "http://numpy.sourceforge.net",

       packages = [''],
       package_dir = {'': 'Lib'},
       extra_path = 'Numeric',
       include_dirs = ['Include'],
       headers = headers,
       ext_modules = [Extension('_numpy',
                                ['Src/_numpymodule.c',
                                 'Src/arrayobject.c',
                                 'Src/ufuncobject.c']),
                      Extension('multiarray', ['Src/multiarraymodule.c']),
                      Extension('umath', ['Src/umathmodule.c']),
                      Extension('fftpack',
                                ['Src/fftpackmodule.c',
                                 'Src/fftpack.c']),
                      Extension('lapack_lite',
                                ['Src/lapack_litemodule.c',
                                 'Src/dlapack_lite.c',
                                 'Src/zlapack_lite.c',
                                 'Src/blas_lite.c',
                                 'Src/f2c_lite.c']),
                      Extension('ranlib',
                                ['Src/ranlibmodule.c',
                                 'Src/ranlib.c',
                                 'Src/com.c',
                                 'Src/linpack.c']),
                      Extension('arrayfns',
                                ['Src/arrayfnsmodule.c'])
                     ]
       )
