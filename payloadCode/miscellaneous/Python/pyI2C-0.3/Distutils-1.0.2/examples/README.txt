Distutils Examples
==================

This directory contains example setup scripts to go with several popular
Python module distributions: Numerical Python, mxDateTime, PIL (the
Python Imaging Library), and PyXML.  (Why these three?  Well, the
Numerical Python example was originally done by Perry Stoll when he
wrote the msvccompiler module, so I gleefuly stole and modified it.
mxDateTime and PIL happened to be lying around on my hard drive when it
came time to write some more examples, so here they are.  The PyXML
setup script was actually written by Andrew Kuchling and stolen by me,
rather than the other way round -- I added it to the Distutils examples
collection because it's another useful example, and because
incompatibilities in the Distutils broke Andrew's setup script; the
version distributed here is modified to work with the current Distutils
(and simplified to assume that the Distutils are available).)

Trying out the example setup scripts is generally simple: just pick one,
copy it into the top-level directory of the corresponding source
distribution as "setup.py", and run it like

   python setup.py build

if you just want to build extensions and copy .py files around, or

   python setup.py install

if you want to go all the way and install the thing.

A few caveats:

  * The example setup scripts for Numerical Python and PyXML are
    actually required to build NumPy 15.2 and PyXML 0.5.3 (the current
    versions as I write this), because of those incompatibilities
    in the Distutils.

  * For PIL, the example setup script builds and installs PIL in
    package form.  However, PIL is not yet fully packagized; its
    extension module, _imaging, is a top-level module.  Thus, it
    will wind up right in your site-packages directory, while the
    rest of PIL will be in the "PIL" package directory.

Finally, if you want to create a setup script for your own module
distribution, a good starting would be "template_setup.py" in this
directory.  Consult the "Distributing Python Modules" manual for details
on just what every option means, or the three real-world examples if you
just want a rough idea of what's going on.

$Id: README.txt,v 1.5 2000/04/12 01:51:34 gward Exp $
