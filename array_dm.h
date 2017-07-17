#include "dmemory.h"

#ifdef TYPE

#ifndef CAPACITY_ARRAY
#define CAPACITY_ARRAY 8
#endif

#define TOKENPASTE(x, y) x ## y

#define TYPECOEF (sizeof(block) / sizeof(TYPE))
#define ITERATOR TYPECOEF * i

// Generic struct declaration.
#define TEMPLATEARRAY(T) TOKENPASTE(array_, T)
#define ARRAY TEMPLATEARRAY (TYPE)
typedef struct {
	TYPE* start;
	int size;
	int capacity;
} ARRAY;

// Generic array constructor.
#define MAKEFUNCTION(T) TOKENPASTE(make_array_, T)
ARRAY MAKEFUNCTION (TYPE) () {
	ARRAY array;
	array.capacity = CAPACITY;
	array.size = 0;
	return array;
}

// Generic append function.
// Adds an element to the end of the array 
// resizing the underlying array if needed.
#define APPENDFUNCTION(T) TOKENPASTE(append_, T)
void APPENDFUNCTION (TYPE) (ARRAY* array, TYPE elem) {
	if (array->size == array->capacity) {
		const int c = (CAPACITY / 2) + array->capacity;
		TYPE* newArray = (TYPE*)dmalloc_array(c);
		for (int i = 0; i < array->size; i++) {
			newArray[ITERATOR] = array->start[ITERATOR];
		}
		dmfree_array(array->start, array->capacity);
		array->start = newArray;
		array->capacity = c;
	}
}

// Generic at function.
// Returns the element of array at index idx.
#define ATFUNCTION(T) TOKENPASTE(at_, T)
TYPE ATFUNCTION (TYPE) (ARRAY* array, int idx) {
	return array->start[TYPECOEF * idx];
}

// Genecric remove_last function.
// Removes and returns the last element of the array.
#define REMOVELAST(T) TOKENPASTE(remove_last_, T)
TYPE REMOVELAST (TYPE) (ARRAY* array) {
	--array->size;
	return array->start[TYPECOEF * array->size];
}

// Generic remove_at function.
// Removes and returns the element at index idx.
// Elements after are moved back to close the gap.
#define REMOVEAT(T) TOKENPASTE(remove_at_, T)
TYPE REMOVEAT (TYPE) (ARRAY* array, int idx) {
	TYPE item = ATFUNCTION(TYPE)(array, idx);
	for (int i = idx + 1; i < array->size; i++) {
		array->start[TYPECOEF * (i - 1)] = array->start[ITERATOR];
	}
	--array->size;
}

// Un-allocates the array.
#define DELETEARRAYFUNCTION(T) TOKENPASTE(delete_array_, T)
void DELETEARRAYFUNCTION (TYPE) (ARRAY* array) {
	dmfree_array((block*)array->start, array->capacity);
}


#undef TOKENPASTE
#undef MAKEFUNCTION
#undef APPENDFUNCTION
#undef ATFUNCTION
#undef REMOVELAST
#undef REMOVEAT
#undef DELETEARRAYFUNCTION
#undef ARRAY
#undef TEMPLATEARRAY
#endif