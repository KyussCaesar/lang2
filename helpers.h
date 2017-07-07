#ifndef HELPERS_H
#define HELPERS_H

/*
helpers for the interpreter
*/

#include"arrays.h"

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
char* strappend(char* a, char* b)
{
	if (!(a && b)) return 0;

	char* cat = (char*)malloc(strlen(a) + strlen(b) + 1);

	int i = 0;
	while(a[i] != 0) 
	{
		cat[i] = a[i];
		i++;
	}

	int j = 0;
	while(b[j] != 0)
	{
		cat[i] = b[j];
		j++;
		i++;
	}

	cat[i] = 0;
	free(a);
	return cat;
}

/* a = thing;
thing = strappend(thing, thing2);
TODO a is now invalid pointer */

// KwArray ============================================================

// Defines an array type for holding a list of keywords.
/* the idea is that the lexer will eventually use this to
generate apropriate keyword tokens. currently unused. */
DefineArrayType(KwArray, char, kw);
DefineArrayNew(KwArray, char, kw);
DefineArrayExtend(KwArray, char, kw);
DefineArrayAdd(KwArray, char, kw);
DefineArrayRemove(KwArray, char, kw);
DefineArrayFree(KwArray, char, kw);

#endif
