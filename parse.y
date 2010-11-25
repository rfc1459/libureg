/* Lemon-based parser for ureg.
 * Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license.
 */

/* Prefix for token #defines (eg. TK_LITERAL, TK_STAR) */
%token_prefix TK_

/* Data type attached to terminals */
%token_type {int}
/* Data type attached to non-terminals */
%default_type {Regexp*}

/* Parser state */
%extra_argument {Parse *pParse}

/* Syntax error handler */
%syntax_error {
    UNUSED_PARAMETER(yymajor);
    UNUSED_PARAMETER(yyminor);
    pParse->parseError = 1;
}
%stack_overflow {
    UNUSED_PARAMETER(yypMinor);
    pParse->parseError = 1;
}

/* Prefix to generated functions */
%name uregParser

/* Code included near the beginning of the generated parser */
%include {
#include "stdinc.h"
#define UREG_INTERNAL
#include "ureg-internal.h"

/* Disable error recovery */
#define YYNOERRORRECOVERY 1

/* Counted repetition values */
struct CountedRepVal {
    int low;
    int high;
};

#ifndef NDEBUG
#define trace_reg_destroy(a)                                                \
    do {                                                                    \
        if (yyTraceFILE)                                                    \
            fprintf(yyTraceFILE, "%sdestroying %p\n", yyTracePrompt, a);    \
        reg_destroy(a);                                                     \
    } while (0)
#else
#define trace_reg_destroy(a)    reg_destroy(a)
#endif /* !defined(NDEBUG) */

}

/* Here comes the grammar */
input ::= regexp(A).                { pParse->ast_root = A; }
%destructor regexp { trace_reg_destroy($$); }
regexp(A) ::= alt(B).               { A = B; }
regexp(A) ::= STAR(B) alt(C).   {
    Regexp *r1 = reg(Lit, NULL, NULL);
    r1->ch = B;
    A = reg(Cat, r1, C);
}

/* Alternation */
%destructor alt { trace_reg_destroy($$); }
alt(A) ::= alt(B) ALT concat(C). {
    if (B == NULL && C == NULL)
        A = NULL;
    else if (B == NULL)
        A = C;
    else if (C == NULL)
        A = B;
    else
        A = reg(Alt, B, C);
}
alt(A) ::= concat(B).               { A = B; }

/* Concatenation */
%destructor concat { trace_reg_destroy($$); }
concat(A) ::= concat(B) repeat(C). {
    if (B == NULL && C == NULL)
        A = NULL;
    else if (B == NULL)
        A = C;
    else if (C == NULL)
        A = B;
    else
        A = reg(Cat, B, C);
}
concat(A) ::= repeat(B).            { A = B; }

/* Repetition */
%destructor repeat { trace_reg_destroy($$); }
repeat(A) ::= single(B).            { A = B; }
/* Zero or more */
repeat(A) ::= single(B) STAR.       { A = reg(Star, B, NULL); }
/* Zero or more (non-greedy) */
repeat(A) ::= single(B) STAR QUES. {
    A = reg(Star, B, NULL);
    A->n = 1;
}
/* One or more */
repeat(A) ::= single(B) PLUS.       { A = reg(Plus, B, NULL); }
/* One or more (non-greedy) */
repeat(A) ::= single(B) PLUS QUES. {
    A = reg(Plus, B, NULL);
    A->n = 1;
}
/* At most one */
repeat(A) ::= single(B) QUES.       { A = reg(Quest, B, NULL); }
/* At most one (non-greedy - yes, there is a ?? operator...) */
repeat(A) ::= single(B) QUES QUES. {
    A = reg(Quest, B, NULL);
    A->n = 1;
}
/* Counted repetition */
repeat(A) ::= single(B) LBRACE count(C) RBRACE. {
    A = simplify_repeat(B, C.low, C.high, 0);
}
/* Counted repetition (non-greedy) */
repeat(A) ::= single(B) LBRACE count(C) RBRACE QUES. {
    A = simplify_repeat(B, C.low, C.high, 1);
}

/* Counted repetition statement */
%type count {struct CountedRepVal}
count(A) ::= INTEGER(B).                    { A.low =  B; A.high =  B; }
count(A) ::= INTEGER(B) COMMA.              { A.low =  B; A.high = -1; }
count(A) ::= COMMA INTEGER(B).              { A.low =  0; A.high =  B; }
count(A) ::= INTEGER(B) COMMA INTEGER(C). {
    if (B > C)
        pParse->parseError = 1;
    else
    {
        A.low = B;
        A.high = C;
    }
}

/* Single character match */
%destructor single { trace_reg_destroy($$); }
single(A) ::= COLON|LITERAL(B). {
    A = reg(Lit, NULL, NULL);
    A->ch = B;
}
single(A) ::= DOT. {
    A = reg(Dot, NULL, NULL);
}
single(A) ::= LBRACKET bracketexp(B) RBRACKET.  { A = B; }
/* Capturing group */
single(A) ::= LPAREN alt(B) RPAREN. {
    pParse->nparen++;
    A = reg(Paren, B, NULL);
    A->n = pParse->nparen;
}
/* Non-capturing group */
single(A) ::= LPAREN QUES COLON alt(B) RPAREN.  { A = B; }

/* Oversimplified bracket expression grammar */
%destructor bracketexp { trace_reg_destroy($$); }
bracketexp(A) ::= class(B).         { A = B; }

/* Character class */
%destructor class { trace_reg_destroy($$); }
class(A) ::= class(B) range(C).     { A = reg(Alt, B, C); }
class(A) ::= range(B).              { A = B; }

/* Range expression */
%destructor range { trace_reg_destroy($$); }
range(A) ::= COLON|LITERAL(B). {
    A = reg(Lit, NULL, NULL);
    A->ch = B;
}
range(A) ::= COLON|LITERAL(B) RSEP LITERAL(C). {
    /* Both ends of range expression are equal, transform them into a literal */
    if (B == C)
    {
        A = reg(Lit, NULL, NULL);
        A->ch = B;
    }
    else
    {
        A = reg(Range, NULL, NULL);
        if (B < C)
        {
            A->lo = B;
            A->hi = C;
        }
        else
        {
            /* Invalid range */
            pParse->parseError = 1;
        }
    }
}
