# Architecture & Flow

Visual overview of how the modules interact and what each one is responsible for.

> For search optimizations applied to `BACKTRACK_BEST` (reachability + branch and bound), see [`optimizations.md`](optimizations.md).

## 1. Module Dependency Map

Who knows about whom at compile time. Arrows mean "depends on / includes".

```mermaid
graph TD
    main["<b>main.c</b><br/>4-question menu + entry point<br/>Wires everything together"]

    maze["<b>maze/maze.c</b><br/>Loads file → flat grid array<br/>BFS reachability + treasure pre-assignment"]

    backtrack["<b>engine/backtrack.c</b><br/>V1: DFS — FIRST or BEST mode<br/>Reachability pruning + branch-and-bound"]

    pathfind["<b>engine/pathfind.c</b><br/>V2: A* (FIRST) + Dijkstra (BEST)<br/>Heap priority queue + parent[] chain"]

    renderer["<b>engine/renderer.c</b><br/>Terminal trail display + backpack<br/>Prints/writes final solution"]

    renderer_gfx["<b>engine/renderer_gfx.c</b><br/>Optional raylib window<br/>Compiled only with WITH_GFX=1"]

    heap["<b>structures/heap.c</b><br/>Binary min-heap<br/>Priority queue for pathfind"]

    stack["<b>structures/stack.c</b><br/>Dynamic (realloc) int stack<br/>Tracks current path"]

    linked_list["<b>structures/linked_list.c</b><br/>Sorted linked list<br/>The backpack"]

    main       --> maze
    main       --> backtrack
    main       --> pathfind
    main       --> linked_list
    main       -. "WITH_GFX=1" .-> renderer_gfx

    backtrack  --> maze
    backtrack  --> stack
    backtrack  --> linked_list
    backtrack  --> renderer

    pathfind   --> maze
    pathfind   --> heap
    pathfind   --> stack
    pathfind   --> linked_list
    pathfind   --> renderer

    renderer   --> maze
    renderer   --> linked_list
    renderer   --> stack
    renderer   -. "WITH_GFX=1" .-> renderer_gfx
```

**Key rule:** data structures (`stack`, `linked_list`, `heap`) and `maze` have zero internal dependencies — they are the foundation. `backtrack` and `pathfind` are the two search engines; each orchestrates the structures it needs. `main` only sets up, dispatches, and tears down.

## 2. Ownership of Data

Each live object is owned by exactly one place.

```mermaid
graph LR
    main_owns["main()"]

    maze_obj["Maze*  (heap, via maze_load)\n─────────────────\ncells[]           raw grid chars\nvisited[]         runtime DFS state\nreachable[]       BFS pre-computed\ntreasure_values[] pre-assigned coins"]

    backpack_obj["LinkedList\n(stack-allocated in main)"]

    path_stack["Stack\n(stack-allocated inside backtrack_run\nor pathfind_run; heap data inside)"]

    heap_obj["Heap\n(stack-allocated inside pathfind_run;\nheap data inside)"]

    main_owns -- "creates / frees" --> maze_obj
    main_owns -- "initialises"      --> backpack_obj

    bt_uses["backtrack_run()"]
    bt_uses -. "reads reachable[]\nmutates visited[]" .-> maze_obj
    bt_uses -. "push / pop"                           .-> path_stack
    bt_uses -. "insert / remove"                      .-> backpack_obj

    pf_uses["pathfind_run()"]
    pf_uses -. "reads reachable[]" .-> maze_obj
    pf_uses -. "push / pop"        .-> heap_obj
    pf_uses -. "push (path)"       .-> path_stack
    pf_uses -. "insert / remove"   .-> backpack_obj

    renderer_uses["renderer_draw()"]
    renderer_uses -. "reads only" .-> maze_obj
    renderer_uses -. "reads only" .-> backpack_obj
    renderer_uses -. "reads only" .-> path_stack
```

## 3. Runtime Sequence

Full flow from launch to program exit. The menu now asks 4 independent questions.

