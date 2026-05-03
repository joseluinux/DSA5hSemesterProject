# C Backtracking Maze

## Overview

This project implements the logic engine of an **archaeological exploration game** written in **C** for the "Data Structures and Algorithms" class @ FACENS. A treasure hunter must navigate an ancient and dangerous maze to find the exit — collecting treasures and avoiding traps along the way. The goal is not just to escape, but to **maximize the total value of treasures collected**.

Two generations of solver are available simultaneously at runtime:

- **V1 — Backtracking DFS**: exhaustive depth-first search with branch-and-bound pruning. Guarantees the globally optimal (maximum-treasure) path.
- **V2 — A\*/Dijkstra**: polynomial-time graph search. A\* finds the shortest path; Dijkstra finds the highest-treasure path as a fast heuristic.

## Gameplay Rules

### The Maze

- Represented as a **heap-allocated flat 1D array** — no fixed size limit; any maze that fits in memory is supported.
- Layout is **loaded from a `.txt` file**. No header line is required: `cols` = widest line, `rows` = non-blank line count.

| Symbol | Meaning         |
|--------|-----------------|
| `#`    | Wall            |
| ` `    | Corridor (path) |
| `P`    | Player (start)  |
| `T`    | Treasure        |
| `A`    | Trap            |
| `S`    | Exit            |

### Movement & Search

- V1 implements a **backtracking** search algorithm using an explicit stack, navigating all crossroads and dead ends.
- V2 implements **A\*** (PATHFIND_FIRST) and **Dijkstra** (PATHFIND_BEST) using a binary min-heap priority queue.

### The Backpack (Mochila)

- Every treasure found is stored in the **backpack** (sorted linked list).
- Each treasure has a **randomly generated value between 1 and 100 coins**, pre-assigned at maze load time.
- **Trap rule:** When the player steps on a trap (`A`), the treasure in the **first position** of the backpack is lost.
- **Minimization strategy:** the backpack is always sorted in ascending order so the **lowest-value treasure is at the head** — the one sacrificed on a trap, minimizing total loss.

### Visual Interface

- **Terminal mode:** the maze is rendered step by step in ASCII. The current player position is shown as `@`; the trail is shown as `.`.
- **Graphical mode** (optional, requires raylib): a native window with colored cells, sidebar backpack display, and animated step-through.

## Technical Requirements

| Requirement         | Detail                                                          |
|---------------------|-----------------------------------------------------------------|
| Language            | C                                                               |
| Data Structures     | Stack, Sorted Linked List, Binary Min-Heap                      |
| Input               | `.txt` file with maze layout (dimensions inferred at load time) |
| Output (terminal)   | Step-by-step ASCII visualization + final total value            |
| Output (file)       | `output/solution.txt` recording the final solution path         |
| Optional GFX        | raylib window (`make WITH_GFX=1`)                               |

## Project Structure

```
/
├── include/                        # All public headers (angle-bracket includes via -Iinclude)
│   ├── defs.h                      # Shared cell-type constants (CELL_WALL, CELL_PLAYER, …)
│   ├── maze.h
│   ├── backtrack.h
│   ├── pathfind.h
│   ├── renderer.h
│   ├── renderer_gfx.h              # Graphical renderer interface (raylib)
│   ├── heap.h
│   ├── stack.h
│   └── linked_list.h
│
├── src/
│   ├── main.c                      # Entry point — 4-question menu, wires everything together
│   ├── maze/
│   │   └── maze.c                  # Maze loading, BFS reachability, treasure pre-assignment
│   ├── engine/
│   │   ├── backtrack.c             # V1: DFS search — FIRST and BEST modes
│   │   ├── pathfind.c              # V2: A* (FIRST) and Dijkstra (BEST)
│   │   ├── renderer.c              # Terminal rendering and solution file output
│   │   └── renderer_gfx.c          # Graphical renderer (raylib, compiled with WITH_GFX=1)
│   └── structures/
│       ├── heap.c                  # Binary min-heap priority queue (used by pathfind)
│       ├── stack.c                 # Dynamic int stack — grows via realloc (tracks the current path)
│       └── linked_list.c           # Sorted linked list (the backpack)
│
├── tests/
│   ├── auto/
│   │   ├── test_stack.c            # Automated unit tests for the stack
│   │   ├── test_linked_list.c      # Automated unit tests for the linked list
│   │   └── test_backtrack.c        # Automated tests for the search algorithm
│   └── visual/
│       ├── visual_test_maze.c      # Renders a maze traversal for visual inspection
│       └── visual_test_backpack.c  # Simulates treasure/trap events, prints backpack state
│
├── mazes/
│   ├── maze_10x10.txt              # Named mazes for running the solver
│   ├── maze_20x15.txt
│   ├── maze_30x10.txt
│   ├── maze_40x40.txt
│   └── test/                       # Targeted mazes used by the automated test suite
│
├── docs/
│   ├── architecture.md             # Module map, data ownership, runtime sequence diagrams
│   ├── optimizations.md            # Reachability pre-computation, branch-and-bound, A* heuristic
│   ├── c-patterns.md               # Coding conventions used in this project
│   ├── makefile.md                 # Build commands reference
│   └── makefile-patterns.md        # Makefile structure and build patterns
│
├── scripts/
│   ├── generate_mazes.py           # Script to generate random maze .txt files
│   └── stress_test.py              # Generates extreme-sized mazes and runs the solver on each
│
├── output/
│   └── solution.txt                # Final solution path (generated at runtime)
│
└── Makefile
```

