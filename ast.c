#include"ast.h"

AST_Node* execute(AST_Node* root)
{
	if (root->type == None) return 0; 
	// TODO: not sure if this is actually what I want to do but 
	// I'm gonna leave it like this for now

	if (root->type == Number)
	{
		return root;
	}

	if (root->type == Identifier)
	{
		AST_Identifier* id = (AST_Identifier*)root;
		return execute(id->tree);
	}

	if (root->type == Assignment)
	{
		AST_Assignment* node = (AST_Assignment*)root;
		node->left->tree = node->right;
		return node->left;
	}

// sets up some common stuff for binary operators (except assignment)
#define BinOpStuff \
	AST_BinOp* op = (AST_BinOp*)root;\
	AST_Number* left = (AST_Number*)execute(op->left);\
	AST_Number* right = (AST_Number*)execute(op->right);\
	/* brief check that left and right are indeed both numbers*/\
	if ( !((left->type == Number) && (right->type == Number)) )\
	{\
		/* TODO: in future may emit error message here;
		depends on how AST building is handled.*/\
		return 0;\
	}\
	AST_Number* sum = (AST_Number*) malloc(sizeof(AST_Number));\
	sum->type = Number

	if (root->type == Plus)
	{
		BinOpStuff;
		sum->number = left->number + right->number;
		return sum;
	}

	if (root->type == Minus)
	{
		BinOpStuff;
		sum->number = left->number - right->number;
		return sum;
	}

	if (root->type == Multiply)
	{
		BinOpStuff;
		sum->number = left->number * right->number;
		return sum;
	}

	if (root->type == Divide)
	{
		BinOpStuff;
		sum->number = left->number / right->number;
		return sum;
	}

	return 0;
}

AST_Node* Statement(TokenArray* ta, int* tlindex)
{
	int index = *tlindex;
	AST_Node* statement = 0;
	


	if (!statement) 
	{
		// is the current token a semicolon?
		if (Token_isType(ta->tokens[*tlindex], "semicolon"))
		{
			*tlindex += 1;
		}
	}

	return statement;
}

AST_Node* VariableDeclaration(TokenArray* ta, int* tlindex)
{
	AST_Node* vardecl = 0;

	/* variable declaration : typename then identifier
	which basically means "two identifiers" */
	if 
}

