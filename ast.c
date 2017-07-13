#include"ast.h"
#include"symboltable.h"

#define d_printf(...) if(DEBUG_BUILD) printf(__VA_ARGS__);

// this is kindof inelegant but I can't be bothered changing everything right now
#define IncrementWithBoundsCheck(i)\
	++i;\
	if (i >= ta->len)\
	{\
		puts("hey, i ran out of input! You might be missing a semicolon at the end of your program");\
		*tlindex = -1;\
		return 0;\
	}

char* AST_TypeRepr(AST_Type t)
{
	switch (t)
	{
		case None:            return "None";
		case AssignmentNode:  return "AssignmentNode";
		case IdentifierNode:  return "IdentifierNode";
		case NumberNode:      return "NumberNode";
		case PlusNode:        return "PlusNode";
		case MinusNode:       return "MinusNode";
		case MultiplyNode:    return "MultiplyNode";
		case DivideNode:      return "DivideNode";
		case UnaryMinus:      return "UnaryMinus";
		default:              return "{Unknown}";
	}

	return 0; // should never execute

}

extern SymbolTable* global_symbol_table;

AST_Node* execute(AST_Node* root)
{
	if (!root)
	{
		d_printf("[execute] execute function was passed a null pointer\n");
		return 0;
	}

	if (root->type == None) return 0; 

	if (root->type == NumberNode)
	{
		AST_Number* numb = (AST_Number*) malloc(sizeof(AST_Number));
		numb->type = NumberNode;
		numb->number = (*(AST_Number*)root).number;
		d_printf("[execute] type '%s' and value %lf\n", AST_TypeRepr(numb->type), numb->number);
		return (AST_Node*)numb;
	}

	if (
		(root->type == PlusNode) ||
		(root->type == MinusNode) ||
		(root->type == MultiplyNode) ||
		(root->type == DivideNode) ) 
	{
		d_printf("[execute] executing binop '%s'\n", AST_TypeRepr(root->type));
		return BinOpCheck(root);
	}

	if (root->type == IdentifierNode)
	{
		AST_Identifier* id = (AST_Identifier*)root;
		d_printf("[execute] executing identifier '%s'\n", id->name);
		return execute(id->tree);
	}

	if (root->type == AssignmentNode)
	{
		AST_Assignment* node = (AST_Assignment*)root;

		/* when you do reference counting, then you will have to update the tree that 
		the left side used to point to, here. */

		// check that left and right are initialised properly
		if (!node->left || !node->right)
		{
			if (!node->left) puts("[execute] left side of assignment was null\n");
			if (!node->right) puts("[execute] right side of assignment was null\n");
			return 0;
		}

		// perform lookup of variable name in symbol table.
		SymbolTableEntry* ste = SymbolTable_Find(global_symbol_table, node->left);
		if (ste)
		{
			if (!strcmp(ste->id->rt_type, "variable")) ;
		}

		else
		{
			printf("[execute] hey, i haven't heard of '%s' before\n", node->left);
			return 0;
		}

		// assignment expressions return a reference to the left side
		return (AST_Node*)(node->left);
	}

	if (root->type == UnaryMinus)
	{
		AST_UnaryMinus* um = (AST_UnaryMinus*) root;
		AST_Node* minus = execute(um->negate);
		// check that result of the above is a number node
		if (minus->type == NumberNode)
		{
			AST_Number* minusnode = (AST_Number*) minus;
			minusnode->number *= -1;
			d_printf("[execute] type '%s'\n", AST_TypeRepr(minusnode->type));
			return minus;
		}
		else
		{
			printf("[execute] hey, you have to do unary minus on either a number or something that evaluates to a number; (you tried to do it on a %s)\n", AST_TypeRepr(minus->type));
			return 0;
		}
	}

	if (root->type == VariableDeclarationNode)
	{
		AST_VariableDeclaration* vdecl = (AST_VariableDeclaration*)root;

		// check if variable with specified name is already in the symbol table.
		if (SymbolTable_Find(global_symbol_table, vdecl->id->name))
		{
			printf("[execute] hey, variable '%s' has already been declared\n", vdecl->id->name);
			freeast((AST_Node*) vdecl);
			return 0;
		}

		else
		{
			// create a new symbol table entry and add to symbol table
			SymbolTableEntry* newsymbol = (SymbolTableEntry*) malloc(sizeof(SymbolTableEntry));
			newsymbol->id = vdecl->id;
			newsymbol->type = "identifier";
			SymbolTable_Add(global_symbol_table, newsymbol);
			if (vdecl->init) execute(vdecl->init);
			return 1;
		}
	}

	return 0;
}

