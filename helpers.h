
// appends b to a; frees a
/*
The idea is that you call it like

	thing = strappend(thing, thing2);

*/
char* strappend(char* a, char* b)
{
	if (!(a && b)) return 0;

	char* cat = (char*)malloc(strlen(a) + strlen(b) + 1);

	int i = 0;
	while(a[i] != 0) 
	{
		cat[i] = a[i];
		i++;
	}

	int j = 0;
	while(b[j] != 0)
	{
		cat[i] = b[j];
		j++;
		i++;
	}

	cat[i] = 0;
	free(a);
	return cat;
}

typedef struct {
	char** kw;		// keywords
	int len;
	int cap;
} KwArray;

KwArray* NewKwArray(KwArray* kw, int cap)
{
	if (cap < 4) kw->cap = 4;
	else kw->cap = cap;

	kw->len = 0;
	kw->kw = (char**) malloc(sizeof(char*) * kw->cap);

	return kw;
}

void KwArray_Extend(KwArray* kw

// type: type to define array extend for
// etype: type of the elements in the array
// array: name of the array in the struct

#define DefineArrayNew(type, etype, array)\
type* New##type(type* x, int cap)\
{\
	if (cap < 4) x->cap = 4;\
	else x->cap = cap;\
	x->len = 0;\
	x->array = (etype**) malloc(sizeof(etype*) * x->cap);\
	return x;\
}

#define DefineArrayExtend(type, etype, array) \
void type##_Extend(type* x) \
{ \
	etype** old = x->array;\
	x->array = (etype**)malloc(sizeof(etype*) * 2 * x->cap);\
	x->cap = 2 * x->cap;\
	for (int i = 0; i < x->len; ++i) x->array[i] = old[i];\
	free(old);\
}


