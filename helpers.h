#ifndef HELPERS_H
#define HELPERS_H

/*
helpers for the interpreter
*/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include"arrays.h"
#include"token.h"

// generic new macro
// compatible types must have a matching function with the same signature.
/* initialising a new object:
	Object* o = New(Object, arg);
should expand to:
	Object* o = NewObject(mallocstuff, arg);
*/
#define New(x, a) New##x((x*)malloc(sizeof(x)), a)

// char str stuff ===============================================================

// appends b to a; frees a
/* The idea is that you call it like
thing = strappend(thing, thing2); */
char* strappend(char* a, char* b);

/* a = thing;
thing = strappend(thing, thing2);
TODO calling strappend on something invalidates pointers that used to point to the old thing
*/

// KwArray ============================================================

// Defines an array type for holding a list of keywords.
/* the idea is that the lexer will eventually use this to
generate apropriate keyword tokens. currently unused. */
DefineArrayType(KwArray, char, kw);
DeclareArrayNew(KwArray, char, kw);
DeclareArrayExtend(KwArray, char, kw);
DeclareArrayAdd(KwArray, char, kw);
DeclareArrayRemove(KwArray, char, kw);
DeclareArrayFree(KwArray, char, kw);

// statement separator =======================================================

/*
returns index of token after the end of a statement

ta : token array to search through
index : index to start from
*/
int statementSeparator(TokenArray* ta, int index);

#endif
