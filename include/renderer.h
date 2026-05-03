#ifndef RENDERER_H
#define RENDERER_H

#include <maze.h>
#include <linked_list.h>
#include <stack.h>

/* 0 = terminal (default), 1 = graphical (raylib, requires WITH_GFX build). */
void renderer_set_mode(int graphical);

/* Set the per-step pause in microseconds; 0 disables sleeping. */
void renderer_set_delay(unsigned int delay_us);

/* Redraw the maze at current_pos with path trail.  Dispatches to graphical
 * renderer when graphical mode is active, otherwise uses ANSI terminal. */
void renderer_draw(const Maze *m, int current_pos,
                   const LinkedList *backpack, const Stack *path);

/* Block until the user advances one step.
 * Terminal mode: waits for Enter.  Graphical mode: waits for SPACE/ENTER. */
void renderer_wait_interactive(void);

/* Print solution to stdout, write solution.txt, and (graphical mode only)
 * show the final state in the window and wait for the user to close it. */
void renderer_finalize(const Stack *path, const Maze *m, const LinkedList *backpack);

void renderer_print_solution(const Stack *path, const Maze *m);
void renderer_write_solution(const Stack *path, const Maze *m, const LinkedList *backpack);

#endif /* RENDERER_H */
