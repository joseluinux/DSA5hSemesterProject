/*
 * lab.c
 * Labyrinth loader and stack-based backtracking solver.
 */

#include <stdio.h>
#include <string.h>
#include "lab.h"

/* Direction offsets: up, right, down, left */
static const int dr[] = { -1, 0, 1, 0 };
static const int dc[] = {  0, 1, 0,-1 };

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
              Position start, Stack *path) {
    /* dir[i] = next direction to try at depth i */
    int dir[MAX_STACK];
    memset(dir, 0, sizeof(dir));

    /* push starting position */
    lab[start.row * cols + start.col] = VISITED;
    stack_push(path, start);
    dir[0] = 0;

    show_labyrinth(lab, rows, cols);
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
                printf("Exit found at (%d, %d)!\n", nr, nc);
                return 1;
            }

            /* move to the new cell */
            Position next = { nr, nc };
            lab[nr * cols + nc] = VISITED;
            stack_push(path, next);
            dir[path->top] = 0; /* reset direction counter for new cell */

            show_labyrinth(lab, rows, cols);
            printf("Moved to (%d, %d)\n", nr, nc);
            wait_for_enter();

            found_next = 1;
            break;
        }

        /* no valid direction found — backtrack */
        if (!found_next) {
            Position dead = stack_pop(path);
            lab[dead.row * cols + dead.col] = PATH;

            show_labyrinth(lab, rows, cols);
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
