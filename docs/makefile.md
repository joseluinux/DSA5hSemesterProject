# Makefile

## Usage

```sh
make                              # compile everything → produces ./maze
make WITH_GFX=1                   # compile with optional raylib graphical renderer
./maze mazes/maze_10x10.txt       # run with a maze file
make test                         # build and run all automated unit tests
make test-visual                  # build and run visual test binary
make clean                        # delete all build artifacts
```

## Commands

| Command | What it does |
|---|---|
| `make` | Builds the `maze` executable with terminal-only rendering. |
| `make WITH_GFX=1` | Same, but also compiles `renderer_gfx.c` (raylib). Requires `libraylib-dev`. |
| `make test` | Builds and runs the three automated test binaries (stack, linked_list, backtrack). |
| `make test-visual` | Builds and runs the visual test binary against `mazes/maze_10x10.txt`. |
| `make clean` | Deletes the `build/` directory and the `maze` executable. |


## Variables

| Variable | Value | Purpose |
|---|---|---|
| `CC` | `gcc` | The compiler. Change to `clang` to use Clang instead. |
| `CFLAGS` | `-Wall -Wextra -g -Isrc -MMD -MP -Iinclude` | Flags passed to every compilation step. |
| `BUILD` | `build` | Output directory for all `.o`, `.d`, and test binaries. |
| `WITH_GFX` | (unset / `1`) | When set to `1`, adds `-DWITH_GFX` to `CFLAGS` and links raylib. |

**Compiler flags explained:**

| Flag | Effect |
|---|---|
| `-Wall` | Enables common warnings: unused variables, missing return values, implicit declarations, etc. |
| `-Wextra` | Enables additional warnings on top of `-Wall`. |
| `-g` | Embeds debug information so `gdb` can show line numbers and variable names. Remove for a release build. |
| `-Iinclude` | Adds `include/` to the header search path so `#include <maze.h>` resolves without a path prefix. |
| `-Isrc` | Keeps nested source paths resolvable during compilation. |
| `-MMD -MP` | Generate `.d` dependency files that track header changes (see Automatic Dependency Tracking below). |

## How a build works

Running `make` triggers the following sequence:

**1. Make reads the dependency graph**

The `maze` rule says: to build `maze`, first build all `.o` files listed in `OBJ`.

**2. Each `.c` file is compiled into a `.o` object file**

```sh
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/main.c                       -o build/main.o
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/maze/maze.c                   -o build/maze.o
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/structures/stack.c            -o build/stack.o
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/structures/heap.c             -o build/heap.o
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/structures/linked_list.c      -o build/linked_list.o
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/engine/backtrack.c            -o build/backtrack.o
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/engine/pathfind.c             -o build/pathfind.o
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/engine/renderer.c             -o build/renderer.o
# Only with WITH_GFX=1:
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -DWITH_GFX -c src/engine/renderer_gfx.c -o build/renderer_gfx.o
```

The `-c` flag means "compile only, do not link." Each `.o` file contains machine code for its source but is not yet an executable.

**3. The object files are linked into the executable**

```sh
# Without WITH_GFX:
gcc ... build/main.o build/maze.o build/stack.o build/heap.o build/linked_list.o \
        build/backtrack.o build/pathfind.o build/renderer.o -o maze

# With WITH_GFX=1:
gcc ... <same .o files> build/renderer_gfx.o -o maze \
        -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

## WITH_GFX — optional graphical renderer

`renderer_gfx.c` wraps the raylib library for a native windowed display. It is excluded from the default build to avoid a mandatory system dependency.

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

To install raylib on Debian/Ubuntu:
```sh
sudo apt install libraylib-dev
```

## Incremental builds

Make compares the modification timestamp of each output file against its inputs. If a source file has not changed since the last build, its `.o` file is up to date and Make skips it.

For example, after editing only `src/engine/pathfind.c`:

```
make: 'build/main.o' is up to date.        ← skipped
make: 'build/maze.o' is up to date.        ← skipped
gcc ... -c src/engine/pathfind.c            ← recompiled
...
gcc ... -o maze ...                         ← relinked
```

## .PHONY

```makefile
.PHONY: all test test-visual clean
```

`all`, `test`, `test-visual`, and `clean` are rule names, not real files. Without `.PHONY`, if a file with one of those names ever existed, Make would skip the rule silently.

## Automatic Dependency Tracking

Avoids stale `.o` files when a header changes. Already enabled in `CFLAGS` via `-MMD -MP`:

```makefile
-include $(OBJ:.o=.d)
```

`-MMD` generates a `.d` file alongside each `.o` listing all headers the source depends on.  
`-include` (with the leading `-`) silently skips missing `.d` files on the first build.  
After the first build, changing any header automatically triggers a recompile of every affected `.o`.

## Adding a new source file

When a new `.c` file is created under `src/`, add a corresponding `.o` entry to `OBJ` in the Makefile:

```makefile
OBJ = $(BUILD)/main.o        \
      $(BUILD)/maze.o        \
      $(BUILD)/stack.o       \
      $(BUILD)/heap.o        \
      $(BUILD)/linked_list.o \
      $(BUILD)/backtrack.o   \
      $(BUILD)/pathfind.o    \
      $(BUILD)/renderer.o    \
      $(BUILD)/new_module.o
```

`CFLAGS`, dependency tracking, and the link step update automatically — no other changes needed.