void freeast(AST_Node* n)
{
	if (!n) return;
	if (n->type == None) return;
	if (n->type == NumberNode) {free(n); return;}

	// binops
	if (
		(n->type == PlusNode) ||
		(n->type == MinusNode) ||
		(n->type == MultiplyNode) ||
		(n->type == DivideNode) )
	{
		AST_BinOp* binop = (AST_BinOp*) n;
		freeast(binop->left);
		freeast(binop->right);
		free(n);
		return;
	}

	if (n->type == IdentifierNode)
	{
		AST_Identifier* id = (AST_Identifier*) n;
		freeast(id->tree);
		free(id->name);
		free(n);
		return;
	}

	if (n->type == AssignmentNode)
	{
		AST_Assignment* assign = (AST_Assignment*) n;
		freeast((AST_Node*)assign->left);
		freeast((AST_Node*)assign->right);
		free(n);
		return;
	}

	if (n->type == UnaryMinus)
	{
		AST_UnaryMinus* um = (AST_UnaryMinus*)n;
		freeast((AST_Node*)um->negate);
		free(n);
		return;
	}

	printf("[freeast] i don't know how to free this; (%i %x) '%s'\n", n->type, n->type, AST_TypeRepr(n->type));
	return;
}

AST_Node* BinOpCheck(AST_Node* root)
{
	if (!root) 
	{
		puts("[binop] binop node was null\n");
		return 0;
	}

	AST_BinOp* op = (AST_BinOp*)root;

	if ( !(op->left) || !(op->right) )
	{
		puts("[binop] one of the sides of the binop was not initialised\n");
		return 0;
	}

	AST_Number* left = (AST_Number*)execute(op->left);
	AST_Number* right = (AST_Number*)execute(op->right);

	if (!(left->type == NumberNode) &&
		(right->type == NumberNode) )
	{
		printf("[binop] left or right side of '%s' binop was not a number\n", AST_TypeRepr(op->type));
		return 0;
	}
	
	AST_Number* result = (AST_Number*) malloc(sizeof(AST_Number));

	result->type = NumberNode;

	if (op->type == PlusNode)     result->number = left->number + right->number;
	if (op->type == MinusNode)    result->number = left->number - right->number;
	if (op->type == MultiplyNode) result->number = left->number * right->number;
	if (op->type == DivideNode)   result->number = left->number / right->number;

	return (AST_Node*)result;
}

AST_Node* Statement(TokenArray* ta, int* tlindex)
{
	AST_Node* statement = 0;
	d_printf("[statement] constructing statement\n");

	/* statement:
		variable declaration ';'
		type definition ';'
		expression ';' */

	statement = VariableDeclaration(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!statement) statement = TypeDefinition(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!statement) statement = Expression(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!statement) 
	{
		// is the current token a semicolon?
		if (Token_isType(ta->tokens[*tlindex], "semicolon"))
		{
			*tlindex += 1;
			return 0;
		}
		else
		{
			// ...wait, all three rules failed and it's not a semicolon?
			puts("[statement] hey i couldn't parse this statement\n");

			// prints tokens up to next semicolon
			int end = statementSeparator(ta, *tlindex);
			for (int i = *tlindex; i < end+1; ++i) printf("%s ", ta->tokens[i]->data);
			puts("\n");

			// this is the only time an error doesn't set tlindex to -1 (for debugging)
			// the program will just skip things that aren't implemented yet
			*tlindex = end+1;
			return 0;
		}
	}

	// so by now we expect the next token to be a semicolon
	if (Token_isType(ta->tokens[*tlindex], "semicolon"))
	{
		*tlindex += 1;
		d_printf("[statement] returning '%s' node\n", AST_TypeRepr(statement->type));
		return statement;
	}
	else
	{
		*tlindex = -1;
		puts("[statement] hey a statement's gotta end with a semicolon\n");
		return 0;
	}
}

