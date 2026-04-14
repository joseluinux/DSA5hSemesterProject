/*
 * stack.c
 * Implementation of the Stack data structure.
 */

#include <stdio.h>
#include "stack.h"

/* Initializes the stack by setting top to -1 (empty). */
void stack_init(Stack *s) {
    s->top = -1;
}

/* Returns 1 if the stack has no elements, 0 otherwise. */
int stack_is_empty(Stack *s) {
    return s->top == -1;
}

/* Returns 1 if the stack has reached its maximum capacity, 0 otherwise. */
int stack_is_full(Stack *s) {
    return s->top == MAX_STACK - 1;
}

/*
 * Pushes pos onto the stack.
 * Returns 1 on success, 0 if the stack is already full (overflow guard).
 */
int stack_push(Stack *s, Position pos) {
    if (stack_is_full(s)) {
        fprintf(stderr, "stack_push: stack overflow.\n");
        return 0;
    }
    s->items[++(s->top)] = pos;
    return 1;
}

/*
 * Removes and returns the top position.
 * Returns {-1, -1} as a sentinel if the stack is empty.
 */
Position stack_pop(Stack *s) {
    Position sentinel = {-1, -1};
    if (stack_is_empty(s)) {
        fprintf(stderr, "stack_pop: stack is empty.\n");
        return sentinel;
    }
    return s->items[(s->top)--];
}

/*
 * Returns the top position without removing it.
 * Returns {-1, -1} as a sentinel if the stack is empty.
 */
Position stack_peek(Stack *s) {
    Position sentinel = {-1, -1};
    if (stack_is_empty(s)) {
        fprintf(stderr, "stack_peek: stack is empty.\n");
        return sentinel;
    }
    return s->items[s->top];
}

/* Resets the stack to its initial empty state. */
void stack_free(Stack *s) {
    s->top = -1;
}
