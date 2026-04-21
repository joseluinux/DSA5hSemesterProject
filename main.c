/*
 * main.c
 * Entry point: loads a labyrinth file and runs the backtracking solver.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Include/lab.h"

int main(void) {
    srand((unsigned)time(NULL));

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

    LinkedList *backpack = list_create();
    if (!backpack) {
        fprintf(stderr, "Error: could not allocate backpack.\n");
        return EXIT_FAILURE;
    }

    int found = find_exit(lab, rows, cols, start, &path, backpack);

    if (found) {
        long total = 0;
        for (ListNode *n = backpack->head; n; n = n->next)
            total += n->data;

        printf("\nPath found! Total treasure: %ld coins (%zu items).\n",
               total, list_size(backpack));
        printf("Saving solution to output.txt\n");
        save_solution("output.txt", lab, rows, cols);
    } else {
        printf("\nNo path to exit found.\n");
    }

    list_destroy(backpack);
    stack_free(&path);
    return EXIT_SUCCESS;
}