AST_Node* TypeDefinition(TokenArray* ta, int* tlindex)
{
	return 0;
}

AST_Node* VariableDeclaration(TokenArray* ta, int* tlindex)
{
	AST_VariableDeclaration* vdecl = 0;

	/* variable decl : 
		typename followed by identifier
		typename then assignment
	*/

	/*
	if first token is not an identifier, return 0;
	if second token is not an identifier, return 0;
	else, construct the two identifier nodes and the variable declaration node;
	if next token is assignment token, 
	*/

	int index = *tlindex;

	// check that next two tokens are identifiers
	if (!Token_isType(ta->tokens[index], "identifier")) return 0;
	IncrementWithBoundsCheck(index);
	if (!Token_isType(ta->tokens[index], "identifier")) return 0;

	// initialise vdecl node
	vdecl = (AST_VariableDeclaration*) malloc(sizeof(AST_VariableDeclaration));
	vdecl->type = VariableDeclarationNode;
	vdecl->id = (AST_Identifier*) malloc(sizeof(AST_Identifier));
	vdecl->rt_type = ta->tokens[index-1]->data;
	vdecl->init = 0;

	// initialise identifier node
	vdecl->id->type = IdentifierNode;
	vdecl->id->tree = 0;
	vdecl->id->name = ta->tokens[index]->data;

	// check if there is assignment too
	IncrementWithBoundsCheck(index);
	if (Token_isType(ta->tokens[index], "assignment"))
	{
		*tlindex = index - 1;
		vdecl->init = Assignment(ta, tlindex);
		if (*tlindex == -1) return 0;
		if (vdecl->init == 0)
		{
			// "this should never happen" since it's an error for Assignment too
			puts("[variabledeclaration] hey, expected assignment after '='");
			*tlindex == -1;
			return 0;
		}
	}

	else *tlindex = index;

	return vdecl;
}

AST_Node* Expression(TokenArray* ta, int* tlindex)
{
	/* expression:
		identifier
		assignment
		function def
		function call
		numeric expression
	*/

	AST_Node* expression = 0;

	expression = Identifier(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!expression) expression = Assignment(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!expression) expression = FunctionDefinition(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!expression) expression = FunctionCall(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!expression) expression = NumericExpression(ta, tlindex);
	if (*tlindex == -1) return 0;

	return expression;
}

AST_Node* Identifier(TokenArray* ta, int* tlindex)
{
	AST_Identifier* identifier = 0;

	/* identifiers are separated out by the lexer,
	so there is no grammar to implement except "is it an identifier token?" */

	if (Token_isType(ta->tokens[*tlindex], "identifier"))
	{
		identifier = (AST_Identifier*) malloc(sizeof(AST_Identifier));
		identifier->type = IdentifierNode;
		identifier->tree = 0;
		identifier->name = ta->tokens[*tlindex]->data;
		identifier->rt_type = 0;
		*tlindex += 1;
		return identifier;
	}

	return 0;
}

AST_Node* Assignment(TokenArray* ta, int* tlindex)
{
	AST_Assignment* assign = 0;

	/* assignment:
		identifier '=' expression
	*/

	int index = *tlindex;

	if (!Token_isType(ta->tokens[index], "identifier")) return 0;
	Token* idToken = ta->tokens[index];

	IncrementWithBoundsCheck(index);
	if (!Token_isType(ta->tokens[index], "assignment")) return 0;

	// create new assignment node
	assign = (AST_Assignment*) malloc(sizeof(AST_Assignment));
	assign->type = AssignmentNode;
	assign->left = idToken->data;

	IncrementWithBoundsCheck(index);
	*tlindex = index;
	assign->right = Expression(ta, tlindex);
	if (*tlindex == -1) return 0;
	if (assign->right == 0)
	{
		puts("[assignment] hey, i expected an expression after the '=' sign");
		*tlindex = -1;
		freeast((AST_Node*)assign);
		return 0;
	}

	return assign;
}

AST_Node* FunctionDefinition(TokenArray* ta, int* tlindex)
{
	return 0;
}

