#ifndef AST_H
#define AST_H

/*
building an ast:
	check that grammar is valid
	check that a variable has been declared (i.e, added to the symbol table) before it is used
	check that a variable is only declared once
	check that types are compatible
*/

#include<stdlib.h>
#include<stdio.h>

#include"helpers.h"
#include"token.h"
#include"symboltable.h"

/* stuff concerning the ASTs */

/*
each node is treated like a tree in and of itself
variables in the program will be pointers to a tree 

functions:
	define a new scope
	set parameters to point to whatever was passed
	execute tree
*/

// AST Node Definitions ========================================================

/* definition is in symboltable to resolve circular dependency
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
AST_Type;*/

// converts type to string for printing
// result does not need to be freed.
char* AST_TypeRepr(AST_Type t);

/* 
name: basic node
description: "base class" for AST nodes
execution: not executed
*/
/*
typedef struct astnode {
	AST_Type type;
	SymbolTableEntry_TypeName* rt_type;
}
AST_Node;*/

/*
name: number node
description: ast node that holds/represents a number
execution: terminal; returns itself
*/
typedef struct {
	AST_Type type;
	SymbolTableEntry_TypeName* rt_type;
	double number;
}
AST_Number;

/*
name: binary operator node
description: node for binary operations
execution: execute left, execute right, return result of operation
*/
typedef struct {
	AST_Type type;
	SymbolTableEntry_TypeName* rt_type;
	AST_Node* left;
	AST_Node* right;
}
AST_BinOp;

/*
name: identifier node
description: node for identifiers; points to another tree (which may be a number etc.)
execution: execute the tree and return the result
*/
typedef struct {
	AST_Type type;
	SymbolTableEntry_TypeName* rt_type;
	SymbolTableEntry_VariableName* variable;
}
AST_Variable;

/*
name: assignment node
description: node for assignment
execution: changes left to point to the same tree as right, returns left.
*/
typedef struct {
	AST_Type type;
	SymbolTableEntry_VariableName* left; // variable in the symbol table
	AST_Node* right;
}
AST_Assignment;

/*
name: unary minus
description: unary minus
execution: unary minus
*/
typedef struct {
	AST_Type type;
	SymbolTableEntry_TypeName* rt_type;
	AST_Node* negate;
}
AST_UnaryMinus;

typedef struct {
	AST_Type type;
	SymbolTableEntry_TypeName* rt_type;
	AST_Node* bexpr;	// the condition to evaluate
	AST_Node* trueTree; // stuff to execute for true case
	AST_Node* falseTree;// stuff to execute for false case
}
AST_If;

typedef struct {
	AST_Type type;
	SymbolTableEntry_TypeName* rt_type;
	char val;
}
AST_Boolean;

typedef struct {
	AST_Type type;
	SymbolTableEntry_TypeName* rt_type;
	AST_Node* negate;
}
AST_LogicalNot;

// AST Runtime Functions ============================================================

// defines execution of all ast node types
AST_Node* execute(AST_Node* root);

// frees an ast
/* this function is called when building an ast has failed */
void freeast(AST_Node* n);

// Building ASTs from the input tokens =============================================

/*
These functions try to use a rule in the grammar to build an ast.

The grammar is written below these function delcarations

These functions conform to the following interface:

if building the ast is successful;
	update the tlindex parameter
	and return a pointer to an ast for executing the rule.

if rule failed without error
	returns zero; and does not affect the tlindex parameter

if rule failed with error
	returns zero and sets tlindex to -1

VariableDeclaration:
	if a variable declaration is made, but no assignment in the same statement,
	returns a "none" node

	if it's declaration + assignment; then returns an assignment node
*/

AST_Node* Statement(TokenArray* ta, int* tlindex);
AST_Node* IfStatement(TokenArray* ta, int* tlindex);
AST_Node* TypeDefinition(TokenArray* ta, int* tlindex);
AST_Node* VariableDeclaration(TokenArray* ta, int* tlindex);
AST_Node* Expression(TokenArray* ta, int* tlindex);
AST_Node* VariableReference(TokenArray* ta, int* tlindex);
AST_Node* Assignment(TokenArray* ta, int* tlindex);
AST_Node* FunctionDefinition(TokenArray* ta, int* tlindex);
//AST_Node* FunctionDefinitionParameters(TokenArray* ta, int* tlindex);
AST_Node* FunctionCall(TokenArray* ta, int* tlindex);
//AST_Node* FunctionCallParameters(TokenArray* ta, int* tlindex);
AST_Node* NumericExpression(TokenArray* ta, int* tlindex);
AST_Node* NumericTerm(TokenArray* ta, int* tlindex);
AST_Node* NumericFactor(TokenArray* ta, int* tlindex);
AST_Node* Number(TokenArray* ta, int* tlindex);
AST_Node* BooleanExpression(TokenArray* ta, int* tlindex);
AST_Node* BooleanTerm(TokenArray* ta, int* tlindex);
AST_Node* BooleanFactor(TokenArray* ta, int* tlindex);
AST_Node* BooleanLiteral(TokenArray* ta, int* tlindex);

// prints the tree to file fp
void AST_FPrint(FILE* fp, AST_Node* tree);

/*
grammar:

statement:
	variable declaration ';'
	type def ';'
	expression ';'

variable decl : 
	typename followed by identifier
	typename then assignment

Number forty;

'Number' 'forty' ';'



typedef:
	'class' identifier '{' statements '}'

expression:
	identifier
	assignment
	function def
	function call
	numeric expression

assignment:
	identifier '=' expression

function def:
	'func' '(' parameter declaration ')' '->' typename ':' '{' statements '}'

parameter declaration:
	variable declaration optionally followed by comma and then more variable declarations

function call:
	identifier '(' parameter list ')'

parameter list:
	expression optionally followed by comma and then more expressions

numeric expression:
	summation of terms

term:
	product/division of factors/divisors
	factor
	factor plus factor
	factor minus factor

factor:
	number
	identifier
	factor '^' factor
	'(' numeric expression ')' 

number : digit optionally followed by a . then more digit, optionally followed by the letter 'e' then ('+', '-', '') then more digit.
*/

#endif
