/* Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "ureg.h"

static Inst *pc;
static int count(Regexp *);
static void emit(Regexp *);

/* Compile an AST into an instruction stream */
Prog*
compile(Regexp *r)
{
	int n;
	Prog *p;
	
	n = count(r) + 1;
	p = (Prog *)mal(sizeof(Prog) + n*sizeof(p->start[0]));
	p->start = (Inst *)(p+1);
	pc = p->start;
	emit(r);
	pc->opcode = Match;
	pc++;
	p->len = pc - p->start;
	return p;
}

static int
count(Regexp *r)
{
	switch(r->type)
	{
		default:
			fatal("PUPPA/2!");
			break;
		case Alt:
			return 2 + count(r->left) + count(r->right);
			break;
		case Cat:
			return 2 + count(r->left) + count(r->right);
			break;
		case Lit:
		case Dot:
		case Range:
			return 1;
			break;
		case Quest:
			return 1 + count(r->left);
			break;
		case Star:
			return 2 + count(r->left);
			break;
		case Plus:
			return 1 + count(r->left);
			break;
	}
	/* Not reached */
}

static void
emit(Regexp *r)
{
	Inst *p1, *p2, *t;
	
	switch(r->type)
	{
		default:
			fatal("bad emit (PUPPA/3!)");
			break;
			
		case Alt:
			pc->opcode = Split;
			p1 = pc++;
			p1->x = pc;
			emit(r->left);
			pc->opcode = Jmp;
			p2 = pc++;
			p1->y = pc;
			emit(r->right);
			p2->x = pc;
			break;
		
		case Cat:
			emit(r->left);
			emit(r->right);
			break;
		
		case Lit:
			pc->opcode = Char;
			pc->c = r->ch;
			pc++;
			break;
		
		case Dot:
			pc->opcode = Any;
			pc++;
			break;
		
		case Range:
			pc->opcode = Rng;
			pc->lo = r->lo;
			pc->hi = r->hi;
			pc++;
			break;
		
		case Quest:
			pc->opcode = Split;
			p1 = pc++;
			p1->x = pc;
			emit(r->left);
			p1->y = pc;
			if(r->n)
			{
				/* Non-greedy */
				t = p1->x;
				p1->x = p1->y;
				p1->y = t;
			}
			break;
			
		case Star:
			pc->opcode = Split;
			p1 = pc++;
			p1->x = pc;
			emit(r->left);
			pc->opcode = Jmp;
			pc->x = p1;
			pc++;
			p1->y = pc;
			if(r->n)
			{
				t = p1->x;
				p1->x = p1->y;
				p1->y = t;
			}
			break;
			
		case Plus:
			p1 = pc;
			emit(r->left);
			pc->opcode = Split;
			pc->x = p1;
			p2 = pc;
			pc++;
			p2->y = pc;
			if(r->n)
			{
				t = p2->x;
				p2->x = p2->y;
				p2->y = t;
			}
			break;
	}
}

void
printprog(Prog *p)
{
	Inst *pc, *e;
	
	pc = p->start;
	e = p->start + p->len;
	
	for(; pc < e; pc++)
	{
		switch(pc->opcode)
		{
			default:
				fatal("bad opcode (PUPPA/4!)");
				break;
			case Split:
				printf("%2d. split %d, %d\n", (int)(pc-p->start), (int)(pc->x-p->start), (int)(pc->y-p->start));
				break;
			case Jmp:
				printf("%2d. jmp %d\n", (int)(pc-p->start), (int)(pc->x-p->start));
				break;
			case Char:
				printf("%2d. char '%c'\n", (int)(pc-p->start), pc->c);
				break;
			case Rng:
				printf("%2d. rng '%c', '%c'\n", (int)(pc-p->start), pc->lo, pc->hi);
				break;
			case Any:
				printf("%2d. any\n", (int)(pc-p->start));
				break;
			case Match:
				printf("%2d. match\n", (int)(pc-p->start));
				break;
		}
	}
}