//AST_Node* FunctionDefinitionParameters(TokenArray* ta, int* tlindex)

AST_Node* FunctionCall(TokenArray* ta, int* tlindex)
{
	return 0;
}

//AST_Node* FunctionCallParameters(TokenArray* ta, int* tlindex)

AST_Node* NumericExpression(TokenArray* ta, int* tlindex)
{
	/* numeric expression:
		summation of terms
	*/

	AST_Node* numericExpression = 0;

	numericExpression = NumericTerm(ta, tlindex);
	if (*tlindex == -1) return 0;

	/* while the next token is a plus or minus,
	construct a binary op node and set it's left and right sides accordingly */

	while(Token_isType(ta->tokens[*tlindex], "plus") ||
		Token_isType(ta->tokens[*tlindex], "minus") )
	{
		if (Token_isType(ta->tokens[*tlindex], "plus"))
		{
			*tlindex += 1;
			AST_BinOp* plusop = (AST_BinOp*) malloc(sizeof(AST_BinOp));
			plusop->type = PlusNode;
			plusop->left = numericExpression;
			plusop->right = NumericTerm(ta, tlindex);
			if (*tlindex == -1) return 0;
			numericExpression = (AST_Node*)plusop;
		}

		else if (Token_isType(ta->tokens[*tlindex], "minus"))
		{
			*tlindex += 1;
			AST_BinOp* minusop = (AST_BinOp*) malloc(sizeof(AST_BinOp));
			minusop->type = MinusNode;
			minusop->left = numericExpression;
			minusop->right = NumericTerm(ta, tlindex);
			if (*tlindex == -1) return 0;
			numericExpression = (AST_Node*)minusop;
		}
	}

	return numericExpression;
}

AST_Node* NumericTerm(TokenArray* ta, int* tlindex)
{
	/* term:
		product/division of factors/divisors
		factor
	*/

	AST_Node* numericTerm = 0;

	numericTerm = NumericFactor(ta, tlindex);
	if (*tlindex == -1) return 0;

	while(Token_isType(ta->tokens[*tlindex], "mult") ||
		Token_isType(ta->tokens[*tlindex], "divide") )
	{
		if (Token_isType(ta->tokens[*tlindex], "mult"))
		{
			*tlindex += 1;
			AST_BinOp* multop = (AST_BinOp*) malloc(sizeof(AST_BinOp));
			multop->type = MultiplyNode;
			multop->left = numericTerm;
			multop->right = NumericFactor(ta, tlindex);

			if (*tlindex == -1) 
			{
				// recursively free the tree that has been allocated up to this point
				return 0;
			}

			numericTerm = (AST_Node*)multop;
		}

		else if (Token_isType(ta->tokens[*tlindex], "divide"))
		{
			*tlindex += 1;
			AST_BinOp* divop = (AST_BinOp*) malloc(sizeof(AST_BinOp));
			divop->type = DivideNode;
			divop->left = numericTerm;
			divop->right = NumericFactor(ta, tlindex);
			if (*tlindex == -1) return 0;
			numericTerm = (AST_Node*)divop;
		}
	}

	return numericTerm;
}

AST_Node* NumericFactor(TokenArray* ta, int* tlindex)
{
	/* factor:
		'-' factor
		number
		identifier
		factor '^' factor
		'(' numeric expression ')' 
	*/

	AST_Node* numericFactor = 0;

	if (Token_isType(ta->tokens[*tlindex], "l paren"))
	{
		*tlindex += 1;
		numericFactor = NumericExpression(ta, tlindex);
		if (*tlindex == -1) return 0;
		if (!numericFactor)
		{
			puts("[numericfactor] expected numeric expression after left parentheses\n");
			*tlindex = -1;
			return 0;
		}
		if (!Token_isType(ta->tokens[*tlindex], "r paren"))
		{
			puts("[numericfactor] expected right parentheses after expression\n");
			*tlindex = -1;
			return 0;
		}
		*tlindex += 1;
	}

	if (!numericFactor)
	{
		if (Token_isType(ta->tokens[*tlindex], "minus"))
		{
			*tlindex += 1;
			AST_UnaryMinus* um = (AST_UnaryMinus*) malloc(sizeof(AST_UnaryMinus));
			um->type = UnaryMinus;
			um->negate = NumericFactor(ta, tlindex);
			if (*tlindex == -1) return 0;

			numericFactor = (AST_Node*)um;
		}
	}

	if (!numericFactor) numericFactor = Number(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!numericFactor) numericFactor = Identifier(ta, tlindex);
	if (*tlindex == -1) return 0;
	
	return (AST_Node*)numericFactor;
}