```mermaid
sequenceDiagram
    participant U  as user
    participant M  as main
    participant Mz as maze
    participant BT as backtrack
    participant PF as pathfind
    participant St as stack/heap
    participant LL as linked_list
    participant R  as renderer

    U  ->> M  : ./maze [filepath]
    M  ->> U  : prompt for file (if no argv[1])
    U  ->> M  : filepath

    M  ->> U  : Algorithm? (1=V1 backtrack / 2=V2 A*/Dijkstra)
    M  ->> U  : Display?   (1=Terminal / 2=Graphical)
    M  ->> U  : Animation? (1=Interactive / 2=Auto / 3=Silent)
    M  ->> U  : Path mode? (1=First / 2=Best)
    U  ->> M  : 4 answers → algo, display_type, DisplayMode, PathfindMode/BacktrackMode

    M  ->> Mz : maze_load(filepath)
    Note over Mz: infer cols×rows from grid<br/>fill cells[], find P and S<br/>BFS from S → reachable[]<br/>rand values → treasure_values[]
    Mz -->> M : Maze* (or NULL → exit)

    M  ->> LL : list_init(&backpack)

    alt algo == 2  (V2 A*/Dijkstra)
        M  ->> PF : pathfind_run(maze, &backpack, mode, display)
        Note over PF: init Heap + dist[] + parent[] + closed[]<br/>push start node
        loop Each heap expansion
            PF ->> R  : renderer_draw (build_trail from parent[])
            PF ->> St : heap_pop → expand neighbors
            PF ->> Mz : reachable[neighbor]? (skip if 0)
            PF ->> St : heap_push relaxed neighbor
        end
        PF ->> St : reconstruct_path → push path to Stack
        PF ->> LL : simulate_backpack → insert/remove along path
        PF ->> R  : renderer_finalize(path, maze, backpack)
        PF -->> M : return 1 (found) or 0
    else algo == 1  (V1 backtracking)
        M  ->> BT : backtrack_run(maze, &backpack, mode, display)
        Note over BT: push player_pos, mark visited
        loop Each DFS step
            BT ->> R  : renderer_draw
            BT ->> Mz : reachable[neighbor]? (skip if 0)
            alt neighbor valid and reachable
                BT ->> LL : list_insert / list_remove_head
                BT ->> St : stack_push(neighbor)
            else all directions exhausted
                BT ->> St : stack_pop (backtrack)
                Note over BT: BEST: undo_event,<br/>B&B check at next call
            end
        end
        BT ->> R  : renderer_finalize(path, maze, backpack)
        BT -->> M : return 1 or 0
    end

    alt found
        M  ->> U  : print total treasure + backpack
    else no solution
        M  ->> U  : "NO SOLUTION"
    end

    M ->> Mz : maze_free(maze)
    M ->> LL : list_free(&backpack)
```

## 4. V1 — Backtracking Modes

### BACKTRACK_FIRST — Iterative DFS

Stops at the first exit reached. Non-reachable neighbors are skipped via the `reachable[]` pre-computation.

```mermaid
flowchart TD
    START([Start]) --> INIT["Push player_pos onto stack\nMark player_pos visited"]
    INIT --> DRAW["renderer_draw (if not DISPLAY_NONE)"]
    DRAW --> INTER{DISPLAY_INTERACTIVE?}
    INTER -->|Yes| WAIT["Wait for Enter"]
    INTER -->|No| EXIT_CHECK
    WAIT --> EXIT_CHECK

    EXIT_CHECK{Current cell == 'S'?}
    EXIT_CHECK -->|Yes| WRITE["renderer_finalize\nReturn 1 — SUCCESS"]
    WRITE --> END_OK([Done])

    EXIT_CHECK -->|No| TRY["Try next direction ↑ ↓ ← →"]
    TRY --> VALID{"valid?\nnot wall, not visited,\nin bounds, no wrap\nreachable[]?"}

    VALID -->|Yes| EVENT{Cell type?}
    EVENT -->|'T' treasure| TREASURE["list_insert(treasure_values[pos])\npush neighbor, mark visited"]
    EVENT -->|'A' trap| TRAP["list_remove_head()\npush neighbor, mark visited"]
    EVENT -->|' ' corridor| MOVE["push neighbor\nmark visited"]

    TREASURE --> DRAW
    TRAP      --> DRAW
    MOVE      --> DRAW

    VALID -->|No — tried all 4| DEAD_END["stack_pop — step back"]
    DEAD_END --> EMPTY{Stack empty?}
    EMPTY -->|Yes| NO_SOL(["Return 0 — NO SOLUTION"])
    EMPTY -->|No| DRAW
```

### BACKTRACK_BEST — Recursive DFS with Undo + Branch and Bound

Explores all paths. Pruned by reachability and by the upper-bound check `current_total + remaining_treasure <= best_total`. On backtrack, reverses cell events using a `CellEvent` array; `current_total` and `remaining_treasure` are passed by value so undo is implicit.

```mermaid
flowchart TD
    START(["explore(pos, remaining, total)"]) --> BNB{"best found AND\ntotal + remaining\n≤ best_total?"}
    BNB -->|Yes| PRUNE(["return — prune sub-tree"])
    BNB -->|No| DRAW["renderer_draw (if not DISPLAY_NONE)"]

    DRAW --> EXIT_CHECK{Current cell == 'S'?}
    EXIT_CHECK -->|Yes| COMPARE["If total > best.total:\nsave path + backpack snapshot"]
    COMPARE --> RETURN([return])

    EXIT_CHECK -->|No| LOOP["For each direction ↑ ↓ ← →"]
    LOOP --> VALID{"valid?\nnot wall, not visited,\nreachable[]?"}

    VALID -->|No| NEXT[Next direction]
    NEXT --> LOOP

    VALID -->|Yes| APPLY["apply_event → ev_type, ev_val\nmark visited, push\nupdate new_remaining, new_total"]
    APPLY --> RECURSE["explore(next, new_remaining, new_total)"]
    RECURSE --> UNDO["pop, unmark visited\nundo_event:\n  T → list_remove_value(ev_val)\n  A → list_insert(ev_val)"]
    UNDO --> NEXT

    LOOP -->|All 4 tried| DONE([return])
```

