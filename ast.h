#ifndef AST_H
#define AST_H

/* stuff concerning the ASTs */

/*
each node is treated like a tree in and of itself
variables in the program will be pointers to a tree 
*/

/* AST NODE DEFINITIONS */

// AST node types
typedef enum {
	None,
	Assignment,
	Number,
	Plus,
	Minus,
	Multiply,
	Divide
}
AST_Type;

/* 
name: basic node
description: "base class" for AST nodes
execution: not executed
*/
typedef struct {
	AST_Type type;
}
AST_Node;

/*
name: number node
description: ast node that holds/represents a number
execution: terminal; returns itself
*/
typedef struct {
	AST_Type type;
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
	AST_Node* tree;
	char* name;
}
AST_Identifier;

/*
name: assignment node
description: node for assignment
execution: changes left to point to the same tree as right, returns left.
*/
typedef struct {
	AST_Type type;
	AST_Identifier* left;
	AST_Node* right;
}
AST_Assignment;

// defines execution of all ast node types
AST_Node* execute(AST_Node* root);

/* BUILDING THE AST */
/*
All of these functions return 0 if application of the rule was unsuccessful
otherwise, they return an AST for executing the rule
*/
AST_Node* Statement(TokenArray* ta, int* tlindex);
AST_Node* TypeDefinition(TokenArray* ta, int* tlindex);
AST_Node* VariableDeclaration(TokenArray* ta, int* tlindex);
AST_Node* Expression(TokenArray* ta, int* tlindex);
AST_Node* Identifier(TokenArray* ta, int* tlindex);
AST_Node* Assignment(TokenArray* ta, int* tlindex);
AST_Node* FunctionDefinition(TokenArray* ta, int* tlindex);
//AST_Node* FunctionDefinitionParameters(TokenArray* ta, int* tlindex);
AST_Node* FunctionCall(TokenArray* ta, int* tlindex);
//AST_Node* FunctionCallParameters(TokenArray* ta, int* tlindex);
AST_Node* NumericExpression(TokenArray* ta, int* tlindex);
AST_Node* NumericTerm(TokenArray* ta, int* tlindex);
AST_Node* NumericFactor(TokenArray* ta, int* tlindex);
AST_Node* Number(TokenArray* ta, int* tlindex);

#endif
