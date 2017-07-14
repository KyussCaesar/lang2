#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include"arrays.h"
#include"string.h"

typedef enum {
	VariableName,
	TypeName
}
SymbolType;

/*
Struct representing entries in a symbol table.
*/
typedef struct {
	SymbolType entrytype;
	char* symbol;
}
SymbolTableEntry;

typedef struct {
	SymbolType entrytype;
	char* symbol;
	// probably a pointer to a vtable, coming soon
}
SymbolTableEntry_TypeName;

// AST node types
typedef enum {
	None,
	NumberNode,
	PlusNode,
	MinusNode,
	MultiplyNode,
	DivideNode,
	VariableReferenceNode,
	AssignmentNode,
	UnaryMinus,
}
AST_Type;

typedef struct {
	AST_Type type;
	SymbolTableEntry_TypeName* rt_type;
}
AST_Node;

typedef struct {
	SymbolType entrytype;
	char* symbol;
	AST_Node* tree;
	SymbolTableEntry_TypeName* rt_type;
}
SymbolTableEntry_VariableName;

DefineArrayType(SymbolTable, SymbolTableEntry, symbols);
DeclareArrayNew(SymbolTable, SymbolTableEntry, symbols);
DeclareArrayExtend(SymbolTable, SymbolTableEntry, symbols);
DeclareArrayAdd(SymbolTable, SymbolTableEntry, symbols);
DeclareArrayRemove(SymbolTable, SymbolTableEntry, symbols);
DeclareArrayFree(SymbolTable, SymbolTableEntry, symbols);

/* searches for identifier named name in symboltable st.
if found, returns the AST_Node for that identifier
else, returns 0; */
SymbolTableEntry* SymbolTable_Find(SymbolTable* st, char* name);

char* SymbolTable_TypeRepr(SymbolType t);

#endif
