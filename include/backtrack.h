/**
 * @file backtrack.h
 * @brief DFS backtracking search through the maze.
 *
 * backtrack_run() drives the full search, calling the renderer at each
 * step and triggering treasure/trap events on the backpack.
 */

#ifndef BACKTRACK_H
#define BACKTRACK_H

#include <maze.h>
#include <linked_list.h>

typedef enum {
    BACKTRACK_FIRST = 0,
    BACKTRACK_BEST
} BacktrackMode;

/**
 * @brief Run the maze solver from the player's starting position.
 *
 * @param maze Loaded maze; visited[] is mutated during search.
 * @param backpack Initialised backpack; modified by treasure and trap events.
 * @param mode BACKTRACK_FIRST (stop at first exit) or BACKTRACK_BEST (find optimal).
 * @param display DISPLAY_AUTO, DISPLAY_INTERACTIVE, or DISPLAY_NONE.
 * @return 1 if the exit was reached, 0 if no solution exists.
 */
int backtrack_run(Maze *maze, LinkedList *backpack, BacktrackMode mode, DisplayMode display);

#endif /* BACKTRACK_H */
