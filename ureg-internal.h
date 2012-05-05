/* Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license
 */

#ifndef INCLUDED_ureg_internal_h
#define INCLUDED_ureg_internal_h

#ifndef UREG_INTERNAL
#error "ureg-internal.h should not be used directly, use ureg.h instead"
#endif /* UREG_INTERNAL */

#define UNUSED_PARAMETER(x) (void)(x)

typedef struct Regexp Regexp;
typedef struct Prog Prog;
typedef struct Inst Inst;
typedef struct Parse Parse;

/* Parser status */
struct Parse
{
    int parseError;
    int nparen;
    Regexp *ast_root;
};

extern void *uregParserAlloc(void *(*mallocProc)(size_t));
extern void uregParserFree(void *p, void (*freeProc)(void*));
extern void uregParser(void *, int, int, Parse *);
#ifndef NDEBUG
extern void uregParserTrace(FILE *, char *);
#endif

/* An AST node */
struct Regexp
{
    int type;
    int n;
    int ch;
    int lo, hi;
    int refc;
    Regexp *left;
    Regexp *right;
};

/* AST node types (Regexp.type) */
enum
{
    Alt = 1,
    Cat,
    Lit,
    Dot,
    Range,
    Quest,
    Star,
    Plus,
    Paren
};

extern Regexp *parse(const char *);
extern Regexp *reg(int, Regexp *, Regexp *);
extern void reg_destroy(Regexp *);
#define reg_incref(r)               \
    do {                            \
        if (r)                      \
            (r)->refc++;            \
    } while (0)
#define reg_decref(r)               \
    do {                            \
        if (r)                      \
        {                           \
            (r)->refc--;            \
                if ((r)->refc <= 0) \
                reg_destroy(r);     \
        }                           \
    } while (0)

extern Regexp *simplify_repeat(Regexp *, int, int, int);
#if !defined(NDEBUG) && defined(UREG_TRACE)
extern void printre(Regexp *);
#endif
extern void fatal(char *, ...);
extern void *mal(size_t);

struct Prog
{
    Inst *start;
    int len;
};

struct Inst
{
    int opcode;
    int c;
    int n;
    int lo, hi;
    Inst *x;
    Inst *y;
    int gen;
};

/* Opcodes (Inst.opcode) */
enum
{
    Char = 1,
    Match,
    Jmp,
    Split,
    Any,
    Save,
    Rng
};

extern Prog *compile(Regexp *);
#if !defined(NDEBUG) && defined(UREG_TRACE)
extern void printprog(Prog *);
#endif

extern int thompsonvm(Prog *, const char *);

#endif /* INCLUDED_ureg_internal_h */
