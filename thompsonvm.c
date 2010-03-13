/* Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#define UREG_INTERNAL
#include "ureg-internal.h"

typedef struct Thread Thread;
struct Thread
{
	Inst *pc;
};

typedef struct ThreadList ThreadList;
struct ThreadList
{
	int n;
	Thread t[1];
};

static Thread
thread(Inst *pc)
{
	Thread t = {pc};
	return t;
}

static ThreadList*
threadlist(int n)
{
	ThreadList *l = mal(sizeof(ThreadList) + n*sizeof(Thread));
}

static void
destroylist(ThreadList *l)
{
	if(l == NULL)
		return;
	free(l);
}

static void
addthread(ThreadList *l, Thread t, int gen)
{
	if(t.pc->gen == gen)
		return;
	t.pc->gen = gen;
	l->t[l->n] = t;
	l->n++;
	
	switch(t.pc->opcode)
	{
		case Jmp:
			addthread(l, thread(t.pc->x), gen);
			break;
		case Split:
			addthread(l, thread(t.pc->x), gen);
			addthread(l, thread(t.pc->y), gen);
			break;
	}
}

int
thompsonvm(Prog *prog, const char *input)
{
	int i, len, matched, gen;
	ThreadList *clist, *nlist, *tmp;
	Inst *pc;
	const char *sp;
	
	len = prog->len;
	clist = threadlist(len);
	nlist = threadlist(len);
	
	gen = 1;
	addthread(clist, thread(prog->start), gen);
	matched = 0;
	for(sp = input; ; sp++)
	{
		if(clist->n == 0)
			break;
		gen++;
		for(i = 0; i < clist->n; i++)
		{
			pc = clist->t[i].pc;
			switch(pc->opcode)
			{
				case Char:
					if(*sp != pc->c)
						break;
					addthread(nlist, thread(pc+1), gen);
					break;
				case Rng:
					if(*sp < pc->lo || *sp > pc->hi)
						break;
				case Any:
					if(*sp == 0)
						break;
					addthread(nlist, thread(pc+1), gen);
					break;
				case Match:
					matched = 1;
					goto BreakFor;		/* I know, it's ugly. Sue me */
			}
		}
BreakFor:
		tmp = clist;
		clist = nlist;
		nlist = tmp;
		nlist->n = 0;
		if(*sp == '\0')
			break;
	}
	destroylist(nlist);
	destroylist(clist);
	return matched;
}
