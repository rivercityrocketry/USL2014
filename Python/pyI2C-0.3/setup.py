#!/usr/bin/env python
from distutils.core import setup

from I2C import version

import os
if os.name == 'nt':
    data_files = {'parallel': ['simpleio.dll']}
else:
    data_files = {}

setup (name = "pyI2C",
    description="Python I2C Port Extension",
    version=version,
    author="Patrick Nomblot",
    author_email="pyI2C@nomblot.org",
    url="http://pyI2C.sourceforge.net",
    packages=['I2C', 'I2C.drivers'],
    license="Python",
    long_description="Pure Python I2C Port Extension",
    package_data = data_files
)
