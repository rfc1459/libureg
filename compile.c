/* Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license
 */

#include "stdinc.h"
#define UREG_INTERNAL
#include "ureg-internal.h"

static int count(Regexp *);
static void emit(Regexp *, Inst **);

/* Compile an AST into an instruction stream */
Prog*
compile(Regexp *r)
{
	int n;
	Prog *p;
	Inst *pc;
	
	n = count(r) + 1;
	p = (Prog *)mal(sizeof(Prog) + n*sizeof(p->start[0]));
	p->start = (Inst *)(p+1);
	pc = p->start;
	emit(r, &pc);
	pc->opcode = Match;
	pc++;
	p->len = pc - p->start;
	return p;
}

static int
count(Regexp *r)
{
	if (r == NULL)
		return 0;

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
		case CountedRep:
			if (r->lo == r->hi)
			{
				/* Base case: exact count */
				return r->lo * count(r->left);
			}
			else if (r->lo == -1)
			{
				/* Upper bound only
				 * eg: n{,3} -> n?n?n?
				 */
				return r->hi * (1 + count(r->left));
			}
			else if (r->hi == -1)
			{
				/* Lower bound only
				 * eg: n{3,} -> nnn+
				 */
				return 1 + r->lo * count(r->left);
			}
			else
			{
				/* Both bounds
				 * eg n{3,7} -> nnnn?n?n?n?
				 */
				int optcount = r->hi - r->lo;
				int lcount = count(r->left);
				return (optcount * (1 + lcount)) + (r->lo * lcount);
			}
			break;
	}
	/* Not reached */
}

static void
emit(Regexp *r, Inst **pc)
{
	Inst *p1, *p2, *t;
	int i;
	
	if (r == NULL)
		return;
	
	switch(r->type)
	{
		default:
			fatal("bad emit (PUPPA/3!)");
			break;
			
		case Alt:
			(*pc)->opcode = Split;
			p1 = (*pc)++;
			p1->x = *pc;
			emit(r->left, pc);
			(*pc)->opcode = Jmp;
			p2 = (*pc)++;
			p1->y = *pc;
			emit(r->right, pc);
			p2->x = *pc;
			break;
		
		case Cat:
			emit(r->left, pc);
			emit(r->right, pc);
			break;
		
		case Lit:
			(*pc)->opcode = Char;
			(*pc)->c = r->ch;
			(*pc)++;
			break;
		
		case Dot:
			(*pc)->opcode = Any;
			(*pc)++;
			break;
		
		case Range:
			(*pc)->opcode = Rng;
			(*pc)->lo = r->lo;
			(*pc)->hi = r->hi;
			(*pc)++;
			break;
		
		case Quest:
			(*pc)->opcode = Split;
			p1 = (*pc)++;
			p1->x = *pc;
			emit(r->left, pc);
			p1->y = *pc;
			if(r->n)
			{
				/* Non-greedy */
				t = p1->x;
				p1->x = p1->y;
				p1->y = t;
			}
			break;
			
		case Star:
			(*pc)->opcode = Split;
			p1 = (*pc)++;
			p1->x = *pc;
			emit(r->left, pc);
			(*pc)->opcode = Jmp;
			(*pc)->x = p1;
			(*pc)++;
			p1->y = *pc;
			if(r->n)
			{
				t = p1->x;
				p1->x = p1->y;
				p1->y = t;
			}
			break;
			
		case Plus:
			p1 = *pc;
			emit(r->left, pc);
			(*pc)->opcode = Split;
			(*pc)->x = p1;
			p2 = *pc;
			(*pc)++;
			p2->y = *pc;
			if(r->n)
			{
				t = p2->x;
				p2->x = p2->y;
				p2->y = t;
			}
			break;

		case CountedRep:
			if (r->lo == r->hi)
				/* Emit r->left exactly r->lo times */
				for (i = 0; i < r->lo; i++)
					emit(r->left, pc);
			else if (r->lo == -1)
				/* Emit r->hi Quest sequences */
				for (i = 0; i < r->hi; i++)
				{
					(*pc)->opcode = Split;
					p1 = (*pc)++;
					p1->x = *pc;
					emit(r->left, pc);
					p1->y = *pc;
					if(r->n)
					{
						/* Non-greedy */
						t = p1->x;
						p1->x = p1->y;
						p1->y = t;
					}
				}
			else if (r->hi == -1)
			{
				/* Emit r->left r->lo - 1 times, then emit a Plus sequence */
				for (i = 0; i < r->lo - 1; i++)
					emit(r->left, pc);
				p1 = *pc;
				emit(r->left, pc);
				(*pc)->opcode = Split;
				(*pc)->x = p1;
				p2 = *pc;
				(*pc)++;
				p2->y = *pc;
				if(r->n)
				{
					t = p2->x;
					p2->x = p2->y;
					p2->y = t;
				}
			}
			else
			{
				/* Tricky: emit r->left r->lo times, then emit a Quest sequence (r->hi - r->lo) times */
				int optcount = r->hi - r->lo;
				for (i = 0; i < r->lo; i++)
					emit(r->left, pc);
				for (i = 0; i < optcount; i++)
				{
					(*pc)->opcode = Split;
					p1 = (*pc)++;
					p1->x = *pc;
					emit(r->left, pc);
					p1->y = *pc;
					if(r->n)
					{
						/* Non-greedy */
						t = p1->x;
						p1->x = p1->y;
						p1->y = t;
					}
				}
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
