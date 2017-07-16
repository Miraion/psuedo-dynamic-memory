/*
 * Pseudo Dynamic Memory Allocation Framework (d_memory for short)
 *
 * No external references were used in the making of this file!
 *
 * Description:
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
 *
 *      As the size of the block the element is stored in and the
 *      actual size of the element differ, standard array notation
 *      may not work as expected.  To combat this problem a
 *      coefficient must be multiplied to the desired index to get
 *      the correct location.
 *          Example:
 *              to get the 5th element of an array of ints stored
 *              in dynamic memory use this:
 *                  int_array[INTCOEFF * 5] = 32;
 *      for custom types or types that do not have a predefined
 *      coefficient, the formula is:
 *          sizeof(block) / sizeof(TYPE)
 *                      or
 *                8 / sizeof(TYPE)
 *
 * Update:
 *      __free_memory array size reduced by a factor of 8.
 *
 *      Instead of using a whole byte to store whether a location
 *      is in use, a single bit is now being used.  The states of
 *      8 locations are stored as a single byte and, with the use
 *      of bitwise operators, we are able to determine and set the
 *      states of individual bits in that set.
 */

#ifndef __d_memory_h__
#define __d_memory_h__

// An 8 byte chunk
typedef void* block;
// A 1 byte chunk
typedef unsigned char byte;

// Declares how large the dynamic memory array should be.
// Overwrite this size by defining MEMORY_SIZE before including this file.
#ifndef MEMORY_SIZE
#define MEMORY_SIZE 512 // in blocks
#endif

// Array declarations.
static block __d_memory [MEMORY_SIZE];
static byte __free_memory [MEMORY_SIZE / 8];

// Custom boolean declaration as to not conflict with RobotC'c bool type.
typedef unsigned char __bool;
#define YES 1
#define NO  0

#define EMPTY 0

// Array coefficients, see 'Problems' for use.
#define INTCOEF 	(sizeof(block) / sizeof(int))
#define CHARCOEF 	(sizeof(block) / sizeof(char))
#define FLOATCOEF	(sizeof(block) / sizeof(float))

/*
 * Returns an isolated bit used in the functions below to determine the states
 * of bits in the __free_memory array.
 *
 * Using bitwise left shift operator, returns an byte with a single bit set to
 * 1 depending on where the given index lies in a set of 8 locations.
 *
 *  Example:
 *      idx % 8 = 0
 *          returns bx00000001
 *
 *      idx % 8 = 1
 *          returns bx00000010
 *
 *          .
 *          .
 *          .
 *
 *      idx % 8 = 7
 *          returns bx10000000
 */
byte __bit_encoder (int idx) {
    return (byte)(1 << (idx % 8));
}

/*
 * Sets location idx to be in use.
 *
 *      Example, idx = 9:
 *      before:
 *          __free_memory[1] = bx00000000
 *                                      ^
 *                                    idx 8
 *      after:
 *          __free_memory[1] = bx00000010
 *
 *      Sets desired bit to 1 via bitwise OR
 *            v set this to 1
 *      bx00100100
 *
 *      bx00100100
 *    | bx00001000 < isolated bit
 *    = bx00101100
 *
 *    Result is original with isolated bit set to 1
 */
void __set_block_used (int idx) {
    int idx_converted = idx / 8;
    __free_memory[idx_converted] = __free_memory[idx_converted] | __bit_encoder(idx);
}

/*
 * Sets location idx to be free..
 *
 *      Example, idx = 9:
 *      before:
 *          __free_memory[1] = bx00000010
 *                                      ^
 *                                    idx 8
 *      after:
 *          __free_memory[1] = bx00000000
 *
 *      Sets desired bit to 0 via bitwise AND and NOT
 *            v set this to 0
 *      bx01101011
 *
 *      On isolated bit perform NOT operation
 *    ~ bx00001000
 *    = bx11110111
 *
 *      AND original and NOT isolated
 *      bx01101011
 *    & bx11110111
 *    = bx01100011
 *
 *      Result is original with isolated bit set to 0
 */
void __set_block_free (int idx) {
    int idx_helper = idx / 8;
    __free_memory[idx_helper] = __free_memory[idx_helper] & (~__bit_encoder(idx));
}

/*
 * Checks whether a location is in use.
 *
 * Checks if desired bit is set to 1 using bitwise AND
 *
 *  Example 1:
 *             v check this bit
 *      bx00110100
 *
 *      bx00110100
 *    & bx00000100 < isolated bit
 *    = bx00000100
 *
 *      bx00000100 = 4 which passes ternary operator ?: so we return 0
 *                   as this space is currently in use.
 *
 *  Example 2:
 *             v check this bit
 *      bx11001000
 *
 *      bx11001000
 *    & bx00000100 < isolated bit
 *    = bx00000000
 *
 *      bx00000000 = 0 which fails the ternary operation so we return 1
 *                   as this space is free.
 */
__bool __check_block_free (int idx) {
    int idx_helper = idx / 8;
    return __free_memory[idx_helper] & __bit_encoder(idx) ? (__bool)0:(__bool)1;
}

// Sets all blocks in __d_memory to NULL and all slots as free.
//
// Call once at the start of the program.
void initialize_memory () {
	for (int i = 0; i < MEMORY_SIZE; i++) {
		__d_memory[i] = NULL;
        if (i < MEMORY_SIZE / 8) {
            __free_memory[i] = EMPTY;
        }
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
		if (__check_block_free(i)) {
			// Check if blocks after are free as well.
            __bool isOK = YES;
			for (int j = i; j < i + additionalBlocksNeeded; j++) {
				if (!__check_block_free(j)) {
					i = j;
                    isOK = NO;
					break;
				}
			}

            if (isOK == YES) {
                // All needed blocks are free...

                // Set blocks to be in use
                for (int j = i; j <= i + additionalBlocksNeeded; j++) {
                    __set_block_used(j);
                }

                // Return pointer to the first block
                return &__d_memory[i];
            }
		}
	}

	// Unable to find a suitable chunk of memory...
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
// Use to un-allocate a single item.
void dmfree (block* item) {
	int index = (int)(item - __d_memory);
    __set_block_free(index);
}

// Sets a section of blocks of size 'size' after and including
// the block that 'start' points to to be not in use.
//
// Use to un-allocate an entire array.
void dmfree_array (block* start, int size) {
	int index = (int)(start - __d_memory);
	for (int i = index; i < size + index; i++) {
        __set_block_free(i);
	}
}

// Returns the amount of memory used as a percent.
int amount_memory_used () {
    int slotsFilled = 0;
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (!__check_block_free(i)) {
            ++slotsFilled;
        }
    }
    return (slotsFilled * 100 / MEMORY_SIZE);
}

// For testing...
//
// Prints out the entirety of __d_memory with the elements represented as ints.
void print_memory () {
	for (int i = 0; i < MEMORY_SIZE; i++) {
		printf("block[%d]:\t0x%x\t\tfree: %d\n",
                i,
                (int)__d_memory[i],
                (__check_block_free(i) != 0 ? 1:0));
	}
    printf("Memory capacity: %d blocks\n", MEMORY_SIZE);
    printf("Memory in use:   %d%%\n", amount_memory_used());
}

#endif
