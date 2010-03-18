/* Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license
 */

#ifndef INCLUDED_stdinc_h
#define INCLUDED_stdinc_h

#include "setup.h"

/* alloca() handling */
#if defined(__GNUC__)
#undef alloca
#define alloca __builtin_alloca
#else
# ifdef _MSC_VER
#  include <malloc.h>
#  define alloca _alloca
# else
#  if HAVE_ALLOCA_H
#   include <alloca.h>
#  else
#   ifdef _AIX
#pragma alloca
#   else
#    ifndef alloca
#error "Your platform does not support alloca()"
#    endif
#   endif
#  endif
# endif
#endif

/* stdlib.h */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

/* string.h/strings.h */
#ifdef HAVE_STRING_H
# include <string.h>
#else
# ifdef HAVE_STRINGS_H
#  include <strings.h>
# endif
#endif

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#endif /* INCLUDED_stdinc_h */
