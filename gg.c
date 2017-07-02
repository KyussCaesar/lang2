/*
baby steps: write program that reads file and outputs what tokens it found

current tokens:
	identifiers
	base-10 literals
	string literals

token grammar rules:
	identifier : letter followed by zero or more letters or numbers
	base-10 literal : numeric character followed by zero or more numeric characters
	string literal : quote followed by zero or more of any character, followed by quote

in order to do all of this we need:
	tokens
	strings

program loop:
	open file for reading
	read whole file into memory
	parse tokens
	write to output
*/

#define typestruct typedef struct
#define typeenum typedef enum
#define New(x, a) Newx(((x)*)malloc(sizeof(x)), a)

typestruct {
	void * this;
	char * type;
} Object;

Object * NewObject(Object * object, Dict * args)
{
	object->this = object;

	if (args)
	{
		object->type = args->k("type");
	}
	else {
		object->type = "None";
	}
}

typestruct {
	Object obj;
	
} Dict;

Object * NewDict(Dict * dict, Dict * args)
{

}

typestruct {
	String * data;
} Token;
