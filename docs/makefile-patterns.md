# Makefile Patterns

How the project's Makefile is structured and why.

## Variables

```makefile
CC     = gcc
CFLAGS = -Wall -Wextra -g -Isrc -MMD -MP -Iinclude

BUILD  = build
```

`-Iinclude` lets every file use `#include <maze.h>` (angle-bracket style) regardless of where it lives in `src/`.  
`-Isrc` keeps nested source paths resolvable during compilation.  
`-MMD -MP` generate `.d` dependency files that track header changes (see below).  
`-g` keeps debug symbols; strip it for a release build.

## Optional WITH_GFX Flag

The graphical renderer (`renderer_gfx.c`) is conditionally compiled to avoid a mandatory raylib dependency:

```makefile
ifdef WITH_GFX
  CFLAGS  += -DWITH_GFX
  GFX_OBJ  = $(BUILD)/renderer_gfx.o
  LDFLAGS  = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
else
  GFX_OBJ  =
  LDFLAGS  =
endif
```

`renderer.c` and `main.c` guard their raylib calls with `#ifdef WITH_GFX` — the binary works correctly without it. The `GFX_OBJ` variable is appended to `OBJ` after the conditional block.

## Directory Layout (build artifacts)

All `.o` files land in `build/` so the source tree stays clean:

```makefile
OBJ = $(BUILD)/main.o        \
      $(BUILD)/maze.o        \
      $(BUILD)/stack.o       \
      $(BUILD)/heap.o        \
      $(BUILD)/linked_list.o \
      $(BUILD)/backtrack.o   \
      $(BUILD)/pathfind.o    \
      $(BUILD)/renderer.o

OBJ += $(GFX_OBJ)    # renderer_gfx.o when WITH_GFX=1, empty otherwise
```

## Pattern Rule

Compile any `.c` → `.o` with one rule per source subdirectory:

```makefile
$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/maze/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/engine/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/structures/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@
```

`| $(BUILD)` is an order-only prerequisite — it creates the directory before compiling but doesn't trigger a rebuild if the directory's timestamp changes.

```makefile
$(BUILD):
	mkdir -p $(BUILD)
```

## Targets

```makefile
.PHONY: all test test-visual clean

all: maze          # default target

maze: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test: $(BUILD)/test_stack $(BUILD)/test_linked_list $(BUILD)/test_backtrack
	@echo "=== test_stack ==="
	./$(BUILD)/test_stack
	@echo "=== test_linked_list ==="
	./$(BUILD)/test_linked_list
	@echo "=== test_backtrack ==="
	./$(BUILD)/test_backtrack

test-visual: $(BUILD)/visual_test_maze $(BUILD)/visual_test_backpack
	./$(BUILD)/visual_test_maze mazes/maze_10x10.txt
	./$(BUILD)/visual_test_backpack

clean:
	rm -rf $(BUILD) maze
```

## Test Executables

Each test binary links only the modules it needs (not `main.o`). `test_backtrack` now includes `heap.o` and `pathfind.o` because `backtrack.c` and `pathfind.c` share the same test suite via `test_backtrack.c`:

```makefile
$(BUILD)/test_stack: tests/auto/test_stack.c $(BUILD)/stack.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/test_linked_list: tests/auto/test_linked_list.c $(BUILD)/linked_list.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/test_backtrack: tests/auto/test_backtrack.c      \
                         $(BUILD)/stack.o                  \
                         $(BUILD)/linked_list.o            \
                         $(BUILD)/maze.o                   \
                         $(BUILD)/backtrack.o              \
                         $(BUILD)/renderer.o               \
                         $(BUILD)/heap.o                   \
                         $(BUILD)/pathfind.o               \
                         $(GFX_OBJ) | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
```

The visual test links the same set (minus `main.o`, plus the test harness):

```makefile
$(BUILD)/visual_test_maze: tests/visual/visual_test_maze.c \
                           $(BUILD)/maze.o $(BUILD)/backtrack.o \
                           $(BUILD)/renderer.o $(BUILD)/stack.o \
                           $(BUILD)/linked_list.o $(BUILD)/heap.o \
                           $(BUILD)/pathfind.o $(GFX_OBJ) | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
```

## Automatic Dependency Tracking

Avoids stale `.o` files when a header changes. Already enabled in `CFLAGS` via `-MMD -MP`:

```makefile
-include $(OBJ:.o=.d)
```

`-MMD` generates a `.d` file alongside each `.o` listing all headers the source depends on.  
`-include` (with the leading `-`) silently skips missing `.d` files on the first build.  
After the first build, changing any header automatically triggers a recompile of every affected `.o`.
