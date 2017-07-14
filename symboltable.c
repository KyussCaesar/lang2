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
		if (!strcmp(st->symbols[i]->symbol, name)) return st->symbols[i];
	}
	return 0;
}

char* SymbolTable_TypeRepr(SymbolType t)
{
	if (t == VariableName) return "VariableName";
	if (t == TypeName)     return "TypeName";
	return "{Unknown}";
}
