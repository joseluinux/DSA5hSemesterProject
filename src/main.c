#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <maze.h>
#include <backtrack.h>
#include <pathfind.h>
#include <linked_list.h>
#include <renderer.h>
#ifdef WITH_GFX
#include <renderer_gfx.h>
#endif

static void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static int read_filepath(char *buf, int bufsize) {
    printf("Maze file path: ");
    fflush(stdout);
    if (!fgets(buf, bufsize, stdin)) return 0;
    buf[strcspn(buf, "\n")] = '\0';
    return buf[0] != '\0';
}

static int ask(const char *prompt, int lo, int hi) {
    int v = 0;
    printf("%s [%d-%d]: ", prompt, lo, hi);
    fflush(stdout);
    if (scanf("%d", &v) != 1 || v < lo || v > hi) {
        fprintf(stderr, "Invalid choice.\n");
        exit(1);
    }
    flush_stdin();
    return v;
}

int main(int argc, char *argv[]) {
    printf("=== MAZE SOLVER ===\n\n");

    char filepath[256];
    if (argc >= 2) {
        strncpy(filepath, argv[1], sizeof(filepath) - 1);
        filepath[sizeof(filepath) - 1] = '\0';
    } else {
        if (!read_filepath(filepath, sizeof(filepath))) {
            fprintf(stderr, "Error: no maze file provided.\n");
            return 1;
        }
    }

    /*
    Menu
    */
    printf("Algorithm:\n");
    printf("  1. Backtracking v1  (exhaustive DFS, branch-and-bound)\n");
    printf("  2. A* / Dijkstra v2 (A* for first path, Dijkstra for best path)\n");
    int algo = ask("Choice", 1, 2);

    printf("\nDisplay:\n");
    printf("  1. Terminal (ASCII)\n");
#ifdef WITH_GFX
    printf("  2. Graphical (raylib)\n");
    int display_type = ask("Choice", 1, 2);
#else
    int display_type = 1; /* graphical unavailable without WITH_GFX */
#endif

    printf("\nAnimation:\n");
    printf("  1. Interactive  (step-by-step, press Enter / SPACE)\n");
    printf("  2. Auto         (40 ms per step)\n");
    printf("  3. Silent       (result only)\n");
    int anim = ask("Choice", 1, 3);

    printf("\nPath mode:\n");
    printf("  1. First path found\n");
    printf("  2. Best path  (maximum treasure)\n");
    int path_mode = ask("Choice", 1, 2);

    DisplayMode display;
    switch (anim) {
        case 1: display = DISPLAY_INTERACTIVE; break;
        case 2: display = DISPLAY_AUTO;        break;
        default: display = DISPLAY_NONE;       break;
    }

    /* srand before maze_load: maze_assign_treasures uses rand() at load time. */
    srand((unsigned int)time(NULL));

    Maze *m = maze_load(filepath);
    if (!m) return 1;

    const char *algo_str = (algo == 2) ? "A*/Dijkstra v2" : "Backtracking v1";
    const char *disp_str = (display == DISPLAY_INTERACTIVE) ? "Interactive"
                         : (display == DISPLAY_AUTO)        ? "Auto (40 ms)"
                                                            : "Silent";
    const char *gfx_str  = (display_type == 2) ? "Graphical" : "Terminal";
    const char *mode_str = (path_mode == 2)    ? "Best path" : "First path";

    printf("\nAlgorithm : %s\n", algo_str);
    printf("Display   : %s / %s\n", gfx_str, disp_str);
    printf("Path mode : %s\n", mode_str);
    printf("Maze      : %s  (%d x %d)\n\n", filepath, m->cols, m->rows);

    if (display_type == 2) {
#ifdef WITH_GFX
        renderer_set_mode(1);
        renderer_gfx_init(m);
#else
        fprintf(stderr, "Graphical mode not available (rebuild with WITH_GFX=1).\n");
        maze_free(m);
        return 1;
#endif
    }

    LinkedList backpack;
    list_init(&backpack);

    int found;
    if (algo == 2) {
        PathfindMode pf = (path_mode == 2) ? PATHFIND_BEST : PATHFIND_FIRST;
        found = pathfind_run(m, &backpack, pf, display);
    } else {
        BacktrackMode bt = (path_mode == 2) ? BACKTRACK_BEST : BACKTRACK_FIRST;
        found = backtrack_run(m, &backpack, bt, display);
    }

    if (found) {
        int total = 0;
        for (Node *n = backpack.head; n; n = (*n).next) total += (*n).value;

        printf("\n=== EXIT REACHED ===\n");
        printf("Total treasure value: %d coins\n", total);
        printf("Backpack: ");
        list_print(&backpack);
    } else {
        printf("\n=== NO SOLUTION ===\n");
        printf("The maze has no path from P to S.\n");
    }

    list_free(&backpack);
    maze_free(m);
    return 0;
}
