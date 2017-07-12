#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include"arrays.h"

typedef struct {
	AST_Identifier* id;
	char* type;
}
SymbolTableEntry;

DefineArrayType(SymbolTable, SymbolTableEntry, symbols);
DeclareArrayNew(SymbolTable, SymbolTableEntry, symbols);
DeclareArrayExtend(SymbolTable, SymbolTableEntry, symbols);
DeclareArrayAdd(SymbolTable, SymbolTableEntry, symbols);
DeclareArrayRemove(SymbolTable, SymbolTableEntry, symbols);
DeclareArrayFree(SymbolTable, SymbolTableEntry, symbols);

/* searches for identifier named name in symboltable st.
if found, returns the AST_Node for that identifier
else, returns 0; */
AST_Identifier* SymbolTable_Find(SymbolTable* st, char* name);

#endif
