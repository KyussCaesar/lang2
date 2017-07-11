#include"helpers.h"

// char str

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

// keyword arrays

DefineArrayNew(KwArray, char, kw);
DefineArrayExtend(KwArray, char, kw);
DefineArrayAdd(KwArray, char, kw);
DefineArrayRemove(KwArray, char, kw);
DefineArrayFree(KwArray, char, kw);

// statement separator

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
