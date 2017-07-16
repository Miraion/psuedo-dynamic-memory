/*
	Pseudo generic stack object definition.

	TOKENPASTE macro and pseudo generic idea from:
		https://stackoverflow.com/questions/16522341/pseudo-generics-in-c

	Example Declaration in .c file:
		#define TYPE char
		#include "stack.h"
		#undef TYPE

	This will declare a struct named stack_char
	along with functions:
		stack_char make_stack_char()
		void push_char(stack_char*, char)
		char pop_char(stack_char*)
 */

#include "dmemory.h" // for dmalloc_array() and dmfree_array()

// Only define the struct and functions if a type has been defined
#ifdef TYPE

// Default capacity for the dynamic array.
#ifndef CAPACITY
#define CAPACITY 8
#endif

// Macro for creating type names.
// Will appand token y to token x.
#define TOKENPASTE(x, y) x ## y

#define TYPECOEF (sizeof(block) / sizeof(TYPE))

// Generic struct declaration.
#define S(T) TOKENPASTE(stack_, T)
#define STACK S (TYPE)
typedef struct {
    TYPE* arr;
    int size;
    int capacity;
} STACK;

// Generic stack constructor.
// Retuns a premade stack object of type TYPE.
#define MAKEFUNCTION(T) TOKENPASTE(make_stack_, T)
STACK MAKEFUNCTION (TYPE) () {
    STACK stack;
    stack.capacity = CAPACITY;
    stack.size = 0;

    stack.arr = (TYPE*)dmalloc_array(CAPACITY);
    return stack;
}

// void push_TYPE (stack_TYPE*, TYPE);
// Generic push function.
// Appends an item of type TYPE to the given stack.
// Will expand the underlying array if needed.
#define PUSHFUNCTION(T) TOKENPASTE(push_, T)
void PUSHFUNCTION (TYPE) (STACK* stack, TYPE value) {
    if (stack->size == stack->capacity) {
        const int c = (CAPACITY / 2) + stack->capacity;
        TYPE* newArray = (TYPE*) dmalloc_array(c);
        for (int i = 0; i < stack->size; i++) {
            newArray[TYPECOEF * i] = stack->arr[TYPECOEF * i];
        }
        dmfree_array((block*)stack->arr, stack->capacity);
        stack->arr = newArray;
        stack->capacity = c;
    }

    stack->arr[TYPECOEF * stack->size] = value;
    ++stack->size;
}

// TYPE pop_TYPE (stack_TYPE*);
// Generic pop function.
// Returns the last element of the underling array and decrements
// will un-allocate some memory if size is small enough.
#define POPFUNCTION(T) TOKENPASTE(pop_, T)
TYPE POPFUNCTION (TYPE) (STACK* stack) {
    --stack->size;
    if (stack->capacity > CAPACITY && stack->size < stack->capacity / 2) {
        dmfree_array((block*)&stack->arr[TYPECOEF * (stack->capacity / 2)], stack->capacity / 2);
        stack->capacity = stack->capacity / 2;
    }
    return stack->arr[TYPECOEF * stack->size];
}

// Un-allocate a stack freeing up all memory currently used by it.
#define DELETESTACKFUNCTION(T) TOKENPASTE(delete_stack_, T)
void DELETESTACKFUNCTION (TYPE) (STACK* stack) {
    dmfree_array((block*)stack->arr, stack->capacity);
}

// Undefine all the macros so that they may be used again.
#undef TOKENPASTE
#undef MAKEFUNCTION
#undef PUSHFUNCTION
#undef POPFUNCTION
#undef DELETESTACKFUNCTION
#undef STACK

#endif
