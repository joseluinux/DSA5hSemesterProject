/*
 * main.c
 * Entry point: loads a labyrinth file and runs the backtracking solver.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Include/lab.h"

int main() {
    char filename[256];

    printf("Enter labyrinth file path: ");
    if (!fgets(filename, sizeof(filename), stdin)) {
        fprintf(stderr, "Error reading input.\n");
        return EXIT_FAILURE;
    }
    /* strip newline */
    filename[strcspn(filename, "\n")] = '\0';

    char lab[MAX_ROWS * MAX_COLS];
    int rows, cols;
    Position start, exit_pos;

    if (!load_labyrinth(filename, lab, &rows, &cols, &start, &exit_pos)) {
        fprintf(stderr, "Error: could not open '%s'\n", filename);
        return EXIT_FAILURE;
    }

    printf("Labyrinth loaded: %d x %d\n", rows, cols);
    printf("Start: (%d, %d)  Exit: (%d, %d)\n",
           start.row, start.col, exit_pos.row, exit_pos.col);
    wait_for_enter();

    Stack path;
    stack_init(&path);

    if (find_exit(lab, rows, cols, start, &path)) {
        printf("\nPath found! Saving to output.txt\n");
        save_solution("output.txt", lab, rows, cols);
    } else {
        printf("\nNo path to exit found.\n");
    }

    stack_free(&path);
    return EXIT_SUCCESS;
}
