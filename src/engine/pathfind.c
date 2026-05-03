#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <pathfind.h>
#include <heap.h>
#include <stack.h>
#include <renderer.h>
#include <defs.h>

/* Same column-wrap guard used by backtrack.c and maze_compute_reachability. */
static int is_wrap(int pos, int d, int cols) {
    if (d == 2 && pos % cols == 0)        return 1;
    if (d == 3 && pos % cols == cols - 1) return 1;
    return 0;
}

static int manhattan(int a, int b, int cols) {
    int dr = a / cols - b / cols;
    int dc = a % cols - b % cols;
    if (dr < 0) dr = -dr;
    if (dc < 0) dc = -dc;
    return dr + dc;
}

/*
Trail: parent-chain path used for live visualization
*/
static void build_trail(Stack *trail, const int *parent, int pos) {
    /* Reset without freeing — reuse the existing heap allocation. */
    (*trail).top = -1;

    int len = 0, p = pos;
    while (p != -1) { len++; p = parent[p]; }

    int *tmp = malloc(len * sizeof(int));
    if (!tmp) return;
    p = pos;
    for (int i = len - 1; i >= 0; i--) { tmp[i] = p; p = parent[p]; }
    for (int i = 0; i < len; i++) stack_push(trail, tmp[i]);
    free(tmp);
}

/*
Path reconstruction + backpack simulation
*/
static void reconstruct_path(const int *parent, int goal, Stack *path) {
    int n = 0, p = goal;
    while (p != -1) { n++; p = parent[p]; }

    int *tmp = malloc(n * sizeof(int));
    if (!tmp) { fprintf(stderr, "malloc failed\n"); exit(1); }

    p = goal;
    for (int i = n - 1; i >= 0; i--) {
        tmp[i] = p;
        p = parent[p];
    }
    for (int i = 0; i < n; i++)
        stack_push(path, tmp[i]);
    free(tmp);
}

static void simulate_backpack(const Stack *path, const Maze *maze, LinkedList *backpack) {
    for (int i = 0; i <= (*path).top; i++) {
        int p = (*path).data[i];
        if (maze_cell(maze, p) == CELL_TREASURE)
            list_insert(backpack, (*maze).treasure_values[p]);
        else if (maze_cell(maze, p) == CELL_TRAP)
            list_remove_head(backpack);
    }
}

/*
A* — shortest path (PATHFIND_FIRST)
*/
static int run_astar(Maze *maze, LinkedList *backpack, Stack *path, DisplayMode display) {
    int n    = (*maze).rows * (*maze).cols;
    int cols = (*maze).cols;
    int goal = (*maze).exit_pos;
    int offsets[4] = {-cols, +cols, -1, +1};

    int  *dist   = malloc(n * sizeof(int));
    int  *parent = malloc(n * sizeof(int));
    char *closed = calloc(n, 1);
    if (!dist || !parent || !closed) {
        fprintf(stderr, "malloc failed\n");
        free(dist); free(parent); free(closed);
        return 0;
    }
    for (int i = 0; i < n; i++) { dist[i] = INT_MAX; parent[i] = -1; }

    Stack trail;
    stack_init(&trail);

    Heap h;
    heap_init(&h);

    int start = (*maze).player_pos;
    dist[start] = 0;
    HeapNode sn = {start, manhattan(start, goal, cols), 0, -1};
    heap_push(&h, sn);

    int result = 0;

    while (!heap_is_empty(&h)) {
        HeapNode cur = heap_pop(&h);
        if (closed[cur.pos]) continue;
        closed[cur.pos] = 1;
        parent[cur.pos] = cur.parent;

        if (display != DISPLAY_NONE) {
            build_trail(&trail, parent, cur.pos);
            renderer_draw(maze, cur.pos, backpack, &trail);
            if (display == DISPLAY_INTERACTIVE)
                renderer_wait_interactive();
        }

        if (cur.pos == goal) {
            reconstruct_path(parent, goal, path);
            simulate_backpack(path, maze, backpack);
            renderer_finalize(path, maze, backpack);
            result = 1;
            break;
        }

        for (int d = 0; d < 4; d++) {
            if (is_wrap(cur.pos, d, cols)) continue;
            int next = cur.pos + offsets[d];
            if (next < 0 || next >= n)             continue;
            if ((*maze).cells[next] == CELL_WALL)  continue;
            if (closed[next])                      continue;
            if (!(*maze).reachable[next])          continue;

            int new_g = cur.g + 1;
            if (new_g < dist[next]) {
                dist[next] = new_g;
                HeapNode nn = {next, new_g + manhattan(next, goal, cols), new_g, cur.pos};
                heap_push(&h, nn);
            }
        }
    }

    stack_free(&trail);
    heap_free(&h);
    free(dist); free(parent); free(closed);
    return result;
}

