
# Setup script for the XML tools.  Stolen from the PyXML 0.5.3
# distribution and hacked up to keep up with incompatible
# changes in the Distutils post-0.1 (and pre-1.0).  Also simplified
# to assume that the Distutils are available.

__revision__ = "$Id: xml_setup.py,v 1.2 2000/06/29 02:22:02 gward Exp $"


#
# Targets: build test install help

import sys, os
from distutils.core import setup, Extension


# XXX should detect whether to use the unixfilemap or readfilemap
# depending on the platform
FILEMAP_SRC = 'extensions/expat/xmlwf/unixfilemap.c'

setup (name = "PyXML",
       version = "0.5.3",
       description = "Python/XML package",
       author = "XML-SIG",
       author_email = "xml-sig@python.org",
       url = "http://www.python.org/sigs/xml-sig/",

       packages = ['xml', 'xml.dom', 'xml.marshal',
                   'xml.parsers', 'xml.parsers.xmlproc', 
                   'xml.sax', 'xml.sax.drivers',
                   'xml.unicode', 'xml.utils'
                   ],

       ext_modules = [Extension('sgmlop',
                                ['extensions/sgmlop.c']),
                      Extension('xml.unicode.wstrop',
                                ['extensions/wstrop.c']),
                      Extension('xml.parsers.pyexpat',
                                ['extensions/pyexpat.c',
                                 'extensions/expat/xmltok/xmltok.c',
                                 'extensions/expat/xmltok/xmlrole.c',
                                 'extensions/expat/xmlwf/xmlfile.c',
                                 'extensions/expat/xmlwf/xmlwf.c',
                                 'extensions/expat/xmlwf/codepage.c',
                                 'extensions/expat/xmlparse/xmlparse.c',
                                 'extensions/expat/xmlparse/hashtable.c',
                                 FILEMAP_SRC,],
                                define_macros = [('XML_NS', None)],
                                include_dirs = [ 'extensions/expat/xmltok',
                                                 'extensions/expat/xmlparse' ],
                                )
                      ]
       )
