/**
 * @file defs.h
 * @brief Shared constants used across all modules.
 */

#ifndef DEFS_H
#define DEFS_H

#define CELL_WALL     '#'
#define CELL_CORRIDOR ' '
#define CELL_PLAYER   'P'
#define CELL_TREASURE 'T'
#define CELL_TRAP     'A'
#define CELL_EXIT     'S'

typedef enum {
    DISPLAY_AUTO = 0,   /* animated, 40 ms per step */
    DISPLAY_INTERACTIVE, /* step-by-step, waits for input */
    DISPLAY_NONE         /* silent, result only */
} DisplayMode;

#endif /* DEFS_H */