AST_Node* Number(TokenArray *ta, int* tlindex)
{
	int index = *tlindex;
	int firstToken = index;

	if (!Token_isType(ta->tokens[index], "digit")) return 0;
	// first token was not digit, return zero but no syntax error.
	else IncrementWithBoundsCheck(index);

	// handle decimal point
	if (Token_isType(ta->tokens[index], "period"))
	{
		IncrementWithBoundsCheck(index);
		if (!Token_isType(ta->tokens[index], "digit"))
		{
			puts("[number] invalid syntax: digits must follow a period.\n");
			*tlindex = -1;
			return 0;
		}
		else IncrementWithBoundsCheck(index);
	}

	// handle scientific notation
	Token t = { -1, "e", "identifier" };
	if (Token_cmp(ta->tokens[index], &t))
	{
		IncrementWithBoundsCheck(index);
		if (Token_isType(ta->tokens[index], "plus") ||
			Token_isType(ta->tokens[index], "minus") ) IncrementWithBoundsCheck(index);

		if (!Token_isType(ta->tokens[index], "digit"))
		{
			puts("[number] invalid syntax: no number after 'e' in numeric literal.\n");
			*tlindex = -1;
			return 0;
		}
		else IncrementWithBoundsCheck(index);
	}

	if (index > ta->len) index = ta->len; // special case at end of input

	/* the index variable now points to the last token in the number (exclusive)
	hence, we can build the number by concatenating all the data strings. */

	// init output buffer
	char* number = (char*)malloc(1);
	number[0] = 0;

	// cat data strings
	for (int i = firstToken; i < index; ++i) number = strappend(number, ta->tokens[i]->data);

	AST_Number* numb = (AST_Number*) malloc(sizeof(AST_Number));
	numb->type = NumberNode;
	numb->number = atof(number);
	*tlindex = index;

	// free number (strappend makes calls to malloc)
	free(number);

	return (AST_Node*)numb;
}

void printIndent(FILE* fp, int number)
{
	for (int i = 0; i < number; ++i) fprintf(fp, "\t");
}

void AST_FPrint(FILE* fp, AST_Node* tree)
{
	static int indentLevel;

	if (!tree)
	{
		printIndent(fp, indentLevel);
		fprintf(fp, "(null pointer)\n");
	}

	else if (tree->type == None)
	{
		printIndent(fp, indentLevel);
		fprintf(fp, "None\n");
	}

	else if (tree->type == NumberNode)
	{
		AST_Number* number = (AST_Number*) tree;
		printIndent(fp, indentLevel);
		fprintf(fp, "num:%lf\n", number->number);
	}

	else if (
		(tree->type == PlusNode) ||
		(tree->type == MinusNode) ||
		(tree->type == MultiplyNode) ||
		(tree->type == DivideNode) )
	{
		AST_BinOp* bop = (AST_BinOp*) tree;
		
		indentLevel++;
		AST_FPrint(fp, (AST_Node*)bop->right);
		indentLevel--;

		printIndent(fp, indentLevel);
		fprintf(fp, "%s\n", AST_TypeRepr(bop->type));
		
		indentLevel++;
		AST_FPrint(fp, (AST_Node*)bop->left);
		indentLevel--;

	}

	else if (tree->type == UnaryMinus)
	{
		AST_UnaryMinus* um = (AST_UnaryMinus*) tree;

		indentLevel++;
		AST_FPrint(fp, (AST_Node*)um->negate);
		indentLevel--;

		printIndent(fp, indentLevel);
		fprintf(fp, "UnaryMinus\n");
	}

	else
	{
		printIndent(fp, indentLevel);
		fprintf(fp, "{Unknown}\n");
	}
}
