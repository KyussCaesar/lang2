/*
This file contains the definitions of all the stuff related to tokens and token arrays.
*/

// token ============================================================================
typedef struct {
	int pos;	// Position of token in buffer.
	char* data;	// token data
	char* type;	// token type
} Token;

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

// token array ======================================================================
typedef struct {
	Token** tokens;		// the array
	int len;			// number of tokens in the array
	int cap;			// max size of the array
} TokenArray;

// creates a new token array
TokenArray* NewTokenArray(TokenArray* ta, int cap)
{
	if (cap < 4) ta->cap = 4; // default init size to 4
	else ta->cap = cap;

	ta->len = 0; 				// starts with no tokens in it.
	ta->tokens = (Token**)malloc(sizeof(Token*) * ta->cap);

	return ta;
}

// Doubles the size of the array like C++ std::vector
void TokenArray_Extend(TokenArray* ta)
{
	Token** oldTokenArray = ta->tokens;
	ta->tokens = (Token**)malloc(sizeof(Token*) * 2 * ta->cap);
	ta->cap = 2*ta->cap;

	// Copies old array to new array
	for (int i = 0; i < ta->len; ++i) ta->tokens[i] = oldTokenArray[i];

	free(oldTokenArray);
}

// Adds a token to a tokenarray.
void TokenArray_Add(TokenArray* ta, Token* t)
{
	if (ta->len == ta->cap) TokenArray_Extend(ta);

	ta->tokens[ta->len] = t;
	ta->len += 1;
}

// deletes a token array. DOES NOT DELETE THE TOKENS.
void TokenArray_Free(TokenArray* ta)
{
	free(ta);
}
