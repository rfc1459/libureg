/* ureg.c - public API
 *
 * Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ureg.h"
#define UREG_INTERNAL
#include "ureg-internal.h"

/* Actual declaration of struct ureg_regexp_t */
struct ureg_regexp_t
{
	/* Original text representation */
	const char *txt;
	/* Compiled regexp (intermediate AST is not saved) */
	Prog *p;
};

/* Compile a regexp and return an handler */
ureg_regexp
ureg_compile(const char *pattern, unsigned int flags)
{
	Regexp *r;
	struct ureg_regexp_t *res;
	if(pattern == NULL)
	{
		/* TODO: ureg_errno = UREG_ERR_NULL; */
		return NULL;
	}

	/* Parse the regexp and build the equivalent AST */
	if((r = parse(pattern)) == NULL)
	{
		/* TODO: ureg_errno = UREG_ERR_SYNTAX; */
		return NULL;
	}
	res = (struct ureg_regexp_t *)malloc(sizeof(struct ureg_regexp_t));
	if(res == NULL)
	{
		/* TODO: ureg_errno = UREG_ERR_NOMEM; */
		reg_destroy(r);
		return NULL;
	}

	/* Compile the AST into the final NFA program */
	if((res->p = compile(r)) == NULL)
	{
		/* TODO: ureg_errno = UREG_ERR_COMPILE; */
		reg_destroy(r);
		free(res);
		return NULL;
	}
	
	/* Success, throw away the AST, duplicate text form and return */
	reg_destroy(r);
	res->txt = strdup(pattern); /* FIXME: check for OOM */
	/* TODO: ureg_errno = UREG_SUCCESS; */
	return res;
}

/* Release a regexp */
void
ureg_free(ureg_regexp handle)
{
	if(handle == NULL)
	{
		/* TODO: ureg_errno = UREG_ERR_NULL; */
		return;
	}
	if(handle->txt)
		free((char *)handle->txt);
	if(handle->p)
		free(handle->p);
	free(handle);
	/* TODO: ureg_errno = UREG_SUCCESS; */
}

/* Match a string against a regexp */
int
ureg_match(ureg_regexp handle, const char *s)
{
	if(handle == NULL || s == NULL || handle->p == NULL)
	{
		/* TODO: ureg_errno = UREG_ERR_NULL; */
		return -1;
	}
	/* TODO: ureg_errno = UREG_SUCCESS; */
	return thompsonvm(handle->p, s);
}

/* Return a string representation of a given regexp */
const char *
ureg_txt(ureg_regexp handle)
{
	if(handle == NULL || handle->txt == NULL)
	{
		/* TODO: ureg_errno = UREG_ERR_NULL; */
		return NULL;
	}
	/* TODO: ureg_errno = UREG_SUCCESS; */
	return handle->txt;
}
