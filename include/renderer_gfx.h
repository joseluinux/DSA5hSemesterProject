#ifndef RENDERER_GFX_H
#define RENDERER_GFX_H

#include <maze.h>
#include <linked_list.h>
#include <stack.h>

/* Open a window sized to the maze.  Must be called before any draw/wait. */
void renderer_gfx_init(const Maze *m);

/* Draw one frame: maze grid + backpack sidebar. */
void renderer_gfx_draw(const Maze *m, int current_pos,
                        const LinkedList *backpack, const Stack *path);

/* Wait for SPACE or ENTER in the window (interactive step mode). */
void renderer_gfx_wait_interactive(void);

/* Process window events for @p seconds (auto animation delay). */
void renderer_gfx_wait_auto(double seconds);

/* Redraw the last frame in a loop until the user closes the window,
 * then close it.  Called after the solver finishes. */
void renderer_gfx_show_and_close(void);

/* Close the window immediately (called when user closes mid-search). */
void renderer_gfx_force_close(void);

/* Return 1 if the user has requested window close. */
int renderer_gfx_should_close(void);

#endif /* RENDERER_GFX_H */