## 5. V2 — Pathfinding Modes

Both A\* and Dijkstra share the same code skeleton in `pathfind.c`. The difference is the priority function stored in each `HeapNode`.

### PATHFIND_FIRST — A\*

Priority `f = g + h` where `g` = steps from start, `h` = Manhattan distance to exit.

```mermaid
flowchart TD
    START([Start]) --> INIT["dist[start]=0\nheap_push({start, h(start), 0, -1})"]
    INIT --> LOOP{heap_is_empty?}
    LOOP -->|Yes| NOSOL(["return 0 — no solution"])
    LOOP -->|No| POP["cur = heap_pop()"]
    POP --> SKIP{closed[cur.pos]?}
    SKIP -->|Yes| LOOP
    SKIP -->|No| CLOSE["closed[cur.pos] = 1\nparent[cur.pos] = cur.parent"]
    CLOSE --> DRAW["renderer_draw (build_trail)"]
    DRAW --> GOAL{cur.pos == exit?}
    GOAL -->|Yes| RECON["reconstruct_path\nsimulate_backpack\nrenderer_finalize\nreturn 1"]
    GOAL -->|No| EXPAND["For each valid reachable neighbor"]
    EXPAND --> RELAX{"new_g = cur.g+1\nnew_g < dist[next]?"}
    RELAX -->|Yes| PUSH["dist[next] = new_g\nheap_push({next, new_g+h(next), new_g, cur.pos})"]
    PUSH --> EXPAND
    RELAX -->|No| EXPAND
```

### PATHFIND_BEST — Dijkstra (max-treasure heuristic)

Priority is the negated accumulated treasure value (`-dist`), turning the min-heap into an effective max-heap.

- Only `CELL_TREASURE` cells contribute weight `w = treasure_values[next]`; corridors and traps have `w = 0`.
- Trap losses are applied during `simulate_backpack` on the final chosen path — not during expansion.
- **Trade-off:** cells are settled greedily; a later path with more net treasure after traps may be missed. This is an intentional O((V+E) log V) heuristic rather than the exact exponential solution.

## 6. Data Structure Roles

### Stack — the path memory

```
push(3)  push(4)  push(9)  pop()   peek()
  [3]   [3,4]  [3,4,9] [3,4]    → 4

Stores 1D cell indices. Top = current position.
Pop = backtrack one step.
```

`renderer_draw` uses the full stack contents to mark trail cells (`.`) on the display.

Used by: backtrack (path tracking), pathfind (reconstructed final path).

### LinkedList — the backpack

```
insert(40)   insert(15)   insert(75)   remove_head()   insert(60)
  [40]       [15,40]    [15,40,75]       [40,75]       [40,60,75]
              ^sorted                    ^15 lost
                                         (trap hit)
```

Always sorted ascending so the **head is always the cheapest treasure** — the one sacrificed on a trap, minimising total loss.

| Operation | Complexity | Caller |
|---|---|---|
| `list_insert` | O(n) — walk to sorted position | Treasure pickup |
| `list_remove_head` | O(1) — always used by traps | Trap event |
| `list_remove_value` | O(n) — undo a specific pickup | BACKTRACK_BEST undo |

### Heap — the priority queue

Binary min-heap stored as a flat array. Capacity doubles via `realloc` starting at 64 nodes.

```
Heap property: data[parent].priority <= data[child].priority

push: insert at end, sift-up   → O(log n)
pop:  swap root with last, sift-down → O(log n)
```

For Dijkstra's max-treasure variant, priorities are negated at insertion — no code change to the heap itself.

Used by: pathfind only (backtrack uses an explicit stack instead).

## 7. Maze Memory Layout

The maze is stored as a **flat 1D array** (row-major order). The `Maze` struct holds four **heap-allocated parallel arrays** over the same index space, each `malloc`'d to `rows * cols` at load time and freed by `maze_free`:

```
cells[i]           — char *; raw cell char from the file ('P', 'T', 'A', 'S', ' ', '#')
visited[i]         — char *; 0/1, mutated during DFS, reset on backtrack (BEST mode)
reachable[i]       — char *; 0/1, computed once at load by BFS from exit, never mutated
treasure_values[i] — int  *; pre-assigned coin value (1–100) for CELL_TREASURE cells, 0 otherwise
```

Index formula for a 4×5 grid (cols = 5):

```
Row 0: cells[0]  cells[1]  cells[2]  cells[3]  cells[4]
Row 1: cells[5]  cells[6]  cells[7]  cells[8]  cells[9]
...

index(row, col) = row * cols + col

Direction offsets:
  UP    = −cols   DOWN  = +cols   LEFT  = −1   RIGHT = +1
```

**Column-wrap guard:** before moving LEFT, check `current % cols == 0`; before RIGHT, check `current % cols == cols − 1`. Without this, the algorithm silently steps from the end of one row to the start of the next.
