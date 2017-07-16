/*
 * Pseudo Dynamic Memory Allocation Framework (d_memory for short)
 *
 * No external references were used in the making of this file!
 *
 * This framework was created to simulate dynamic memory allocation
 * for RobotC which does not support such a feature.
 *
 * The system works by created a static array of 8 byte blocks which
 * may hold any basic type from char to double, or any data type that
 * is smaller that 8 bytes. Along with the main memory array, which
 * is referred to as __d_memory, there is a separate static array of
 * the same number of elements which keeps track of whether a memory
 * location is in use or not.  This array is named  __free_memory.
 * The size of dynamically allocatable memory is determined by the
 * MEMORY_SIZE definition which is measured in blocks or number of
 * possible elements.  The amount of physical memory that is used is
 * 8 * MEMORY_SIZE.
 *
 * Problems:
 *      As each element is represented by 8 bytes once allocated,
 *      types that need more that 8 bytes of memory can not be
 *      stored. However, this limit may be changed by changing the
 *      type of block (this has not been tested).
 *
 *      Also since each element is represented by 8 bytes, this
 *      means that a lot of memory potentialy wasted depending
 *      on the element being stored.
 *          For an int,   4 bytes are wasted.
 *          For a char,   7 bytes are wasted.
 *          For a float,  4 bytes are wasted.
 *          For a double, 0 bytes are wasted.
 */

#ifndef __d_memory_h__
#define __d_memory_h__

typedef void* block; // An 8 byte chunk

typedef unsigned char inuse; // A 1 byte chunk
#define YES 1
#define NO  0

// Declares how large the dynamic memory array should be.
// Overwrite this size by defining MEMORY_SIZE before including this file.
#ifndef MEMORY_SIZE
#define MEMORY_SIZE 512 // in blocks
#endif

// Array declarations.
static block __d_memory [MEMORY_SIZE];
static inuse __free_memory [MEMORY_SIZE];

// Sets all blocks in __d_memory to NULL and all slots as free.
//
// Call once at the start of the program.
void initialize_memory () {
	for (int i = 0; i < MEMORY_SIZE; i++) {
		__d_memory[i] = NULL;
		__free_memory[i] = YES;
	}
}

// Called if unable to allocate more memory.
void __memory_error (const char* msg) {
    printf("Memory Error: %s", msg);
    fflush(stdout);
    // abort code here...
}

// Looks for a section of free blocks the size of numBlocks that is all free.
// If it is able to find a suitable section, it returns a pointer to the first element.
// If not, it calls __memory_error() and returns NULL
block* __find_free_chunk(int numBlocks) {
	int additionalBlocksNeeded = numBlocks - 1;
	for (int i = 0; i < MEMORY_SIZE; i++) {
		if (__free_memory[i] == YES) {
			// Check if blocks after are free as well.
            unsigned char isOK = YES;
			for (int j = i; j < i + additionalBlocksNeeded; j++) {
				if (__free_memory[j] == NO) {
					i = j;
                    isOK = NO;
					break;
				}
			}

            if (isOK == YES) {
                // All needed blocks are free...

                // Set blocks to be in use
                for (int j = i; j <= i + additionalBlocksNeeded; j++) {
                    __free_memory[j] = NO;
                }

                // Return pointer to the first block
                return &__d_memory[i];
            }
		}
	}

	// Unable to find a sutable chunck of memory...
	__memory_error("Unable to allocate memory");
	// Send error message or crash the program

	return NULL;
}

// Finds, allocates, and returns a pointer to, a single free block.
block* dmalloc () {
	return __find_free_chunk(1);
}

// Finds, allocates, and returns a pointer to the
// first element of a section of free blocks.
block* dmalloc_array (int size) {
	return __find_free_chunk(size);
}

// Sets the block that 'item' is pointing to to be not in use.
//
// Use to unallocate a single item.
void dmfree (block* item) {
	int index = (int)(item - __d_memory);
	__free_memory[index] = YES;
}

// Sets a section of blocks of size 'size' after and including
// the block that 'start' points to to be not in use.
//
// Use to unallocate an entire array.
void dmfree_array (block* start, int size) {
	int index = (int)(start - __d_memory);
	for (int i = 0; i < size; i++) {
		__free_memory[index + i] = YES;
	}
}

/* 	Array index coefficients.
 		As the size of a block is different from the size
 		of an int or float, using regular array notation
 		will not work as expected.  Therefore, when trying
 		to access an element, the index must be multiplied
 		by the type coefficient bellow.

 	Example:
 		to get element i from an in array use,
 		int x = array_int[INTCOEF * i]
 */
#define INTCOEF 	(sizeof(block) / sizeof(int))
#define CHARCOEF 	(sizeof(block) / sizeof(char))
#define FLOATCOEF	(sizeof(block) / sizeof(float))

// Returns the amount of memory used as a percent.
int amount_memory_used () {
    int slotsFilled = 0;
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (__free_memory[i] == NO) {
            ++slotsFilled;
        }
    }
    return (slotsFilled * 100 / MEMORY_SIZE);
}

// For testing...
//
// Prints out the entrity of __d_memory with the elements represented as ints.
void print_memory () {
	for (int i = 0; i < MEMORY_SIZE; i++) {
		printf("block[%d]:\t%d\t\tfree: %d\n",
                i,
                (int)__d_memory[i],
                (int)__free_memory[i]);
	}
    printf("Memory capacity: %d blocks\n", MEMORY_SIZE);
    printf("Memory in use:   %d%%\n", amount_memory_used());
}

#endif
