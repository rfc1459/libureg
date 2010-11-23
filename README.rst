.. -*- restructuredtext -*-

libuReg
=======

libuReg is a small regexp matching library based on Ken Thompson NFA method. It
can run any valid regexp in linear time and constant stack size, even those
considered to be pathological cases for backtracking-based matching engines.

Requirements
************
libuReg requires a C89-compliant C compiler (gcc is fine), GNU Bison, and
CMake.

The library has been tested on MacOSX 10.6, FreeBSD 8.0 and Debian 5.0, but it
should work on any modern POSIX-compliant operating system. Maybe it could work
under MinGW or Cygwin, but you're pretty much on your own (sorry, I don't own a
Win32 system).

Installation
************
Right now, libuReg is designed for static linking and in-tree shipping.

Usage with CMake-enabled projects
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Drop the libuReg sources in a subdirectory below your project root and add a
reference in CMakeLists.txt:

::
 
 ADD_SUBDIRECTORY(libureg)
 TARGET_LINK_LIBRARIES(your-target ureg)

Usage with automake projects
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Sorry, guys, you're on your own...

Syntax and limitations
**********************
libuReg has mostly the same syntax of POSIX EREs, with a few caveats:
 * backreferences are not supported (sorry folks, they're NP-complete);
 * capturing groups are not supported, they behave just like non-capturing
   groups;
 * POSIX named character classes are not supported and never will be;
 * bracket expressions do not yet support negative matching;
 * no assertions and anchors (I didn't need them), all patterns are strictly
   unanchored;
 * no counted repetitions (yet);
 * non-greedy operators are supported, although they are mostly useless.

Please keep in mind this is experimental code code.

Known bugs
**********
When a syntax error is encountered, the parser will call ``exit()`` instead of
relying on a user-defined error callback.

Development
***********
Issue tracking, wiki and mercurial repository can be found at the `project's page <http://bitbucket.org/rfc1459/libureg/>`_.

Credits
*******

**Author:** `Matteo Panella <morpheus@level28.org>`_.

Heavily inspired by and based on `RE1 <http://code.google.com/p/re1/>`_ by Russ Cox.

References
**********
 * `Implementing Regular expressions <http://swtch.com/~rsc/regexp/>`_
 * `RE1 - toy regular expression implementation <http://code.google.com/p/re1/>`_
 * `Plan 9 grep <http://swtch.com/usr/local/plan9/src/cmd/grep/>`_ (written by Ken Thompson)
 * `The Single UNIX® Specification, Version 2 - Regular Expressions <http://www.opengroup.org/onlinepubs/007908799/xbd/re.html>`_
