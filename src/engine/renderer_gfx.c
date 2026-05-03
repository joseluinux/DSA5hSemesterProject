#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <renderer_gfx.h>
#include <defs.h>

#define MAX_WIN_W  1200
#define MAX_WIN_H   800
#define SIDEBAR_W   260
#define MIN_CELL      3

static int cell_px = 16;
static int grid_w, grid_h;
static int window_closed = 0;

/* Cached last frame so wait functions can keep the window alive. */
static struct {
    const Maze       *m;
    int               pos;
    const LinkedList *backpack;
    const Stack      *path;
    int               valid;
} last;

/*
Color map
*/
static Color cell_color(char cell) {
    switch (cell) {
        case CELL_WALL:     return (Color){40,  40,  40,  255};
        case CELL_CORRIDOR: return (Color){210, 210, 210, 255};
        case CELL_TREASURE: return (Color){255, 200,   0, 255};
        case CELL_TRAP:     return (Color){210,  50,  50, 255};
        case CELL_EXIT:     return (Color){ 50, 200,  80, 255};
        case CELL_PLAYER:   return (Color){100, 149, 237, 255};
        default:            return (Color){128, 128, 128, 255};
    }
}

/*
Draw
*/
static void do_draw(const Maze *m, int pos, const LinkedList *backpack, const Stack *path) {
    int n = (*m).rows * (*m).cols;

    char *on_path = calloc(n, 1);
    if (!on_path) return;
    for (int i = 0; i <= (*path).top; i++)
        on_path[(*path).data[i]] = 1;

    BeginDrawing();
    ClearBackground((Color){30, 30, 30, 255});

    /* Grid */
    for (int r = 0; r < (*m).rows; r++) {
        for (int c = 0; c < (*m).cols; c++) {
            int   idx  = r * (*m).cols + c;
            int   x    = c * cell_px;
            int   y    = r * cell_px;
            char  cell = (*m).cells[idx];
            Color col;

            if (idx == pos) {
                col = (Color){0, 220, 255, 255};
            } else if (on_path[idx] && cell == CELL_CORRIDOR) {
                col = (Color){140, 190, 255, 255};
            } else {
                col = cell_color(cell);
            }

            DrawRectangle(x, y, cell_px, cell_px, col);

            if (cell_px >= 14 && cell != CELL_WALL && cell != CELL_CORRIDOR) {
                char s[2] = {(idx == pos) ? '@' : cell, '\0'};
                int  fw   = MeasureText(s, cell_px / 2);
                DrawText(s, x + (cell_px - fw) / 2, y + cell_px / 4,
                         cell_px / 2, BLACK);
            }

            if (cell_px >= 4)
                DrawRectangleLines(x, y, cell_px, cell_px, (Color){60, 60, 60, 80});
        }
    }

    /* Sidebar */
    int sx = grid_w + 10;
    int sy = 10;
    DrawText("BACKPACK", sx, sy, 16, WHITE);
    sy += 24;

    int total = 0;
    for (const Node *nd = (*backpack).head; nd; nd = (*nd).next) {
        total += (*nd).value;
        if (sy < grid_h - 40) {
            char buf[32];
            snprintf(buf, sizeof(buf), "  %d coins", (*nd).value);
            DrawText(buf, sx, sy, 13, YELLOW);
            sy += 17;
        }
    }

    char tbuf[64];
    snprintf(tbuf, sizeof(tbuf), "Total: %d", total);
    DrawText(tbuf, sx, grid_h - 36, 15, GREEN);
    DrawText("[SPACE] next  [ESC] quit", sx, grid_h - 18, 11, GRAY);

    EndDrawing();  /* swaps buffers and polls input events */
    free(on_path);
}

static void draw_blank(void) {
    BeginDrawing();
    ClearBackground((Color){30, 30, 30, 255});
    EndDrawing();
}

void renderer_gfx_init(const Maze *m) {
    int avail_w = MAX_WIN_W - SIDEBAR_W;
    int avail_h = MAX_WIN_H;
    int cw = (*m).cols > 0 ? avail_w / (*m).cols : MIN_CELL;
    int ch = (*m).rows > 0 ? avail_h / (*m).rows : MIN_CELL;
    cell_px = cw < ch ? cw : ch;
    if (cell_px < MIN_CELL) cell_px = MIN_CELL;

    grid_w = (*m).cols * cell_px;
    grid_h = (*m).rows * cell_px;
    if (grid_h < 200) grid_h = 200;

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(grid_w + SIDEBAR_W, grid_h, "Maze Solver");
    SetTargetFPS(60);

    last.valid    = 0;
    window_closed = 0;
}

void renderer_gfx_draw(const Maze *m, int pos, const LinkedList *backpack, const Stack *path) {
    last.m        = m;
    last.pos      = pos;
    last.backpack = backpack;
    last.path     = path;
    last.valid    = 1;
    do_draw(m, pos, backpack, path);
}

void renderer_gfx_wait_interactive(void) {
    /* Draw first so EndDrawing() polls events, THEN check keys.
     * Checking keys before EndDrawing reads stale input state. */
    while (!WindowShouldClose()) {
        if (last.valid) do_draw(last.m, last.pos, last.backpack, last.path);
        else            draw_blank();

        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) ||
            IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ESCAPE))
            break;
    }
}

void renderer_gfx_wait_auto(double seconds) {
    /* Keep redrawing during the delay so the OS doesn't mark the window
     * as unresponsive and so input events are processed continuously. */
    double end = GetTime() + seconds;
    while (GetTime() < end && !WindowShouldClose()) {
        if (last.valid) do_draw(last.m, last.pos, last.backpack, last.path);
        else            draw_blank();
    }
}

void renderer_gfx_show_and_close(void) {
    if (window_closed) return;
    while (!WindowShouldClose()) {
        if (last.valid) do_draw(last.m, last.pos, last.backpack, last.path);
        else            draw_blank();
    }
    CloseWindow();
    window_closed = 1;
}

/* Close the window immediately — called from renderer.c when the user
 * closes the window mid-search so it doesn't stay frozen until search ends. */
void renderer_gfx_force_close(void) {
    if (!window_closed) {
        CloseWindow();
        window_closed = 1;
    }
}

int renderer_gfx_should_close(void) {
    return window_closed || WindowShouldClose();
}
