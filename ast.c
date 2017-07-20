#include"ast.h"
#include"symboltable.h"

/*
ISSUES:

IncrementWithBoundsCheck:
IncrementWithBoundsCheck is a really silly way of doing things I think.
it could be much neater to have a lexer object which has a "current token" attribute,
that way, these functions don't have to check if they have gone out of bounds.

It's also really silly that each function keeps track of the index... come to think of
it, it's actually really silly that each function even knows what an "index into the
token array" *is*, constrast to each function just saying "the current token"

course of action:
probably nothing, since it's all throughout the code in this file and the holiday is 
almost over. it's not the nicest way of doing things but it *works* (for now).
*/

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
		case None:                   return "None";
		case AssignmentNode:         return "AssignmentNode";
		case VariableReferenceNode:  return "VariableReferenceNode";
		case NumberNode:             return "NumberNode";
		case PlusNode:               return "PlusNode";
		case MinusNode:              return "MinusNode";
		case MultiplyNode:           return "MultiplyNode";
		case DivideNode:             return "DivideNode";
		case UnaryMinus:             return "UnaryMinus";
		default:                     return "{Unknown}";
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

	if (root->type == None) return 0; // "no-op"

	if (root->type == NumberNode)
	{
		AST_Number* numb = (AST_Number*) malloc(sizeof(AST_Number));
		numb->type = NumberNode;
		numb->number = (*(AST_Number*)root).number;
		numb->rt_type = (SymbolTableEntry_TypeName*) SymbolTable_Find(global_symbol_table, "Number");
		d_printf("[execute] number with value %lf\n", numb->number);
		return (AST_Node*)numb;
	}

	if (root->type == BooleanNode)
	{
		AST_Boolean* boolean = (AST_Boolean*) malloc(sizeof(AST_Boolean));
		boolean->type = BooleanNode;
		boolean->rt_type = (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Boolean");
		boolean->val = (*(AST_Boolean*)root).val;
		return (AST_Node*)boolean;
	}

	if (
		(root->type == PlusNode) ||
		(root->type == MinusNode) ||
		(root->type == MultiplyNode) ||
		(root->type == DivideNode) ||
		(root->type == LogicalAndNode) ||
		(root->type == LogicalOrNode) ||
		(root->type == LogicalEqualsNode) )
	{
		d_printf("[execute] executing binop '%s'\n", AST_TypeRepr(root->type));

		AST_BinOp* op = (AST_BinOp*)root;

		AST_Node* lop = execute(op->left);
		AST_Node* rop = execute(op->right);

		AST_Node* masterresult = 0;

		if (op->rt_type == (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Number"))
		{
			AST_Number* result = (AST_Number*)malloc(sizeof(AST_Number));
			result->type = NumberNode;
			result->rt_type = op->rt_type;

			AST_Number* left = (AST_Number*) lop;
			AST_Number* right = (AST_Number*) rop;

			if      ( op->type ==   PlusNode   ) result->number = left->number + right->number;
			else if ( op->type ==  MinusNode   ) result->number = left->number - right->number;
			else if ( op->type == MultiplyNode ) result->number = left->number * right->number;
			else if ( op->type ==  DivideNode  ) result->number = left->number / right->number;

			masterresult = (AST_Node*) result;
		}

		else if (op->rt_type == (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Boolean"))
		{
			AST_Boolean* result = (AST_Boolean*)malloc(sizeof(AST_Boolean));
			result->type = BooleanNode;
			result->rt_type = op->rt_type;

			if ( op->type == LogicalAndNode )
			{
				AST_Boolean* left = (AST_Boolean*) lop;
				AST_Boolean* right = (AST_Boolean*) rop;
				if ((left->val == 'T') && (right->val == 'T')) result->val = 'T';
				else result->val = 'F';
			}
			else if (op->type == LogicalOrNode)
			{
				AST_Boolean* left = (AST_Boolean*) lop;
				AST_Boolean* right = (AST_Boolean*) rop;
				if ((left->val == 'T') || (right->val == 'T')) result->val = 'T';
				else result->val = 'F';
			}
			else if (op->type == LogicalEqualsNode)
			{
				// check that left and right have the same rt_type
				if (lop->rt_type != rop->rt_type) return 0;
				if (lop->rt_type == (SymbolTableEntry_TypeName*) SymbolTable_Find(global_symbol_table, "Boolean"))
				{
					AST_Boolean* left = (AST_Boolean*) lop;
					AST_Boolean* right = (AST_Boolean*) rop;
					result->val = left->val == right->val;
				}
				else if (lop->rt_type == (SymbolTableEntry_TypeName*) SymbolTable_Find(global_symbol_table, "Number"))
				{
					AST_Number* left = (AST_Number*) lop;
					AST_Number* right = (AST_Number*) rop;
					if ((left->number - right->number) * (left->number - right->number) < 0.001) result->val = 'T';
					else result->val = 'F';
				}
			}

			masterresult = (AST_Node*) result;
		}

		return masterresult;
	}

	if (root->type == VariableReferenceNode)
	{
		AST_Variable* var = (AST_Variable*)root;
		return execute(var->variable->tree);
	}

	if (root->type == AssignmentNode)
	{
		AST_Assignment* node = (AST_Assignment*)root;

		/* when you do reference counting, then you will have to update the tree that 
		the left side used to point to, here. */

		node->left->tree = node->right;
	}

	if (root->type == UnaryMinus)
	{
		AST_UnaryMinus* um = (AST_UnaryMinus*) root;
		AST_Number* minus = (AST_Number*)execute(um->negate);
		minus->number *= -1;

		return (AST_Node*) minus;
	}

	if (root->type == LogicalNotNode)
	{
		AST_LogicalNot* not = (AST_LogicalNot*) root;
		AST_Boolean* negate = (AST_Boolean*)execute(not->negate);
		if (negate->val == 'T') negate->val = 'F';
		else negate->val = 'T';
		return (AST_Node*) negate;
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

	if (n->type == VariableReferenceNode)
	{
		AST_Variable* var = (AST_Variable*) n;
		freeast(var->variable->tree);
		free(var->variable->symbol);
		free(var->variable);
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

int BinOpSemanticCheck(AST_BinOp* bop)
{
	/* semantic check:
		left and right must be of the same type */

	if (bop->left->rt_type != bop->right->rt_type)
	{
		printf("[binop] hey, the left and right sides have different types ('%s' and '%s', respectively)\n",
			bop->left->rt_type->symbol,
			bop->right->rt_type->symbol);
		return 0;
	}

	/* semantic check:
		left and right must be of Number type (for now) */

	if (bop->left->rt_type != (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Number"))
	{
		puts("[binop] hey, left and right must be 'Number' type");
		return 0;
	}
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

	if (!statement) statement = IfStatement(ta, tlindex);
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
		AST_FPrint(stdout, statement);
		return 0;
	}
}

AST_Node* VariableDeclaration(TokenArray* ta, int* tlindex)
{
	AST_Node* vdecl = 0;

	/* variable decl :
		identifier identifier
		identifier then assignment
	*/

	int index = *tlindex;

	/* grammar check: are the next two tokens identifiers?
	{*/
	if (!Token_isType(ta->tokens[index], "identifier")) return 0;
	IncrementWithBoundsCheck(index);
	if (!Token_isType(ta->tokens[index], "identifier")) return 0;
	//}

	/* semantic check:
	is the first token:
		in the symbol table already AND
		an identifier that refers to a type? */
	Token* typetoken = ta->tokens[index-1];
	SymbolTableEntry* typesymbolintable = SymbolTable_Find(global_symbol_table, typetoken->data);
	SymbolTableEntry_TypeName* typenameentry = 0;
	//{
	if (typesymbolintable) // found the entry in the symbol table; check that it refers to a type
	{
		if (typesymbolintable->entrytype != TypeName)
		{
			*tlindex = -1;
			printf("[variabledeclaration] hey, '%s' is not a type name\n", typesymbolintable->symbol);
			return 0;
		}
		
		else
		{
			typenameentry = (SymbolTableEntry_TypeName*) typesymbolintable;
		}
	}

	else
	{
		*tlindex = -1;
		printf("[variabledeclaration] hey, '%s' is not a known type name\n", typetoken->data);
		return 0;
	}
	//}

	/* semantic check:
	is the second token already in the symbol table? (it shouldn't be) */
	Token* variabletoken = ta->tokens[index];
	SymbolTableEntry* variableinsymboltable = SymbolTable_Find(global_symbol_table, variabletoken->data);
	//{
	if (variableinsymboltable)
	{
		if (variableinsymboltable->entrytype == VariableName)
		{
			SymbolTableEntry_VariableName* variable = (SymbolTableEntry_VariableName*) variableinsymboltable;
			printf("[variabledeclaration] hey, '%s' has already been declared (with type '%s')\n",
				variable->symbol,
				variable->rt_type->symbol);

			*tlindex = -1;
			return 0;
		}
		
		else
		{
			printf("[variabledeclaration] hey, the name '%s' already refers to a '%s'\n",
				variabletoken->data,
				SymbolTable_TypeRepr(variableinsymboltable->entrytype));

			*tlindex = -1;
			return 0;
		}
	}
	//}

	/* cool beans; construct a new entry for the variable in the symbol table
	and initialise it properly */

	// construct new entry for symbol table
	SymbolTableEntry_VariableName* newvariable =
		(SymbolTableEntry_VariableName*) malloc(sizeof(SymbolTableEntry_VariableName));
	newvariable->entrytype = VariableName;
	newvariable->symbol = variabletoken->data;
	newvariable->tree = 0;
	newvariable->rt_type = typenameentry;

	// add new variable to the symbol table
	SymbolTable_Add(global_symbol_table, (SymbolTableEntry*)newvariable);
	
	// check if the next token is the assignment token
	IncrementWithBoundsCheck(index);
	if (Token_isType(ta->tokens[index], "assignment"))
	{
		*tlindex = index - 1;
		vdecl = Assignment(ta, tlindex);
		if (*tlindex == -1) return 0;
		if (vdecl == 0)
		{
			// next token was assignment; assignment cannot fail without eror so this should never run
			// (famous last words, I know)
			puts("[varaibledeclaration] hey, I expected an assignment statement after '=' sign");
			*tlindex = -1;
			freeast((AST_Node*)vdecl);
			return 0;
		}
	}

	else
	{
		*tlindex = index;
		// construct a "none" node
		vdecl = (AST_Node*) malloc(sizeof(AST_Node));
		vdecl->type = None;
	}

	d_printf("exit variable declaration\n");
	return (AST_Node*) vdecl;
}

AST_Node* TypeDefinition(TokenArray* ta, int* tlindex)
{
	// not implemented yet
	return 0;
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

	expression = Assignment(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!expression) expression = FunctionDefinition(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!expression) expression = FunctionCall(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!expression) expression = BooleanExpression(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!expression) expression = NumericExpression(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!expression) expression  = VariableReference(ta, tlindex);
	if (*tlindex == -1) return 0;

	d_printf("exit expression\n");
	return expression;
}

AST_Node* VariableReference(TokenArray* ta, int* tlindex)
{
	AST_Variable* identifier = 0;

	int index = *tlindex;

	/* grammar check:
		is the current token an identifier type? */
	if (!Token_isType(ta->tokens[index], "identifier")) return 0;

	/* semantic check:
		1: is the identifier defined in the symbol table AND
		2: does it refer to a variable? */

	SymbolTableEntry* ste = SymbolTable_Find(global_symbol_table, ta->tokens[index]->data);
	if (ste)
	{
		if (ste->entrytype == VariableName)
		{
			SymbolTableEntry_VariableName* variable = (SymbolTableEntry_VariableName*) ste;
			IncrementWithBoundsCheck(index);
			*tlindex = index;
			identifier = (AST_Variable*) malloc(sizeof(AST_Variable));
			identifier->type = VariableReferenceNode;
			identifier->rt_type = variable->rt_type;
			identifier->variable = variable;
		}

		else
		{
			printf("[identifier] hey, '%s' does not refer to a variable\n", ste->symbol);
			*tlindex = -1;
			return 0;
		}
	}

	d_printf("exit variable reference\n");
	return (AST_Node*)identifier;
}

AST_Node* Assignment(TokenArray* ta, int* tlindex)
{
	AST_Assignment* assign = 0;

	/* assignment:
		identifier '=' expression
	*/

	int index = *tlindex;

	/* grammar check:
		first token should be an identifier
		second token should be assignment
		what follows should be an expression */
	Token* idToken = 0;
	AST_Node* rightside = 0;
	//{
	if (!Token_isType(ta->tokens[index], "identifier")) return 0;
	idToken = ta->tokens[index];

	IncrementWithBoundsCheck(index);
	if (!Token_isType(ta->tokens[index], "assignment")) return 0;

	IncrementWithBoundsCheck(index);
	*tlindex = index;
	rightside = Expression(ta, tlindex);
	if (*tlindex == -1) return 0;
	if (!rightside)
	{
		puts("[assignment] hey, I expected an expression after the '='");
		*tlindex = -1;
		return 0;
	}
	//}

	/* semantic check:
		left side should be a known symbol that is a reference to a variable */
	SymbolTableEntry* ste = SymbolTable_Find(global_symbol_table, idToken->data);
	//{
	if (ste)
	{
		if (ste->entrytype != VariableName)
		{
			printf("[assignment] hey, '%s' refers to a '%s', not a variable\n",
				ste->symbol,
				SymbolTable_TypeRepr(ste->entrytype));

			*tlindex = -1;
			return 0;
		}
	}

	else
	{
		printf("[assignment] hey, '%s' is not a known variable\n", idToken->data);
		*tlindex = -1;
		return 0;
	}
	//}

	/* semantic check:
		type of left and right should be the same
		later, this will be a check that they are compatible types */
	SymbolTableEntry_VariableName* leftside = (SymbolTableEntry_VariableName*) ste;
	//{
	if (leftside->rt_type != rightside->rt_type)
	{
		printf("[assignment] hey, '%s' (left side) is a '%s', but the right side evaluates to a '%s'\n",
			leftside->symbol, leftside->rt_type->symbol, rightside->rt_type->symbol);

		*tlindex = -1;
		return 0;
	}
	//}

	// create new assignment node
	assign = (AST_Assignment*) malloc(sizeof(AST_Assignment));
	assign->type = AssignmentNode;
	assign->left = leftside;
	assign->right = rightside;

	d_printf("exit assignment\n");
	return (AST_Node*)assign;
}

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

	int index = *tlindex;

	while(Token_isType(ta->tokens[index], "plus") ||
		Token_isType(ta->tokens[index], "minus") )
	{
		AST_BinOp* plusop = (AST_BinOp*) malloc(sizeof(AST_BinOp));

		if (Token_isType(ta->tokens[index], "plus")) plusop->type = PlusNode;
		else plusop->type = MinusNode;

		plusop->left = numericExpression;

		IncrementWithBoundsCheck(index);
		*tlindex = index;
		plusop->right = NumericTerm(ta, tlindex);
		if (*tlindex == -1) return 0;
		if (plusop->right == 0)
		{
			printf("[expression] hey, I expected a term after '%s'\n", ta->tokens[*tlindex]->data);
			*tlindex = -1;
			return 0;
		}

		if (BinOpSemanticCheck(plusop))
		{
			plusop->rt_type = (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Number");
			numericExpression = (AST_Node*) plusop;
		}

		else
		{
			*tlindex = -1;
			return 0;
		}

		index = *tlindex;
	}

	d_printf("exit numeric expression\n");
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

	int index = *tlindex;

	while(Token_isType(ta->tokens[index], "mult") ||
		Token_isType(ta->tokens[index], "divide") )
	{
		AST_BinOp* multop = (AST_BinOp*) malloc(sizeof(AST_BinOp));

		if (Token_isType(ta->tokens[index], "mult")) multop->type = MultiplyNode;
		else multop->type = DivideNode;

		multop->left = numericTerm;

		IncrementWithBoundsCheck(index);
		*tlindex = index;

		multop->right = NumericFactor(ta, tlindex);
		if (*tlindex == -1) 
		{
			freeast((AST_Node*)multop);
			return 0;
		}

		if (multop->right == 0)
		{
			puts("[term] hey, expected a factor");
			freeast((AST_Node*)multop);
			*tlindex = -1;
			return 0;
		}

		if (BinOpSemanticCheck(multop))
		{
			multop->rt_type = (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Number");
			numericTerm = (AST_Node*)multop;
		}

		else
		{
			*tlindex = -1;
			return 0;
		}

		index = *tlindex;
	}
	
	d_printf("exit numeric term\n");
	return numericTerm;
}

AST_Node* NumericFactor(TokenArray* ta, int* tlindex)
{
	/* factor:
		'(' numeric expression ')' 
		'-' factor
		number
		identifier
	*/

	AST_Node* numericFactor = 0;
	int index = *tlindex;

	// check first rule
	if (Token_isType(ta->tokens[index], "l paren"))
	{
		IncrementWithBoundsCheck(index);
		*tlindex = index;
		numericFactor = NumericExpression(ta, tlindex);
		if (*tlindex == -1) return 0;
		if (!numericFactor)
		{
			puts("[numericfactor] expected expression after left parentheses\n");
			*tlindex = -1;
			return 0;
		}

		index = *tlindex;

		// next token should now be the closing parentheses
		if (!Token_isType(ta->tokens[index], "r paren"))
		{
			puts("[numericfactor] expected right parentheses after expression\n");
			*tlindex = -1;
			return 0;
		}
		IncrementWithBoundsCheck(index);
		*tlindex = index;
	}

	// check second rule
	if (!numericFactor && Token_isType(ta->tokens[index], "minus"))
	{
		AST_UnaryMinus* um = (AST_UnaryMinus*) malloc(sizeof(AST_UnaryMinus));
		um->type = UnaryMinus;

		IncrementWithBoundsCheck(index);
		*tlindex = index;
		um->negate = NumericFactor(ta, tlindex);
		if (*tlindex == -1)
		{
			freeast((AST_Node*)um);
			return 0;
		}

		/* semantic check:
			negate is only defined for Numbers */

		if (um->negate->rt_type != (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Number"))
		{
			puts("[factor] unary minus '-' is only defined for numbers at the moment");
			*tlindex = -1;
			freeast((AST_Node*)um);
			return 0;
		}

		um->rt_type = um->negate->rt_type;

		numericFactor = (AST_Node*)um;
	}

	if (!numericFactor) numericFactor = Number(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!numericFactor) numericFactor = VariableReference(ta, tlindex);
	if (*tlindex == -1) return 0;

	if (!numericFactor) return 0;

	/* semantic check:
		is it a Number?
	*/
	if (numericFactor->rt_type != (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Number"))
	{
		printf("[number] hey, I expected a 'Number' type but I got a '%s'\n", numericFactor->rt_type->symbol);
		*tlindex = -1;
		return 0;
	}

	d_printf("exit numericfactor\n");
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
	numb->rt_type = (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Number");
	*tlindex = index;

	// free number (strappend makes calls to malloc)
	free(number);

	d_printf("exit number\n");
	return (AST_Node*)numb;
}

AST_Node* IfStatement(TokenArray* ta, int* tlindex)
{
	/* ifstatement:
		'if' booleanexpression statement
	*/

	AST_If* ifstatement = 0;
	int index = *tlindex;

	Token ifToken = {-1, "if", "keyword"};

	/* grammar check:
		current token must be the word "if"
	*/
	if (!Token_cmp(ta->tokens[index], &ifToken)) return 0;

	// construct new if statement node
	ifstatement = (AST_If*) malloc(sizeof(AST_If));
	ifstatement->type = IfNode;
	ifstatement->rt_type = 0;
	ifstatement->bexpr = 0;
	ifstatement->trueTree = 0;
	ifstatement->falseTree = 0;

	/* grammar check:
		next thing must be a boolean expression
	*/
	IncrementWithBoundsCheck(index);
	*tlindex = index;
	ifstatement->bexpr = BooleanExpression(ta, tlindex);
	if (*tlindex == -1) return 0;
	if (ifstatement->bexpr == 0)
	{
		puts("[ifstatement] hey, 'if' statement must be followed by a boolean expression");
		*tlindex = -1;
		return 0;
	}

	while (Token_isType(ta->tokens[index], "equality"))
	{
		// construct node
		AST_BinOp* bop = (AST_BinOp*) malloc(sizeof(AST_BinOp));
		bop->type = LogicalEqualsNode;
		bop->rt_type = (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Boolean");
		bop->left = ifstatement->bexpr;
		bop->right = 0;

		ifstatement->bexpr = (AST_Node*)bop;

		IncrementWithBoundsCheck(index);
		*tlindex = index;
		bop->right = BooleanExpression(ta, tlindex);
		if (*tlindex == -1) return 0;
		if (bop->right == 0)
		{
			puts("[booleanexpression] hey, I expected another expression after the '=='");
			*tlindex = -1;
			freeast((AST_Node*)ifstatement);
			return 0;
		}

		index = *tlindex;
	}

	/* grammar check:
		next thing must be a statement
	*/
	ifstatement->trueTree = Statement(ta, tlindex);
	if (*tlindex == -1) return 0;
	if (ifstatement->trueTree == 0)
	{
		puts("[if] hey, expected statement1 after 'if' condition");
		*tlindex = -1;
		return 0;
	}

	/* grammar check:
		next thing must also be a statement
	*/
	ifstatement->falseTree = Statement(ta, tlindex);
	if (*tlindex == -1) return 0;
	if (ifstatement->falseTree == 0)
	{
		puts("[if] hey, expected statement after 'if' condition");
		*tlindex = -1;
		return 0;
	}

	return (AST_Node*)ifstatement;
}

AST_Node* BooleanExpression(TokenArray* ta, int* tlindex)
{
	/* boolean expression:
		boolean term [ '|' boolean term ]
	*/

	AST_Node* bexpr = 0;

	/* grammar check:
		is the next thing a boolean term?
	*/
	bexpr = BooleanTerm(ta, tlindex);
	if (*tlindex == -1) return 0;	// boolean term returned with error
	if (!bexpr) return 0;			// boolean term returned without error

	int index = *tlindex;

	if (Token_isType(ta->tokens[index], "equality"))
	{
		AST_BinOp* bop = (AST_BinOp*) malloc(sizeof(AST_BinOp));
		bop->type = LogicalEqualsNode;
		bop->rt_type = (SymbolTableEntry_TypeName*) SymbolTable_Find(global_symbol_table, "Boolean");
		bop->left = bexpr;
		bop->right = 0;
		bexpr = bop;

		IncrementWithBoundsCheck(index);
		*tlindex = index;
		bop->right = Expression(ta, tlindex);
		if (*tlindex == -1) return 0;
		if (bop->right == 0)
		{
			puts("[booleanexpression] hey, I expected an expression after '=='");
			*tlindex = -1;
			return 0;
		}

		index = *tlindex;
	}

	else if (
		Token_isType(ta->tokens[index], "plus") ||
		Token_isType(ta->tokens[index], "minus") ||
		Token_isType(ta->tokens[index], "multiply") ||
		Token_isType(ta->tokens[index], "divide") )
	{
		// whoops; it's a numeric expression, not a boolean expression
		AST_BinOp* bop = (AST_BinOp*) malloc(sizeof(AST_BinOp));

		if (Token_isType(ta->tokens[index], "plus")) bop->type = PlusNode;
		else if (Token_isType(ta->tokens[index], "minus")) bop->type = MinusNode;
		else if (Token_isType(ta->tokens[index], "multiply")) bop->type = MultiplyNode;
		else if (Token_isType(ta->tokens[index], "divide")) bop->type = DivideNode;

		bop->rt_type = (SymbolTableEntry_TypeName*) SymbolTable_Find(global_symbol_table, "Number");
		bop->left = bexpr;
		bop->right = 0;

		IncrementWithBoundsCheck(index);
		*tlindex = index;
		bop->right = NumericExpression(ta, tlindex);
		if (*tlindex == -1) return 0;
		if (bop->right == 0)
		{
			puts("hey i expected an expression");
			*tlindex = -1;
			return 0;
		}
	}

	/* grammar check:
		is the next token the '|' operator?
	*/
	while (Token_isType(ta->tokens[index], "logical or"))
	{
		// create a binop
		AST_BinOp* bop = (AST_BinOp*) malloc(sizeof(AST_BinOp));
		bop->type = LogicalOrNode;
		bop->rt_type = (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Boolean");
		bop->left = bexpr;
		bop->right = 0;
		bexpr = (AST_Node*)bop;

		// next thing should be another boolean term
		IncrementWithBoundsCheck(index);
		*tlindex = index;
		bop->right = BooleanTerm(ta, tlindex);
		if (*tlindex == -1) return 0;
		if (bop->right == 0)
		{
			// next thing wasn't a boolean term; error
			puts("[booleanexpression] hey, I expected another term after the '|'");
			*tlindex = -1;
			freeast(bexpr); // cleanup
			return 0;
		}

		index = *tlindex;
	}

	return bexpr;
}

AST_Node* BooleanTerm(TokenArray* ta, int* tlindex)
{
	/* boolean term:
		boolean factor [ '&' boolean factor ]
	*/

	AST_Node* bterm = 0;

	/* grammar check
		is the next thing a boolean factor?
	*/
	bterm = BooleanFactor(ta, tlindex);
	if (*tlindex == -1) return 0; // bfactor returned with error
	if (!bterm) return 0; // not a bfactor

	int index = *tlindex;

	while (Token_isType(ta->tokens[index], "logical and"))
	{
		// create a new binop
		AST_BinOp* bop = (AST_BinOp*) malloc(sizeof(AST_BinOp));
		bop->type = LogicalAndNode;
		bop->rt_type = (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Boolean");
		bop->left = bterm;
		bop->right = 0;

		bterm = (AST_Node*)bop;

		IncrementWithBoundsCheck(index);
		*tlindex = index;
		bop->right = BooleanFactor(ta, tlindex);
		if (*tlindex == -1) return 0;
		if (bop->right == 0)
		{
			puts("[booleanterm] hey, I expected another boolean factor after the '&'");
			*tlindex = -1;
			freeast((AST_Node*)bterm);
			return 0;
		}

		if (bop->right->rt_type != (SymbolTableEntry_TypeName*) SymbolTable_Find(global_symbol_table, "Boolean"))
		{
			puts("[booleanterm] hey, both sides of an '&' must be boolean type");
			*tlindex = -1;
			return 0;
		}

		index = *tlindex;
	}

	return bterm;
}

AST_Node* BooleanFactor(TokenArray* ta, int* tlindex)
{
	/* boolean factor:
		'!' booleanfactor
		'(' boolean expression ')'
		variable reference
		booleanliteral
	*/
	AST_Node* bfactor = 0;

	int index = *tlindex;

	if (Token_isType(ta->tokens[index], "logical not"))
	{
		AST_LogicalNot* lot = (AST_LogicalNot*) malloc(sizeof(AST_LogicalNot));
		lot->type = LogicalNotNode;
		lot->rt_type = (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Boolean");
		lot->negate = 0;
		bfactor = (AST_Node*) lot;

		IncrementWithBoundsCheck(index);
		*tlindex = index;
		lot->negate = BooleanFactor(ta, tlindex);
		if (*tlindex == -1) return 0;
		if (lot->negate == 0)
		{
			puts("[booleanfactor] hey, I expected a boolean factor after the '!'");
			*tlindex = -1;
			freeast((AST_Node*)bfactor);
			return 0;
		}

		index = *tlindex;

	}

	else if (Token_isType(ta->tokens[index], "l paren"))
	{
		IncrementWithBoundsCheck(index);
		*tlindex = index;
		bfactor = BooleanExpression(ta, tlindex);
		if (*tlindex == -1) return 0;
		if (!bfactor)
		{
			puts("[booleanfactor] hey, I expected a boolean expression after the '('");
			*tlindex = -1;
			return 0;
		}

		/* grammar check:
			next token should be a r paren
		*/
		index = *tlindex;
		if (!Token_isType(ta->tokens[index], "r paren"))
		{
			puts("[booleanfactor] hey, I expected a ')' after the boolean expression");
			*tlindex = -1;
			freeast((AST_Node*)bfactor);
			return 0;
		}

		IncrementWithBoundsCheck(index);
		*tlindex = index;
	}

	else if (Token_isType(ta->tokens[index], "identifier"))
	{
		bfactor = VariableReference(ta, tlindex);
		if (*tlindex == -1) return 0;

		if (!bfactor) bfactor = BooleanLiteral(ta, tlindex);
		if (*tlindex == -1) return 0;

		if (!bfactor) return 0;
	}

	return (AST_Node*)bfactor;
}

AST_Node* BooleanLiteral(TokenArray* ta, int* tlindex)
{
	/* blit:
		'TRUE'
		'FALSE'
	*/

	AST_Node* litbool = 0;

	Token tToken = {-1, "TRUE", "identifier"};
	Token fToken = {-1, "FALSE", "identifier"};

	int index = *tlindex;

	if (!Token_isType(ta->tokens[index], "identifier"))
	{
		// construct the new node
		AST_Boolean* lbool = (AST_Boolean*) malloc(sizeof(AST_Boolean));
		lbool->type = BooleanNode;
		lbool->rt_type = (SymbolTableEntry_TypeName*)SymbolTable_Find(global_symbol_table, "Boolean");
		lbool->val = 0;
		litbool = (AST_Node*) lbool;

		if (Token_cmp(ta->tokens[index], &tToken)) lbool->val = 'T';
		if (Token_cmp(ta->tokens[index], &fToken)) lbool->val = 'F';
		if (lbool->val = 0)
		{
			free(litbool);
			return 0;
		}

		IncrementWithBoundsCheck(index);
		*tlindex = index;
	}

	return litbool;
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

	else if (tree->type == VariableReferenceNode)
	{
		AST_Variable* var = (AST_Variable*) tree;
		printIndent(fp, indentLevel);
		fprintf(fp, "var:'%s'\n", var->variable->symbol);
	}

	else if (tree->type == AssignmentNode)
	{
		AST_Assignment* assign = (AST_Assignment*) tree;

		indentLevel++;
		AST_FPrint(fp, (AST_Node*)assign->right);
		indentLevel--;

		printIndent(fp, indentLevel);
		fprintf(fp, "assignment\n");

		indentLevel++;
		printIndent(fp, indentLevel);
		fprintf(fp, "var:%s\n", assign->left->symbol);
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
