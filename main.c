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

in order to do all of this we need:
	tokens
	strings

program loop:
	open file for reading
	read whole file into memory
	parse tokens
	write to output
*/

#define typestruct typedef struct
#define typeenum typedef enum
#define New(x, a) Newx(((x)*)malloc(sizeof(x)), a)

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

int main(int argc, char ** argv)
{
	/* reads from stdin */
	char * buffer = (char*) malloc(1000);
	int bufindex = 0;

	int c;
	while ((c = getchar()) != EOF) {
		buffer[bufindex] = c;
		bufindex++;
		buffer[bufindex] = 0;
	}

	buffer[bufindex] = EOF;

	/* begin parse */
	char * currentToken;
	bufindex = 0;

	while(buffer[bufindex] != EOF) 
	{

		// WHITESPACE
		if (isspace(buffer[bufindex]))
		{
			puts("whitespace");
			fflush(stdout);
			bufindex++;
			continue;
		}

		// IDENTIFIERS
		if (isalpha(buffer[bufindex]))
		{
			int tokenStart = bufindex;
			int tokenEnd   = 0;

			while (isalpha(buffer[bufindex])) 
			{
				bufindex++;
				if (buffer[bufindex] == EOF) break;
			}

			tokenEnd = bufindex;
			// token is chars from tokenStart (includes) to tokenEnd (excludes)

			char* newId = (char*)malloc(tokenEnd-tokenStart + 1);
			int newIdIndex = 0;
			for (int i = tokenStart; i < tokenEnd; ++i)
			{
				newId[newIdIndex] = buffer[i];
				newIdIndex++;
			}

			newId[newIdIndex] = 0;

			printf("%s%s%s", "Token(identifier, \"", newId, "\")\n");
			continue;
		}

	}

	return 0;
}
