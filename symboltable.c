#include"symboltable.h"

DefineArrayNew(SymbolTable, SymbolTableEntry, symbols);
DefineArrayExtend(SymbolTable, SymbolTableEntry, symbols);
DefineArrayAdd(SymbolTable, SymbolTableEntry, symbols);
DefineArrayRemove(SymbolTable, SymbolTableEntry, symbols);
DefineArrayFree(SymbolTable, SymbolTableEntry, symbols);

AST_Identifier* SymbolTable_Find(SymbolTable* st, char* name)
{
	#define ST_ENTRY st->symbols
	for (int i = 0; i < st->len; ++i)
	{
		if (!strcmp(ST_ENTRY[i]->id->name, name)) return ST_ENTRY->id;
	}
	#undef STENTRY

	return 0;
}


