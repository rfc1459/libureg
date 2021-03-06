libuReg
=======

libuReg is a small regexp matching library based on Ken Thompson NFA method. It
can run any valid regexp in linear time and constant stack size, even those
considered to be pathological cases for backtracking-based matching engines.


Requirements
------------

libuReg requires a C89-compliant C compiler (gcc is fine) and CMake.

The library has been tested on MacOSX 10.6, FreeBSD 8.1 and Debian squeeze, but it
should work on any modern POSIX-compliant operating system. Maybe it could work
under MinGW or Cygwin, but you're pretty much on your own.


Installation
------------

Right now, libuReg is designed for static linking and in-tree shipping.

### Usage with CMake-enabled projects ###

Drop the libuReg sources in a subdirectory below your project root and add a
reference in CMakeLists.txt:

    ADD_SUBDIRECTORY(libureg)
    TARGET_LINK_LIBRARIES(your-target ureg)

### Usage with autotools projects ###

TBD, for the moment you're on your own.


Syntax and limitations
----------------------

libuReg has mostly the same syntax of POSIX EREs, with a few caveats:

  * backreferences are not supported (sorry folks, they're NP-complete);
  * capturing groups are not supported, they behave just like non-capturing
    groups;
  * POSIX named character classes are not supported and never will be;
  * bracket expressions do not yet support negative matching;
  * no assertions and anchors (I didn't need them), all patterns are strictly
    unanchored;
  * non-greedy operators are supported, although they are mostly useless.

Please keep in mind this is experimental code.


Known bugs
----------

When the compiler hits an unknown AST node, it will call `exit()` instead of
relying on a user-defined error callback.


Development
-----------

Issue tracking, wiki and git repository can be found at the [project's page on github][libureg].


Credits
-------

**Author:** [Matteo Panella](https://github.com/rfc1459/).

Heavily inspired by and based on [RE1][] by Russ Cox.


References
----------

  * [Implementing Regular expressions][regexp]
  * [RE1 - toy regular expression implementation][RE1]
  * [Plan 9 grep][p9grep] (written by Ken Thompson)
  * [The Single UNIX® Specification, Version 2 - Regular Expressions][SUSv2]

[libureg]: https://github.com/rfc1459/libureg/
[RE1]: http://code.google.com/p/re1/
[regexp]: http://swtch.com/~rsc/regexp/
[p9grep]: http://swtch.com/usr/local/plan9/src/cmd/grep/ "Yes, THAT Ken Thompson"
[SUSv2]: http://www.opengroup.org/onlinepubs/007908799/xbd/re.html
