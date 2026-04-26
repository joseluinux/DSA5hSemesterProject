/*
 * stack.h
 * Stack data structure definition and prototypes.
 * Used by the backtracking algorithm to store the path through the labyrinth.
 */

#ifndef STACK_H
#define STACK_H

#include <stdio.h>


#define MAX_ROWS  40
#define MAX_COLS  40
#define MAX_STACK (MAX_ROWS * MAX_COLS)  /* maximum 1600 positions */


/* Position: row/column coordinates of a cell in the labyrinth. */
typedef struct {
    int row;
    int col;
} Position;

/* Stack: static array-backed stack of positions. top == -1 means empty. */
typedef struct {
    int      top;
    Position items[MAX_STACK];
} Stack;

/* --- Prototypes --------------------------------------------------------- */

/* Initializes the stack (top = -1). Must be called before any other use. */
void stack_init(Stack *s);

/* Returns 1 if the stack is empty, 0 otherwise. */
int stack_is_empty(Stack *s);

/* Returns 1 if the stack is full, 0 otherwise. */
int stack_is_full(Stack *s);

/*
 * Pushes pos onto the stack.
 * Returns 1 on success, 0 on overflow (stack is full).
 */
int stack_push(Stack *s, Position pos);

/*
 * Pops and returns the top position.
 * If the stack is empty, prints an error and returns {-1, -1} as a sentinel.
 * Caller must check whether row == -1 before using the result.
 */
Position stack_pop(Stack *s);

/*
 * Returns the top position WITHOUT removing it (peek).
 * If the stack is empty, prints an error and returns {-1, -1} as a sentinel.
 */
Position stack_peek(Stack *s);

/*
 * Resets the stack (top = -1).
 * Kept for API symmetry; no heap memory is allocated in this implementation.
 */
void stack_free(Stack *s);

#endif /* STACK_H */
