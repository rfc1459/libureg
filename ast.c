/* Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license
 */

#include "stdinc.h"
#define UREG_INTERNAL
#include "ureg-internal.h"
#include "parse.h"

typedef enum lexer_state
{
	NORMAL,
	BRACKET1,
	BRACKET,
	BRACE
} lexer_state_t;

Regexp*
parse(const char *s)
{
	Parse pParse;
	void *parser;
	int value, token;
	char c;
	lexer_state_t lstate = NORMAL;
	memset((void *)&pParse, '\0', sizeof(pParse));

#if !defined(NDEBUG) && defined(TRACE_PARSER)
	uregParserTrace(stderr, "uregParser -> ");
#endif
	if (s == NULL)
		return NULL;
	parser = uregParserAlloc(malloc);
	if (parser == NULL)
		return NULL;

	c = *s++;
	value = c;
	while (c && !pParse.parseError)
	{
		switch (lstate)
		{
			case NORMAL:
				value = c;
				switch (c)
				{
					default:
						token = TK_LITERAL;
						break;
					case '\\':
						if (*s == '\0')
						{
							/* CRAAAAAAAAAAAP */
							token = 0;
							break;
						}
						c = *s++;
						token = TK_LITERAL;
						value = c;
						break;
					case '[':
						token = TK_LBRACKET;
						lstate = BRACKET1;
						break;
					case ']':
						token = TK_RBRACKET;
						break;
					case '(':
						token = TK_LPAREN;
						break;
					case ')':
						token = TK_RPAREN;
						break;
					case '{':
						token = TK_LBRACE;
						value = 0;
						lstate = BRACE;
						break;
					case '}':
						token = TK_RBRACE;
						break;
					case '|':
						token = TK_ALT;
						break;
					case '*':
						token = TK_STAR;
						break;
					case '+':
						token = TK_PLUS;
						break;
					case '?':
						token = TK_QUES;
						break;
					case '.':
						token = TK_DOT;
						break;
				}
				break;
			case BRACKET1:
				value = c;
				token = TK_LITERAL;
				lstate = BRACKET;
				break;
			case BRACKET:
				value = c;
				switch (c)
				{
					default:
						token = TK_LITERAL;
						break;
					case ']':
						token = TK_RBRACKET;
						lstate = NORMAL;
						break;
					case '-':
						/* Peek next character */
						if (*s == ']')
							token = TK_LITERAL;
						else
							token = TK_RSEP;
						break;
					case '\0':
						token = 0;
						break;
				}
				break;
			case BRACE:
				if (c == '}')
				{
					token = TK_RBRACE;
					lstate = NORMAL;
				}
				else if (isdigit(c))
				{
					token = TK_INTEGER;
					do
					{
						value *= 10;
						value += c - '0';
						if (isdigit(*s))
							c = *s++;
						else
							break;
					} while (c);
				}
				else if (c == ',')
				{
					token = TK_COMMA;
					value = 0;
				}
				else
					/* Everything else is a syntax error */
					token = 0;
				break;
		}
		uregParser(parser, token, value, &pParse);
		c = *s++;
	}
	/* There's no need to emit $ after a syntax error */
	if (!pParse.parseError)
		uregParser(parser, 0, 0, &pParse);

#ifndef NDEBUG	
	if (!pParse.parseError && pParse.ast_root == NULL)
	{
		fprintf(stderr, "WTF?\n");
	}
#endif

	uregParserFree(parser, free);

#if !defined(NDEBUG) && defined(TRACE_PARSER)
	uregParserTrace(NULL, NULL);
#endif

	if (pParse.parseError)
	{
		if (pParse.ast_root)
			reg_destroy(pParse.ast_root);
		return NULL;
	}
	return pParse.ast_root;
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

/* Build a new AST node */
Regexp*
reg(int type, Regexp *left, Regexp *right)
{
	Regexp *r;
	
	r = (Regexp *)mal(sizeof(Regexp));
	r->type = type;
	r->left = left;
	r->right = right;
	return r;
}

/* Recursively destroy an AST branch */
void
reg_destroy(Regexp *r)
{
	if (r == NULL)
		return;
	if (r->left)
		reg_destroy(r->left);
	if (r->right)
		reg_destroy(r->right);
	free(r);
}

/* Dump the AST (preorder traversal) */
void
printre(Regexp *r)
{
	switch (r->type)
	{
		default:
			printf("Unknown(%d)", r->type);
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
			if (r->n)
				printf("Ng");
			printf("Star(");
			printre(r->left);
			printf(")");
			break;
		
		case Plus:
			if (r->n)
				printf("Ng");
			printf("Plus(");
			printre(r->left);
			printf(")");
			break;
		
		case Quest:
			if (r->n)
				printf("Ng");
			printf("Quest(");
			printre(r->left);
			printf(")");
			break;

		case CountedRep:
			if (r->n)
				printf("Ng");
			printf("CountedRep(");
			printre(r->left);
			printf(", %d, %d)", r->lo, r->hi);
			break;
	}
}
