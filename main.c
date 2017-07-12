#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

#include"helpers.h"
#include"token.h"
#include"ast.h"

/*
Retrieves next token from the buffer, using delim as the function to determine delimiting characters.
*/
char* getToken(char * buffer, int *index, int (*delim)(int))
{
	int bufindex   = *index;
	int tokenStart = bufindex;
	int tokenEnd   = 0;

	while (delim(buffer[bufindex])) bufindex++;

	tokenEnd = bufindex;
	// token is chars from tokenStart (includes) to tokenEnd (excludes)

	char* newId = (char*)malloc(tokenEnd-tokenStart + 1);

	int newIdIndex = 0;
	for (int i = tokenStart; i < tokenEnd; ++i)
	{
		newId[newIdIndex] = buffer[i];
		newIdIndex++;
	}

	// null terminate new string
	newId[newIdIndex] = 0;

	// write new buffer index
	*index = bufindex;

	return newId;
}

/*
Delimiting function for string literals (skips backslash-escaped quotes)
*/
int endquote(int ch)
{
	static int previousChar;
	if (ch == EOF) return 0;
	if ((ch == 0x22) && (previousChar != 0x5C)) return 0;

	previousChar = ch;
	return 1;
}

int main(int argc, char ** argv)
{
	/* reads from stdin */
	char * buffer = (char*) malloc(1000);
	if (buffer == 0) {
		fprintf(stderr, "hey; input buffer allocation failed. terminating...\n");
		return 1;
	}

	int bufindex = 0;

	int c;
	while ((c = getchar()) != EOF) {
		buffer[bufindex] = c;
		bufindex++;
	}

	buffer[bufindex] = EOF;

	/* begin parse */
	char * currentToken;
	char * tokenType;
	bufindex = 0;

	TokenArray* tokens = New(TokenArray, 0);

	// Lexer code
	while(buffer[bufindex] != EOF) 
	{

		// WHITESPACE
		if (isspace(buffer[bufindex]))
		{
			bufindex++;
			continue;
		}

		// IDENTIFIERS
		else if (isalpha(buffer[bufindex]))
		{
			currentToken = getToken(buffer, &bufindex, isalpha);
			tokenType = "identifier";
		}

		// DIGITS
		else if (isdigit(buffer[bufindex]))
		{
			currentToken = getToken(buffer, &bufindex, isdigit);
			tokenType = "digit";
		}

		// STRING LITERAL
		else if (buffer[bufindex] == '"')
		{
			bufindex++;
			currentToken = getToken(buffer, &bufindex, endquote);
			bufindex++;
			tokenType = "string literal";
		}

		// LEFT PARENTHESES
		else if (buffer[bufindex] == '(')
		{
			bufindex++;
			currentToken = "(";
			tokenType = "l paren";
		}

		// RIGHT PARENTHESES
		else if (buffer[bufindex] == ')')
		{
			bufindex++;
			currentToken = ")";
			tokenType = "r paren";
		}

		// LEFT BRACE
		else if (buffer[bufindex] == '{')
		{
			bufindex++;
			currentToken = "{";
			tokenType = "l brace";
		}

		// RIGHT BRACE
		else if (buffer[bufindex] == '}')
		{
			bufindex++;
			currentToken = "}";
			tokenType = "r brace";
		}

		// LEFT SQUARE BRACE
		else if (buffer[bufindex] == '[')
		{
			bufindex++;
			currentToken = "[";
			tokenType = "l sq. brace";
		}

		// RIGHT SQUARE BRACE
		else if (buffer[bufindex] == ']')
		{
			bufindex++;
			currentToken = "]";
			tokenType = "r sq. brace";
		}

		// ASSIGNMENT, EQUALITY
		else if (buffer[bufindex] == '=')
		{
			bufindex++;
			if (buffer[bufindex] == '=')
			{
				bufindex++;
				currentToken = "==";
				tokenType = "equality";
			}
			else
			{
				currentToken = "=";
				tokenType = "assignment";
			}
		}

		// PLUS
		else if (buffer[bufindex] == '+')
		{
			bufindex++;
			currentToken = "+";
			tokenType = "plus";
		}

		// MINUS
		else if (buffer[bufindex] == '-')
		{
			bufindex++;
			currentToken = "-";
			tokenType = "minus";
		}

		// MULTIPLY
		else if (buffer[bufindex] == '*')
		{
			bufindex++;
			currentToken = "*";
			tokenType = "mult";
		}

		// DIVIDE
		else if (buffer[bufindex] == '/')
		{
			bufindex++;
			currentToken = "/";
			tokenType = "divide";
		}

		// COLON
		else if (buffer[bufindex] == ':')
		{
			bufindex++;
			currentToken = ":";
			tokenType = "colon";
		}

		// SEMICOLON
		else if (buffer[bufindex] == ';')
		{
			bufindex++;
			currentToken = ";";
			tokenType = "semicolon";
		}

		// PERIOD
		else if (buffer[bufindex] == '.')
		{
			bufindex++;
			currentToken = ".";
			tokenType = "period";
		}

		// COMMA
		else if (buffer[bufindex] == ',')
		{
			bufindex++;
			currentToken = ",";
			tokenType = "comma";
		}

		// UNRECOGNISED TOKEN
		else
		{
			printf("[tokeniser] unrecognised token! num:%i, char:%c\n", buffer[bufindex], buffer[bufindex]);
			bufindex++;
			continue;
		}

		Token t = {bufindex, currentToken, tokenType};
		Token* newToken = New(Token, t);
		TokenArray_Add(tokens, newToken);
	}

	if (DEBUG_BUILD)
	{
		puts("\ntokens:");
		for (int i = 0; i < tokens->len; ++i)
		{
			printf("%s", Token_Repr(tokens->tokens[i]));
		}
	}

	puts("\noutput:");
	int parseIndex = 0;
	AST_Number* result = 0;
	while(parseIndex < tokens->len)
	{
		AST_Node* statement = Statement(tokens, &parseIndex);
		if (parseIndex == -1) return 1;
		if (statement) 
		{
			if (DEBUG_BUILD) AST_FPrint(stdout, statement);
			result = (AST_Number*)execute(statement);
			if (result)
			{
				if (result->type == NumberNode) printf("%lf\n", result->number);
			}

			freeast((AST_Node*)result);
			freeast(statement);
		}
	}

	puts("finished, exiting");
	return 0;
}

