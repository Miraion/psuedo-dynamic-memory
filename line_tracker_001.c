#include <stdio.h>

//===---- Point ----===//
// Point definition
typedef struct {
    int x;
    int y;
} point;

// point object constructor
point make_point (int x, int y) {
    point p;
    p.x = x;
    p.y = y;
    return p;
}

// Adds two point objects
point add (point lhs, point rhs) {
    return make_point(lhs.x + rhs.x, lhs.y + rhs.y);
}

// Scalar multiplication of a int and point
point multiply (int lhs, point rhs) {
    return make_point(lhs * rhs.x, lhs * rhs.y);
}


//===---- Direction ----===//
typedef int direction;

#define NORTH 	0
#define EAST 	1
#define SOUTH	2
#define WEST	3

// Heading turn to right
void increment_right (direction* heading) {
    ++*heading;
    if (*heading > WEST) {
        *heading = NORTH;
    }
}

// Heading turn to left
void increment_left (direction* heading) {
    --*heading;
    if (*heading < NORTH) {
        *heading = WEST;
    }
}

// Converts a direction to a point coefficient
point directional_coefficient (direction heading) {
    switch (heading) {
        case NORTH: return make_point(0, 1);
        case EAST:  return make_point(1, 0);
        case SOUTH: return make_point(0,-1);
        case WEST:  return make_point(-1,0);
        default:    return make_point(0, 0);
    }
}


//===---- Location Tracking ----===//
#define START_POINT make_point(0, 0);
#define START_DIRECTION NORTH;

typedef struct {
    point position;
    direction heading;
} location;

// Constructs a location object using START_POINT and START_DIRECTION
location default_location () {
    location l;
    l.position = START_POINT;
    l.heading = START_DIRECTION;
    return l;
}

// Changes position by adding distance * directional_coefficient(heading)
// to the currentPos
void update_location (location* currentLoc, direction heading, int distance) {
    point p = multiply(distance, directional_coefficient(heading));
    currentLoc->position = add(currentLoc->position, p);
    currentLoc->heading = heading;
}

void increment_location (location* currentLoc, direction heading) {
    update_location(currentLoc, heading, 1);
}






//===---- Testing ----===//

//#define MEMORY_SIZE 64
#include "dmemory.h"

#define TYPE int
#include "stack_dm.h"
#include "array_dm.h"
#undef TYPE

#define TYPE point
#include "stack_dm.h"
#undef TYPE

int main () {
    printf("d_memory size:     %d bytes\n", (int)sizeof(__d_memory));
	printf("free check size:   %d bytes\n", (int)sizeof(__free_memory));
	printf("total memory used: %d bytes\n\n",
            (int)sizeof(__d_memory) + (int)sizeof(__free_memory));

    initialize_memory();

    stack_int i_stack = make_stack_int();

    for (int i = 1; i <= 15; i++) {
        push_int(&i_stack, i);
    }

//    while (i_stack.size > 0) {
//        printf("popped: %d\n", pop_int(&i_stack));
//    }

//    delete_stack_int(&i_stack);

//    printf("\n");

    stack_point p_stack = make_stack_point();
    push_point(&p_stack, make_point(5, 1));
    push_point(&p_stack, make_point(3415, 25));
    point p = pop_point(&p_stack);

//    printf("(%d, %d)\n\n", p.x, p.y);
    printf("Memory capacity: %d blocks\n", MEMORY_SIZE);
    printf("Memory in use:   %d%%\n", amount_memory_used());

//	print_memory();
}
