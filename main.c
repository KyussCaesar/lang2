/*
baby steps: write program that reads file and outputs what tokens it found

current tokens:
	identifiers
	base-10 literals
	string literals

token grammar rules:
	identifier : letter followed by zero or more letters or numbers
	base-10 literal : numeric character followed by zero or more numeric characters
	string literal : quote followed by zero or more of any character, followed by quote
*/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

#include"helpers.h"
#include"token.h"

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

	puts("finished parsing tokens; here is what i saw\n");
	for (int i = 0; i < tokens->len; ++i)
	{
		printf("%s", Token_Repr(tokens->tokens[i]));
	}
	int parseIndex = 0;
	double number = 0;

	while (parseIndex < tokens->len)
	{
		if (Number(tokens, &parseIndex, &number)) printf("found number: %lf\n", number);
		parseIndex++;
	}

	return 0;
}

/*
function that takes tokens from current index up to matching semicolon and tries to figure out 
what production applies

increment index while token is not semicolon
*/

/*
ta : token array to search through
index : index to start from
*/
int statementSeparator(TokenArray* ta, int index)
{
	while (!Token_isType(ta->tokens[index], "semicolon"))
	{
		index++;

		if (Token_isType(ta->tokens[index], "l brace"))
		{
			int braceCount = 1;
			while (braceCount)
			{
				index++;
				if (Token_isType(ta->tokens[index], "l brace")) braceCount++;
				if (Token_isType(ta->tokens[index], "r brace")) braceCount--;
			}
		}
	}

	return index;
}

