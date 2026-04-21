/*
 * lab.c
 * Labyrinth loader and stack-based backtracking solver.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lab.h"

/* Direction offsets: up, right, down, left */
static const int dr[] = { -1, 0, 1, 0 };
static const int dc[] = {  0, 1, 0,-1 };

/* Prints "Backpack (n=.., total=..): [v1, v2, ...]" or "(empty)". */
static void print_backpack(const LinkedList *bp) {
    size_t n = list_size(bp);
    if (n == 0) {
        printf("Backpack (empty)\n");
        return;
    }
    long total = 0;
    for (ListNode *node = bp->head; node; node = node->next)
        total += node->data;

    printf("Backpack (n=%zu, total=%ld coins): [", n, total);
    for (ListNode *node = bp->head; node; node = node->next) {
        printf("%d", node->data);
        if (node->next) printf(", ");
    }
    printf("]\n");
}

/*
 * Applies the effect of the cell's original symbol BEFORE it gets marked
 * VISITED. Returns the number of characters written to `msg` (bounded by
 * msg_size), so the caller can log what happened this step.
 */
static void apply_cell_effect(char cell, LinkedList *backpack,
                              char *msg, size_t msg_size) {
    if (cell == TREASURE) {
        int value = TREASURE_MIN_VALUE +
                    rand() % (TREASURE_MAX_VALUE - TREASURE_MIN_VALUE + 1);
        list_insert_ordered(backpack, value);
        snprintf(msg, msg_size, "Found treasure worth %d coins!", value);
    } else if (cell == TRAP) {
        if (!list_is_empty(backpack)) {
            int lost = list_pop_front(backpack);
            snprintf(msg, msg_size,
                     "TRAP! Lost treasure worth %d coins.", lost);
        } else {
            snprintf(msg, msg_size, "TRAP! Backpack was empty.");
        }
    } else {
        msg[0] = '\0';
    }
}

int load_labyrinth(const char *filename, char lab[], int *rows, int *cols,
                   Position *start, Position *exit_pos) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;

    char line[MAX_COLS + 2]; /* +2 for '\n' and '\0' */
    *rows = 0;
    *cols = 0;

    while (fgets(line, sizeof(line), fp) && *rows < MAX_ROWS) {
        int len = (int)strlen(line);
        /* strip trailing newline / carriage return */
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            len--;

        if (*rows == 0)
            *cols = len;

        for (int c = 0; c < len; c++) {
            char ch = line[c];
            lab[(*rows) * (*cols) + c] = ch;

            if (ch == START) {
                start->row = *rows;
                start->col = c;
            } else if (ch == EXIT_S) {
                exit_pos->row = *rows;
                exit_pos->col = c;
            }
        }
        (*rows)++;
    }

    fclose(fp);
    return 1;
}

void show_labyrinth(const char lab[], int rows, int cols) {
    /* move cursor to top-left to overwrite previous frame */
    printf("\033[H\033[J");
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++)
            putchar(lab[r * cols + c]);
        putchar('\n');
    }
}

void save_solution(const char *filename, const char lab[], int rows, int cols) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "save_solution: could not open '%s'\n", filename);
        return;
    }
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++)
            fputc(lab[r * cols + c], fp);
        fputc('\n', fp);
    }
    fclose(fp);
}

int is_valid_position(int r, int c, const char lab[], int rows, int cols) {
    if (r < 0 || r >= rows || c < 0 || c >= cols)
        return 0;
    char cell = lab[r * cols + c];
    return cell != WALL && cell != VISITED;
}

/*
 * Stack-based iterative backtracking.
 *
 * Uses a second parallel array (dir[]) to track which direction
 * was last tried at each depth level, so we can resume trying
 * the next direction after backtracking.
 *
 * Directions: 0=up, 1=right, 2=down, 3=left
 */
int find_exit(char lab[], int rows, int cols,
              Position start, Stack *path, LinkedList *backpack) {
    /* dir[i] = next direction to try at depth i */
    int dir[MAX_STACK];
    memset(dir, 0, sizeof(dir));

    /* push starting position (starting cell is 'P', no effect to apply) */
    lab[start.row * cols + start.col] = VISITED;
    stack_push(path, start);
    dir[0] = 0;

    show_labyrinth(lab, rows, cols);
    print_backpack(backpack);
    printf("Start at (%d, %d)\n", start.row, start.col);
    wait_for_enter();

    while (!stack_is_empty(path)) {
        Position cur = stack_peek(path);
        int depth = path->top;
        int found_next = 0;

        /* try remaining directions from where we left off */
        while (dir[depth] < 4) {
            int nr = cur.row + dr[dir[depth]];
            int nc = cur.col + dc[dir[depth]];
            dir[depth]++;

            if (!is_valid_position(nr, nc, lab, rows, cols))
                continue;

            /* check if we reached the exit */
            if (lab[nr * cols + nc] == EXIT_S) {
                Position exit_pos = { nr, nc };
                stack_push(path, exit_pos);
                lab[nr * cols + nc] = VISITED;
                show_labyrinth(lab, rows, cols);
                print_backpack(backpack);
                printf("Exit found at (%d, %d)!\n", nr, nc);
                return 1;
            }

            /* apply treasure/trap effect based on the original cell symbol */
            char event_msg[64];
            apply_cell_effect(lab[nr * cols + nc], backpack,
                              event_msg, sizeof(event_msg));

            /* move to the new cell */
            Position next = { nr, nc };
            lab[nr * cols + nc] = VISITED;
            stack_push(path, next);
            dir[path->top] = 0; /* reset direction counter for new cell */

            show_labyrinth(lab, rows, cols);
            print_backpack(backpack);
            printf("Moved to (%d, %d)\n", nr, nc);
            if (event_msg[0]) printf("%s\n", event_msg);
            wait_for_enter();

            found_next = 1;
            break;
        }

        /* no valid direction found — backtrack */
        if (!found_next) {
            Position dead = stack_pop(path);
            lab[dead.row * cols + dead.col] = PATH;

            show_labyrinth(lab, rows, cols);
            print_backpack(backpack);
            printf("Backtracking from (%d, %d)\n", dead.row, dead.col);
            wait_for_enter();
        }
    }

    return 0; /* no path found */
}

void wait_for_enter(void) {
    printf("Press ENTER to continue...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}
