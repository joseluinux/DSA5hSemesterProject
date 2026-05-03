#ifndef PATHFIND_H
#define PATHFIND_H

#include <maze.h>
#include <linked_list.h>
#include <defs.h>

typedef enum {
    PATHFIND_FIRST = 0,  /* shortest path to exit (A*) */
    PATHFIND_BEST        /* maximum raw-treasure path (Dijkstra) */
} PathfindMode;

/* Run the v2 solver.  Fills backpack with items collected along the found
 * path and returns 1 on success, 0 if the exit is unreachable. */
int pathfind_run(Maze *maze, LinkedList *backpack, PathfindMode mode, DisplayMode display);

#endif /* PATHFIND_H */
