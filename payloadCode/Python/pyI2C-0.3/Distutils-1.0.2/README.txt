                     Python Distribution Utilities
                             release 1.0.2
                            April 20th, 2001


INTRODUCTION
------------

The Python Distribution Utilities, or Distutils for short, are a
collection of modules that aid in the development, distribution, and
installation of Python modules.  (It is intended that ultimately the
Distutils will grow up into a system for distributing and installing
whole Python applications, but for now their scope is primarily module
distributions.)

The Distutils are a standard part of Python versions 1.6 and higher;
if you are running 1.6/2.0/2.1, you don't need to install the
Distutils separately.  This release is primarily so that you can add
the Distutils to a Python 1.5.2 installation -- you will then be able
to install modules that require the Distutils, or use the Distutils to
distribute your own modules.

More information is available at the Distutils web page:

    http://www.python.org/sigs/distutils-sig/

Any feedback, comments, bug reports, success reports, etc. should be
reported to the Distutils SIG at

    distutils-sig@python.org

If you're not yet a member of this mailing list, you can join at

    http://www.python.org/mailman/listinfo/distutils-sig

Reports of minor bugs (patches are especially welcome!) can be sent
directly to me (Greg Ward) at gward@python.net.  Anything that expects a
longer response than "Thanks for the bug report/fix!" should be directed
to the SIG.


REQUIREMENTS
------------

This release of the Distutils requires Python 1.5.2 or later.

To use the Distutils under Unix, you must have a *complete* Python
installation, including the Makefile and config.h used to build Python.
These should be in <exec-prefix>/lib/pythonX.Y/config.  (<exec-prefix>
is the value of the --exec-prefix option to Python's configure script,
or the --prefix option if --exec-prefix wasn't supplied; if neither was
supplied, the default is "/usr/local".  If you don't know your
<exec-prefix>, fire up the Python interpreter and type
"import sys ; sys.exec_prefix".)

Certain Linux distributions break Python up into multiple packages; you
should make sure you have all required packages installed:

   Red Hat            python, python-devel
   Mandrake           python, python-devel
   SuSE               python
   Debian             python-base, python-dev

In order to build extensions on any platform, you will need a C/C++
compiler.  On Unix, the compiler used to build Python itself will be
used by default, and should do the trick; if you didn't build Python,
then hopefully you'll find out pretty quickly whether building
extensions works or not.

Building extensions on Windows works best with Microsoft Visual C++ 5.0
or 6.0.  It also helps to have access to the Windows registry from
Python; if you have the Win32 extensions (win32api, win32con) installed,
you're fine.  (Python 1.6 and 2.0 include the winreg module for this
purpose, which the Distutils will use if available.)  If not, the
Distutils might not be able to find the Visual C++ executables, in which
case it will die horribly when you attempt to build any Python
extensions.

There is also experimental support for building extensions under Windows
using Borland C++ or GCC (Cygwin or Mingw32 ports).  Come join the
Distutils SIG to learn about using these compilers.


INSTALLATION UNDER PYTHON 1.5.2
-------------------------------

Obviously, in order to use the Distutils you must first install them.
Since the goal of the whole project is to make distributing and
installing Python module distributions painless, this is quite easy:

    python setup.py install

Note that this installs to the "site" library directory of your local
Python installation: /usr/local/lib/python1.5/site-packages by default
on Unix, "C:\Program Files\Python" by default on Windows.  Since the
Distutils are "package-ized", the installation process will create a
subdirectory "distutils" under the site library directory.

The installation is by default quite verbose.  You can silence it with
the "-q" option:

    python setup.py -q install

but unfortunately the Distutils' verbosity is (currently) all or
nothing.


INSTALLATION UNDER PYTHON 1.6/2.0
---------------------------------

The Distutils have been included with Python since 1.6a1; the
correspondence between Python releases and Distutils releases is as
follows:

     Python release:               Distutils release:
        1.6a1                        pre 0.8
        1.6a2                        0.8
        1.6b1                        0.9
        1.6                          0.9.1
        2.0b1                        0.9.2
        2.0b2                        0.9.3
        2.0                          1.0.1
        2.1                          1.0.2

