/* Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license
 */

#ifndef INCLUDED_ureg_h
#define INCLUDED_ureg_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

typedef struct Regexp Regexp;
typedef struct Prog Prog;
typedef struct Inst Inst;

/* An AST node */
struct Regexp
{
	int type;
	int n;
	int ch;
	int lo, hi;
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
	Plus
};

extern Regexp *parse(char *);
extern Regexp *reg(int, Regexp *, Regexp *);
extern void reg_destroy(Regexp *);
extern void printre(Regexp *);
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
extern void printprog(Prog *);

extern int thompsonvm(Prog *, char *);

#endif /* INCLUDED_ureg_h */
