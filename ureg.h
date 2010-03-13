/* ureg.h - libureg public API
 *
 * Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license
 */

#ifndef INCLUDED_ureg_h
#define INCLUDED_ureg_h

/* Opaque handler to a compiled regexp */
typedef struct ureg_regexp_t *ureg_regexp;

/* Compile a regexp and return an handle for use with ureg_match */
extern ureg_regexp ureg_compile(const char *, unsigned int);

/* Destroy a previously created regexp and free its memory */
extern void ureg_free(ureg_regexp);

/* Match a string against a given compiled regexp */
extern int ureg_match(ureg_regexp, const char *);

/* Return the original string for a given regexp - MAY NOT BE FREED! */
extern const char *ureg_txt(ureg_regexp);

#endif /* INCLUDED_ureg_h */