There's generally no need to install the Distutils under Python
1.6/2.0/2.1.  However, if you'd like to upgrade the Distutils in your
Python 1.6 installation, or if future Distutils releases get ahead of
the Distutils included with Python, you might want to install a newer
Distutils release into your Python installation's library.

To do this, you'll need to hide the original Distutils package directory
from Python, so it will find the version you install.  For example,
under a typical Unix installation, the "stock" Distutils directory is
/usr/local/lib/python1.6/distutils; you could hide this from Python as
follows:

    cd /usr/local/lib/python1.6        # or 2.0
    mv distutils distutils-orig

On Windows, the stock Distutils installation is "Lib\distutils" under
the Python directory ("C:\Python" by default with Python 1.6a2 and
later).  Again, you should just rename this directory, eg. to
"distutils-orig", so that Python won't find it.

Once you have renamed the stock Distutils directory, you can install the
Distutils as described above.


USAGE
-----

The Distutils are intended to have three user communities: developers,
packagers, and installers.  Distutils 0.9 was the first release to cater
seriously to all three communities: developers can use it to build and
install their modules, as well as create source distributions; packagers
can use it to create RPMs and executable installers for Windows; and of
course installers can build and install modules from source (or just use
an installer created by some kind packager).

Documentation for the Distutils is under construction in the form of two
manuals, "Installing Python Modules" (for installers) and "Distributing
Python Modules" (for developers and packagers).  I've included the LaTeX
source for these two manuals in the "doc" subdirectory; if you know your
way around LaTeX, the Python documentation tools, and Unix, you might be
able to get something out of these.  Realistically, though, the
documentation is just provided in the distribution so you can send me
doc patches; if you want to read it, you're better off getting the
latest documentation from the Distutils documentation page:

    http://www.python.org/sigs/distutils-sig/doc/

These two manuals are also part of the standard Python documentation
set; Fred Drake maintains a copy of the complete Python documentation at

    http://python.sourceforge.net/devel-docs/

Sometimes Fred's version is more recent, and sometimes my version is.
Join both doc-sig@python.org and distutils-sig@python.org if you really
want to follow the latest documentation developments.

If you are an installer (system administrator or end-user) and you'd
like to try out the Distutils, you've already done so by installing the
Distutils themselves (thanks!).  Additionally, a number of module
distributions now use the Distutils for installation; if you have
installed the Distutils just to get another distribution up-and-running,
follow the instructions included with that distribution.  For any
"distutil-ized" distribution, though, this should always work:

    python setup.py install


EXAMPLES
--------

For developers who need to convert their own module distributions to use
the Distutils, I've included several example setup scripts in the
"examples" subdirectory.  Two of these (numpy_setup.py and xml_setup.py)
are already in production use -- in fact, xml_setup.py was stolen from
the PyXML distribution, rather than the other way around.  I am
distributing modified versions in order to provide setup scripts for
these distributions that work with Distutils 0.8 and later, and to show
some real-world examples.

If you want to try one of the example setup scripts in the real world,
you'll obviously have to have the appropriate module distribution
available: Numerical-15.3 (numpy_setup.py), PyXML-0.5.3 (xml_setup.py),
Imaging-1.0 (pil_setup.py), or mxDateTime-1.3.0 (mxdatetime_setup.py).
All you have to do is:

  * copy (or link) the provided setup script to "setup.py" in the
    target distribution (eg. copy "examples/pil_setup.py" to "setup.py"
    in Imaging-1.0, wherever you unpacked the Imaging source
    distribution)

  * run the setup script:
        python setup.py install
    to build and install the distribution to your standard Python
    library directory

Also, as always you can split the setup run up into "build" and
"install" steps:

    python setup.py build
    python setup.py install

This is handy if you just want to be sure a given module distribution
will build without actually installing it.

You can also install to a non-standard location, eg. using the --prefix
or --home options to the 'install' command:

    python setup.py install --prefix=/tmp/usr
    python setup.py install --home=/home/greg

See the "Installing Python Modules" manual for more information.
See examples/README.txt for more information on the example setup
scripts included with the Distutils.


MORE INFORMATION
----------------

