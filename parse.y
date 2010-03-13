%{
/* Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license
 */

#include "ureg.h"

static int yylex(void);
static void yyerror(char*);
static Regexp *parsed_regexp;

%}

%union {
	int     val;
	Regexp* re;
}

%type	<re>		line
%type	<re>		alt concat repeat single
%type	<re>		class range
%token	<val>		LCHAR LDASH 
%token				CLPAREN CRPAREN LLPAREN LRPAREN LALT LSTAR LPLUS LQUES
%token				LDOT LBAD EOL
%%

/* Parse a regexp and build the corresponding AST */

line: alt EOL
	{
		parsed_regexp = $1;
		return 1;
	}
;

alt:
	concat
|	alt LALT concat
	{
		$$ = reg(Alt, $1, $3);
	}
;

concat:
	repeat
|	concat repeat
	{
		$$ = reg(Cat, $1, $2);
	}
;

repeat:
	single
|	single LSTAR
	{
		$$ = reg(Star, $1, NULL);
	}
|	single LSTAR LQUES
	{
		$$ = reg(Star, $1, NULL);
		$$->n = 1;
	}
|	single LPLUS
	{
		$$ = reg(Plus, $1, NULL);
	}
|	single LPLUS LQUES
	{
		$$ = reg(Plus, $1, NULL);
		$$->n = 1;
	}
|	single LQUES
	{
		$$ = reg(Quest, $1, NULL);
	}
|	single LQUES LQUES
	{
		$$ = reg(Quest, $1, NULL);
		$$->n = 1;
	}
;

single:
	LCHAR
	{
		$$ = reg(Lit, NULL, NULL);
		$$->ch = $1;
	}
|	LDASH
	{
		$$ = reg(Lit, NULL, NULL);
		$$->ch = $1;
	}
|	LDOT
	{
		$$ = reg(Dot, NULL, NULL);
	}
|	CLPAREN class CRPAREN
	{
		$$ = $2;
		/* FIXME: coalesce overlapping ranges and remove duplicates */
	}
|	LLPAREN alt LRPAREN
	{
		$$ = $2;
	}
;

/* Class grammar is oversimplified, this should REALLY be fixed */
class:
	range
|	class range
	{
		$$ = reg(Alt, $1, $2);
	}
;

/* We use a special Range node in order to reduce the number
 * of branches in our NFA.
 * This saves some memory and execution speed (though the latter
 * is mostly influenced by input length).
 */
range:
	LCHAR
	{
		$$ = reg(Lit, NULL, NULL);
		$$->ch = $1;
	}
|	LCHAR LDASH LCHAR
	{
		/* Slight optimization: convert to Lit node if boundaries are equal */
		if ($1 == $3)
		{
			$$ = reg(Lit, NULL, NULL);
			$$->ch = $1;
		}
		else
		{
			$$ = reg(Range, NULL, NULL);
			if($1 > $3)
			{
				/* Flip boundaries */
				$$->lo = $3;
				$$->hi = $1;
			}
			else
			{
				$$->lo = $1;
				$$->hi = $3;
			}
		}
	}
;
%%

static char *input;
static Regexp *parsed_regexp;
static int nparen;

static int
yylex(void)
{
	int c, s;
	
	if(input == NULL || *input == '\0')
		return EOL;
	c = *input++;
	switch(c)
	{
		default:
			yylval.val = c;
			s = LCHAR;
			break;
		case '\\':
			if(*input == '\0')
				return LBAD;
			c = *input++;
			yylval.val = c;
			s = LCHAR;
			break;
		case '[':
			s = CLPAREN;
			break;
		case ']':
			s = CRPAREN;
			break;
		case '(':
			s = LLPAREN;
			break;
		case ')':
			s = LRPAREN;
			break;
		case '|':
			s = LALT;
			break;
		case '*':
			s = LSTAR;
			break;
		case '+':
			s = LPLUS;
			break;
		case '?':
			s = LQUES;
			break;
		case '.':
			s = LDOT;
			break;
		case '-':
			s = LDASH;
			yylval.val = c;
			break;
	}
	return s;
}

/* FIXME: perform better error reporting, shall we? */
void
fatal(char *fmt, ...)
{
	va_list arg;
	
	va_start(arg, fmt);
	fprintf(stderr, "fatal error: ");
	vfprintf(stderr, fmt, arg);
	fprintf(stderr, "\n");
	va_end(arg);
	exit(2);
}

static void
yyerror(char *s)
{
	fatal("%s", s);
}

Regexp*
parse(char *s)
{
	Regexp *dotstar;
	
	input = s;
	parsed_regexp = NULL;
	if(yyparse() != 1)
		yyerror("Syntax error");
	if(parsed_regexp == NULL)
		yyerror("NULL result (wtf?)");
	
	/* Change AST root to "Cat(NgStar(Dot), parsed_regexp)" */
	dotstar = reg(Star, reg(Dot, NULL, NULL), NULL);
	dotstar->n = 1;
	return reg(Cat, dotstar, parsed_regexp);
}

void*
mal(size_t n)
{
	void *v;
	v = malloc(n);
	if(v == NULL)
		fatal("out of memory");
	memset(v, '\0', n);
	return v;
}

/* Build an AST node */
Regexp*
reg(int type, Regexp *left, Regexp *right)
{
	Regexp *r;
	
	r = (Regexp *) mal(sizeof(Regexp));
	r->type = type;
	r->left = left;
	r->right = right;
	return r;
}

/* Destroy an AST (postorder traversal) */
void
reg_destroy(Regexp *r)
{
	if(r == NULL)
		return;
	if(r->left)
		reg_destroy(r->left);
	if(r->right)
		reg_destroy(r->right);
	free(r);
}

/* Dump the AST (preorder traversal) */
void
printre(Regexp *r)
{
	switch(r->type)
	{
		default:
			printf("PUPPA!");
			break;
			
		case Alt:
			printf("Alt(");
			printre(r->left);
			printf(", ");
			printre(r->right);
			printf(")");
			break;
			
		case Cat:
			printf("Cat(");
			printre(r->left);
			printf(", ");
			printre(r->right);
			printf(")");
			break;
			
		case Lit:
			printf("Lit(%c)", r->ch);
			break;
			
		case Dot:
			printf("Dot");
			break;
			
		case Range:
			printf("Range(%c, %c)", r->lo, r->hi);
			break;
			
		case Star:
			if(r->n)
				printf("Ng");
			printf("Star(");
			printre(r->left);
			printf(")");
			break;
		
		case Plus:
			if(r->n)
				printf("Ng");
			printf("Plus(");
			printre(r->left);
			printf(")");
			break;
		
		case Quest:
			if(r->n)
				printf("Ng");
			printf("Quest(");
			printre(r->left);
			printf(")");
			break;
	}
}
