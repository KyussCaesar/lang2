#ifndef ARRAYS_H
#define ARRAYS_H

/* defines stuff for my custom arrays */

/* custom array types look like this

typedef struct {
	etype** array;
	int len;
	int cap;
} CustomArrayName;

// type: type to define array extend for (CustomArrayName)
// etype: type of the elements in the array
// array: name of the array in the struct
// len : number of elements in the array
// cap : max number of elements in the array
// array expansion is done like std::vector: doubles size each time
*/

// These macros expand to function definitions =========================

// defines a new array type
#define DefineArrayType(type, etype, array)\
typedef struct {\
	etype** array;\
	int len;\
	int cap;\
} type;

// defines a function that creates new instance of array
#define DefineArrayNew(type, etype, array)\
type* New##type(type* x, int cap)\
{\
	if (cap < 4) x->cap = 4;\
	else x->cap = cap;\
	x->len = 0;\
	x->array = (etype**) malloc(sizeof(etype*) * x->cap);\
	return x;\
}

// defines a function that expands an array
#define DefineArrayExtend(type, etype, array) \
void type##_Extend(type* x) \
{ \
	etype** old = x->array;\
	x->array = (etype**)malloc(sizeof(etype*) * 2 * x->cap);\
	x->cap = 2 * x->cap;\
	for (int i = 0; i < x->len; ++i) x->array[i] = old[i];\
	free(old);\
}

// defines a function that adds an element to an array
#define DefineArrayAdd(type, etype, array)\
void type##_Add(type* x, etype* a)\
{\
	if (x->len == x->cap) type##_Extend(x);\
	x->array[x->len] = a;\
	x->len += 1;\
}

// defines a function that removes an element from an array. will not resize the array
#define DefineArrayRemove(type, etype, array)\
void type##_Remove(type* x, int index)\
{\
	if (index >= x->len) \
	{\
		puts("hey you tried to remove an out-of bounds element from a type array\n");\
	}\
	else\
	{\
		for (int i = index; i+1 < x->len; ++i) x->array[i] = x->array[i+1];\
		x->len -= 1;\
	}\
}

// defines a function that frees the array, but *not* the elements it holds.
#define DefineArrayFree(type, etype, array)\
void type##_Free(type* x)\
{\
	free(x->array);\
	free(x);\
}

#endif
