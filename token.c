#include"token.h"

// creates a token
Token* NewToken(Token* t, Token T)
{
	t->pos = T.pos;
	t->data = T.data;
	t->type = T.type;
	return t;
}

// deletes a token.
void Token_Free(Token* t)
{
	free(t->data);
	free(t->type);
	free(t);
}

// returns a char* that represents this token
char* Token_Repr(Token* t)
{
	int reprStrLen = strlen("Token(, '')\n") + strlen(t->data) + strlen(t->type) + 1;
	char* reprStr = (char*) malloc(reprStrLen);
	sprintf(reprStr, "Token(%s, \'%s\')\n", t->type, t->data);
	
	return reprStr;
}

// compare data of two tokens
int Token_Dcmp(Token* a, Token* b)
{
	return !strcmp(a->data, b->data);
}

// compare type of two tokens
int Token_Tcmp(Token* a, Token* b)
{
	return !strcmp(a->type, b->type);
}

// compare two tokens (both data and type must match)
int Token_cmp(Token* a, Token* b)
{
	return Token_Dcmp(a,b) && Token_Tcmp(a,b);
}

// check type of a token
int Token_isType(Token* t, char* type)
{
	return !strcmp(t->type, type);
}

// check data of a token
int Token_isData(Token* t, char* data)
{
	return !strcmp(t->data, data);
}

// token array ======================================================================

DefineArrayNew(TokenArray, Token, tokens);
DefineArrayExtend(TokenArray, Token, tokens);
DefineArrayAdd(TokenArray, Token, tokens);
DefineArrayRemove(TokenArray, Token, tokens);
DefineArrayFree(TokenArray, Token, tokens);

