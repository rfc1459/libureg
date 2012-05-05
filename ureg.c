/* ureg.c - public API
 *
 * Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license
 */

#include "stdinc.h"
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

ureg_error_t ureg_errno = UREG_NOERROR;

/* Compile a regexp and return an handler */
ureg_regexp
ureg_compile(const char *pattern, unsigned int flags)
{
    Regexp *r;
    struct ureg_regexp_t *res;
    if(pattern == NULL)
    {
        ureg_errno = UREG_ERR_NULL;
        return NULL;
    }

    /* Parse the regexp and build the equivalent AST */
    if((r = parse(pattern)) == NULL)
    {
        ureg_errno = UREG_ERR_SYNTAX;
        return NULL;
    }
    /* Keep the root node of the AST referenced */
    reg_incref(r);
#if !defined(NDEBUG) && defined(UREG_TRACE)
    fprintf(stderr, "AST: ");
    printre(r);
    fprintf(stderr, "\n");
#endif
    res = (struct ureg_regexp_t *)malloc(sizeof(struct ureg_regexp_t));
    if(res == NULL)
    {
        ureg_errno = UREG_ERR_NOMEM;
        reg_decref(r);
        return NULL;
    }

    /* Compile the AST into the final NFA program */
    if((res->p = compile(r)) == NULL)
    {
        ureg_errno = UREG_ERR_COMPILE;
        reg_decref(r);
        free(res);
        return NULL;
    }

    /* Success, throw away the AST, duplicate text form and return */
    reg_decref(r);
#if !defined(NDEBUG) && defined(UREG_TRACE)
    fprintf(stderr, "Program:\n");
    printprog(res->p);
#endif
    res->txt = strdup(pattern); /* FIXME: check for OOM */
    ureg_errno = UREG_NOERROR;
    return res;
}

/* Release a regexp */
void
ureg_free(ureg_regexp handle)
{
    if(handle == NULL)
    {
        ureg_errno = UREG_ERR_NULL;
        return;
    }
    if(handle->txt)
        free((char *)handle->txt);
    if(handle->p)
        free(handle->p);
    free(handle);
    ureg_errno = UREG_NOERROR;
}

/* Match a string against a regexp */
int
ureg_match(ureg_regexp handle, const char *s)
{
    if(handle == NULL || s == NULL || handle->p == NULL)
    {
        ureg_errno = UREG_ERR_NULL;
        return -1;
    }
    ureg_errno = UREG_NOERROR;
    return thompsonvm(handle->p, s);
}

/* Return a string representation of a given regexp */
const char *
ureg_txt(ureg_regexp handle)
{
    if(handle == NULL || handle->txt == NULL)
    {
        ureg_errno = UREG_ERR_NULL;
        return NULL;
    }
    ureg_errno = UREG_NOERROR;
    return handle->txt;
}