I presented a paper at the Eighth International Python Conference which
covers the usage, design, and architecture of the Distutils.  The
published paper has several usage examples, which might be useful until
the real documentation is more polished.  The paper was updated
slightly and submitted to the Software Carpentry competition; you can
find both versions at the Distutils documentation page:

    http://www.python.org/sigs/distutils-sig/doc/

where you'll find links to PDF, PostScript, and HTML versions of the
paper, as well as the slides from my presentation.


BUGS AND LIMITATIONS
--------------------

The good news is that every bug/limitation that was present in this list 
in Distutils 0.1.x has been fixed, or at least addressed.  However,
there are a few outstanding problems:

  * problems with specifying relative directories in an installation
    scheme
  * Mac OS support only partially included
  * no test suite
  * doesn't check to see if you're clobbering an existing module
    installation

There are some major features that still need to be added:

  * no knowledge of whether you have installed some module distribution
    before
  * no support for running test suites
  * no support for processing documentation (and no standard
    documentation format for Python modules!)

If you think of any limitations that aren't on the above list, or find
serious bugs, or have contributions to make, come and join the Distutils 
SIG:

    http://www.python.org/mailman/listinfo/distutils-sig

Also, take a look through the TODO file to see a more recent/complete
version of the Distutils to-do list.


CONTRIBUTING
------------

Yes, please!  A good place to start is by reading the IPC paper and/or
presentation slides that I mentioned above.  You should probably also
read the requirements and design documents at

    http://www.python.org/sigs/distutils-sig/

if you're serious about working on the code.

Small patches can be sent to the Distutils-SIG,

    distutils-sig@python.org

for discussion by the group.  Anything larger than 200-300 lines should
be sent straight to me (gward@python.net) to reduce volume on the list.


ACKNOWLEDGMENTS
---------------

[tangible, in roughly chronological order]
  * Fred Drake: the sysconfig module
  * Amos Latteier: NT support in sysconfig
  * Perry Stoll: the MSVCCompiler class and the example setup
    script for Numerical Python, 
  * Robin Becker: registry support in msvccompiler.py 
  * Thomas Heller: more work on the registry support, several
    bug fixes in the Windows support, and just generally improving
    the code for compiling extensions on Windows; also, the
    "bdist_wininst" command (and associated C code)
  * Eric Tiedemann: bug fixes
  * Fred Drake and Guido van Rossum: helping to figure out the
    "right way" to install third-party Python modules
  * Joe Van Andel: tweaks to the sysconfig module, misc. bug fixes
  * Corran Webster: Mac OS support in general
  * Bastian Kleineidam: a bunch of small but vital commands: clean,
    install_scripts, install_data, build_scripts; a pile of patches,
    bug-fixes, and good ideas, large and small
  * Lyle Johnson: bug-spotting and -fixing; support for Borland's C/C++
    compiler
  * Harry Henry Gebel: bug-spotting and -fixing; the "bztar" archive
    format; the "bdist_rpm" command
  * Rene Liebscher: smarter extension-building; Cygwin/Mingw32 support;
    more help options; general improvement to the CCompiler classes;
    lots of other patches and bug reports

[spiritual, in roughly chronological order since the birth of the project]
  * Fred Drake and Andrew Kuchling for agreeing with me that the lack of
    a standard build mechanism is a serious hole in Python
  * the crowd at that Developer's Day session (notably Greg Stein,
    Eric Raymond, Andrew Kuchling, and a few others who I've probably
    forgotten) for a lively, stimulating, and productive (!) discussion
  * David Ascher, Guido van Rossum, and Paul Dubois for occasionally
    prodding me back to work on the Distutils
  * Paul Dubois, Andrew Kuchling, and other early adopters for finding
    bugs (and for taking a chance in general)

[spiritual, before the project ever came to be]
  * various perl5-porters: Andy Dougherty, Andreas König, Tim Bunce
    for showing the way with Perl's ExtUtils family of modules
  * Tim Bunce, Andreas König, and Jarkko Hietaniemi for showing
    the potential by developing the Perl Module List (Tim & Andreas),
    the CPAN archive (Jarkko), and the CPAN module (Andreas)

$Id: README.txt,v 1.24 2001/04/20 18:30:36 akuchling Exp $
