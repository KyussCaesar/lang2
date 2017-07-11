#ifndef TOKEN_H
#define TOKEN_H

/*
This file contains the definitions of all the stuff related to tokens and token arrays.
*/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include"arrays.h"

// token ============================================================================
typedef struct {
	int pos;	// Position of token in buffer.
	char* data;	// token data
	char* type;	// token type
}
Token;

// creates a token
Token* NewToken(Token* t, Token T);

// deletes a token.
void Token_Free(Token* t);

// returns a char* that represents this token
char* Token_Repr(Token* t);

// compare data of two tokens
int Token_Dcmp(Token* a, Token* b);

// compare type of two tokens
int Token_Tcmp(Token* a, Token* b);

// compare two tokens (both data and type must match)
int Token_cmp(Token* a, Token* b);

// check type of a token
int Token_isType(Token* t, char* type);

// check data of a token
int Token_isData(Token* t, char* data);

// token array ======================================================================

DefineArrayType(TokenArray, Token, tokens);
DeclareArrayNew(TokenArray, Token, tokens);
DeclareArrayExtend(TokenArray, Token, tokens);
DeclareArrayAdd(TokenArray, Token, tokens);
DeclareArrayRemove(TokenArray, Token, tokens);
DeclareArrayFree(TokenArray, Token, tokens);

#endif