## Input File Format

No header line is required. Dimensions are inferred at load time: `cols` = length of the widest line, `rows` = number of non-blank lines. Every cell must be one of the symbols in the table above; short lines are padded with `#`. There is no fixed size limit — the arrays are heap-allocated to exactly `rows × cols` at load time.

**Example (`maze_10x10.txt`):**
```
##########
#P  T    #
# ###### #
# #      #
# # #### #
# # #A   #
# # # ####
# # #    #
#   #   S#
##########
```

## Output

### Terminal
At every step of the traversal, the program prints:
- The current state of the maze (with the player's position marked as `@`, trail as `.`).
- The contents of the backpack (list of treasure values).

At the end:
```
=== EXIT REACHED ===
Total treasure value: 230 coins
Backpack: [15, 40, 75, 100]
```

### Solution File (`output/solution.txt`)
Records the final solution grid with the path marked as `.`, plus the backpack summary.

## Algorithm Design

### V1 — Backtracking DFS

#### BACKTRACK_FIRST — Iterative DFS
1. Start at position `P`.
2. Try moving in each direction (Up, Down, Left, Right).
3. Skip walls, visited cells, and non-reachable cells.
4. If a dead end is reached, **backtrack** using the stack (pop = undo last step).
5. Stops as soon as `S` is reached.

#### BACKTRACK_BEST — Recursive DFS with Undo + Branch and Bound
1. Explores **all** simple paths to `S`.
2. On backtrack, undoes cell events: treasure pickup (`list_remove_value`) and trap loss (`list_insert` to restore).
3. Prunes branches where `current_total + remaining_treasure <= best_total` — the upper bound can never beat what was already found.
4. Returns the path with the highest total treasure value.

### V2 — A\* and Dijkstra

Both algorithms share the same binary min-heap priority queue and `parent[]` array for path reconstruction.

#### PATHFIND_FIRST — A\*
- Priority: `f(n) = g(n) + h(n)` where `g` = steps from start, `h` = Manhattan distance to exit.
- Manhattan heuristic is admissible (never overestimates) — guarantees the shortest path.
- Closed set prevents re-expanding settled cells.

#### PATHFIND_BEST — Dijkstra
- Priority: negative accumulated treasure value (negated to simulate max-heap in the min-heap).
- Only treasure cells contribute positive weight; traps are neutral in the priority function.
- Exact trap losses are applied in a final replay pass after the path is found.

### Backpack Sorting Strategy
- Data structure: **sorted linked list** (ascending order by value).
- On treasure pickup: `list_insert` walks to the correct position — O(n).
- On trap: `list_remove_head` removes the cheapest item — O(1).
- On best-path backtrack: `list_remove_value` undoes a specific pickup — O(n).

## How to Build & Run

### 1. Build

```bash
make             # standard terminal build
make WITH_GFX=1  # optional: enable raylib graphical window
                 # requires: sudo apt install libraylib-dev
```

### 2. Run

```bash
./maze mazes/maze_10x10.txt    # pass the maze file directly
./maze                          # omit it to be prompted
```

### 3. Answer the 4-question menu

```
Algorithm:
  1. Backtracking v1  (exhaustive DFS, branch-and-bound)
  2. A* / Dijkstra v2 (A* for first path, Dijkstra for best path)

Display:
  1. Terminal (ASCII)
  2. Graphical (raylib)      ← only shown when built with WITH_GFX=1

Animation:
  1. Interactive  (step-by-step, press Enter / SPACE)
  2. Auto         (40 ms per step)
  3. Silent       (result only)

Path mode:
  1. First path found
  2. Best path  (maximum treasure)
```

### 4. Read the result

The solution is printed to the terminal and written to `output/solution.txt`.

### Other commands

```bash
make test         # Run all automated unit tests (41 tests)
make test-visual  # Run visual test rendering
make clean        # Remove build artifacts
```

## Error Handling

- Maze file not found → print error message and exit.
- No valid path from `P` to `S` → inform the user that the maze has no solution.
- Trap triggered with an empty backpack → skip item removal (no crash).
- Maze file is empty or has no `P` / `S` cell → print error and exit.
- `malloc`/`realloc` failure → print to `stderr` and exit.

## Grading Criteria

| Criterion                                          | Weight |
|----------------------------------------------------|--------|
| Correct use of pointers and data structures        | ✓      |
| Efficiency of the maze search algorithm            | ✓      |
| Error handling                                     | ✓      |
| Code quality (indentation, comments, modularity)   | ✓      |
| Step-by-step ASCII visualization                   | ✓      |
| Oral presentation & team knowledge demonstration   | ✓      |
