#include"symboltable.h"

DefineArrayNew(SymbolTable, SymbolTableEntry, symbols);
DefineArrayExtend(SymbolTable, SymbolTableEntry, symbols);
DefineArrayAdd(SymbolTable, SymbolTableEntry, symbols);
DefineArrayRemove(SymbolTable, SymbolTableEntry, symbols);
DefineArrayFree(SymbolTable, SymbolTableEntry, symbols);

SymbolTableEntry* SymbolTable_Find(SymbolTable* st, char* name)
{
	for (int i = 0; i < st->len; ++i)
	{
		if (!strcmp(st->symbols[i]->id->name, name)) return st->symbols[i];
	}
	return 0;
}