/*
Dijkstra — maximum raw-treasure path (PATHFIND_BEST)

Note: Dijkstra for maximisation is a polynomial-time heuristic.  It finds the
path with the highest sum of treasure-cell values, without accounting for how
trap-cell losses interact with exact backpack state.  The backpack is simulated
exactly only on the final chosen path.  On treasure-dense mazes with few traps
the result is typically optimal; on trap-heavy mazes it may differ from the
exhaustive backtracking v1 result.
*/
static int run_dijkstra(Maze *maze, LinkedList *backpack, Stack *path, DisplayMode display) {
    int n    = (*maze).rows * (*maze).cols;
    int cols = (*maze).cols;
    int goal = (*maze).exit_pos;
    int offsets[4] = {-cols, +cols, -1, +1};

    int  *dist   = malloc(n * sizeof(int));
    int  *parent = malloc(n * sizeof(int));
    char *closed = calloc(n, 1);
    if (!dist || !parent || !closed) {
        fprintf(stderr, "malloc failed\n");
        free(dist); free(parent); free(closed);
        return 0;
    }
    for (int i = 0; i < n; i++) { dist[i] = INT_MIN; parent[i] = -1; }

    Stack trail;
    stack_init(&trail);

    Heap h;
    heap_init(&h);

    int start = (*maze).player_pos;
    dist[start] = 0;
    /* priority = -dist for max-heap behaviour in min-heap */
    HeapNode sn = {start, 0, 0, -1};
    heap_push(&h, sn);

    int result = 0;

    while (!heap_is_empty(&h)) {
        HeapNode cur = heap_pop(&h);
        if (closed[cur.pos]) continue;
        closed[cur.pos] = 1;
        parent[cur.pos] = cur.parent;

        if (display != DISPLAY_NONE) {
            build_trail(&trail, parent, cur.pos);
            renderer_draw(maze, cur.pos, backpack, &trail);
            if (display == DISPLAY_INTERACTIVE)
                renderer_wait_interactive();
        }

        if (cur.pos == goal) {
            reconstruct_path(parent, goal, path);
            simulate_backpack(path, maze, backpack);
            renderer_finalize(path, maze, backpack);
            result = 1;
            break;
        }

        for (int d = 0; d < 4; d++) {
            if (is_wrap(cur.pos, d, cols)) continue;
            int next = cur.pos + offsets[d];
            if (next < 0 || next >= n)             continue;
            if ((*maze).cells[next] == CELL_WALL)  continue;
            if (closed[next])                      continue;
            if (!(*maze).reachable[next])          continue;

            /* Only treasures have positive weight; traps are treated as neutral
             * in the priority function (exact trap loss is applied in the replay). */
            int w = ((*maze).cells[next] == CELL_TREASURE)
                    ? (*maze).treasure_values[next] : 0;

            int new_dist = cur.g + w;
            if (new_dist > dist[next]) {
                dist[next] = new_dist;
                HeapNode nn = {next, -new_dist, new_dist, cur.pos};
                heap_push(&h, nn);
            }
        }
    }

    stack_free(&trail);
    heap_free(&h);
    free(dist); free(parent); free(closed);
    return result;
}

/*
Public entry point
*/
int pathfind_run(Maze *maze, LinkedList *backpack, PathfindMode mode, DisplayMode display) {
    if (display == DISPLAY_INTERACTIVE)
        renderer_set_delay(0);

    Stack path;
    stack_init(&path);

    int result = (mode == PATHFIND_BEST)
        ? run_dijkstra(maze, backpack, &path, display)
        : run_astar(maze, backpack, &path, display);

    stack_free(&path);
    return result;
}
