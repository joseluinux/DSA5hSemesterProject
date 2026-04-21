/*
 * lab.h
 * Labyrinth loader and stack-based backtracking solver.
 */

#ifndef LAB_H
#define LAB_H

#include "stack.h"

/* Labyrinth cell symbols */
#define WALL    '#'
#define PATH    ' '
#define START   'P'
#define EXIT_S  'S'
#define VISITED '.'

/*
 * Loads a labyrinth from a text file into a 1D array (row-major order).
 * Sets *rows, *cols, *start and *exit_pos.
 * Returns 1 on success, 0 on failure.
 */
int load_labyrinth(const char *filename, char lab[], int *rows, int *cols,
                   Position *start, Position *exit_pos);

/*
 * Prints the labyrinth to stdout.
 */
void show_labyrinth(const char lab[], int rows, int cols);

/*
 * Saves the solved labyrinth state to a file.
 */
void save_solution(const char *filename, const char lab[], int rows, int cols);

/*
 * Returns 1 if (r, c) is inside bounds and is a walkable cell.
 */
int is_valid_position(int r, int c, const char lab[], int rows, int cols);

/*
 * Iterative backtracking using a Stack.
 * Finds a path from start to exit.
 * Prints each move to stdout and waits for ENTER.
 * Returns 1 if exit found, 0 otherwise.
 */
int find_exit(char lab[], int rows, int cols,
              Position start, Stack *path);

/*
 * Waits for user to press ENTER.
 */
void wait_for_enter(void);

#endif /* LAB_H */
