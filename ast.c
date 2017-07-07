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

AST_Node* TypeDefinition(TokenArray* ta, int* tlindex)
{
}

AST_Node* VariableDeclaration(TokenArray* ta, int* tlindex)
{
	AST_Node* vardecl = 0;

	/* variable declaration : typename then identifier
	which basically means "two identifiers" */
	if 
}

AST_Node* Expression(TokenArray* ta, int* tlindex)
{}

AST_Node* Identifier(TokenArray* ta, int* tlindex)
{}

AST_Node* Assignment(TokenArray* ta, int* tlindex)
{}

AST_Node* FunctionDefinition(TokenArray* ta, int* tlindex)
{}

//AST_Node* FunctionDefinitionParameters(TokenArray* ta, int* tlindex)

AST_Node* FunctionCall(TokenArray* ta, int* tlindex)
{}

//AST_Node* FunctionCallParameters(TokenArray* ta, int* tlindex)

AST_Node* NumericExpression(TokenArray* ta, int* tlindex)
{}

AST_Node* NumericTerm(TokenArray* ta, int* tlindex)
{}

AST_Node* NumericFactor(TokenArray* ta, int* tlindex)
{}

AST_Node* Number(TokenArray *ta, int* tlindex)
{
	int index = *tlindex;
	int firstToken = index;

	if (!Token_isType(ta->tokens[index], "digit")) return 0;
	// first token was not digit, return zero but no syntax error.
	else index++;

	// handle decimal point
	if (Token_isType(ta->tokens[index], "period"))
	{
		index++;
		if (!Token_isType(ta->tokens[index], "digit"))
		{
			puts("[number] invalid syntax: digits must follow a period.\n");
			return 0;
		}
		else index++;
	}

	// handle scientific notation
	Token t = { -1, "e", "identifier" };
	if (Token_cmp(ta->tokens[index], &t))
	{
		index++;
		if (Token_isType(ta->tokens[index], "plus") ||
			Token_isType(ta->tokens[index], "minus") ) index++;

		if (!Token_isType(ta->tokens[index], "digit"))
		{
			puts("[number] invalid syntax: no number after 'e' in numeric literal.\n");
			return 0;
		}
		else index++;
	}

	if (index > ta->len) index = ta->len; // special case at end of input

	/* the index variable now points to the last token in the number (exclusive)
	hence, we can build the number by concatenating all the data strings. */

	// init output buffer
	char* number = (char*)malloc(1);
	number[0] = 0;

	printf("firsttoken %i index %i\n", firstToken, index);
	fflush(stdout);

	// cat data strings
	for (int i = firstToken; i < index; ++i) number = strappend(number, ta->tokens[i]->data);

	AST_Number* numb = (AST_Number*) malloc(sizeof(AST_Number));
	numb->type = Number;
	numb->number = atof(number);
	*tlindex = index;

	// free number (strappend makes calls to malloc)
	free(number);

	return numb;
}


