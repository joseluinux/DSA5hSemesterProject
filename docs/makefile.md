# Makefile

## Usage

```sh
make                              # compile everything → produces ./maze
./maze mazes/maze_10x10.txt       # run with a maze file
make test                         # build and run all automated unit tests
make test-visual                  # build and run visual test binaries
make clean                        # delete all build artifacts
```

## Commands

| Command | What it does |
|---|---|
| `make` | Builds the `maze` executable. Only recompiles files that changed since the last build. |
| `make test` | Builds and runs all three automated test binaries (stack, linked_list, backtrack). |
| `make test-visual` | Builds and runs the visual test binaries against `mazes/maze_10x10.txt`. |
| `make clean` | Deletes the `build/` directory and the `maze` executable. |


## Variables

| Variable | Value | Purpose |
|---|---|---|
| `CC` | `gcc` | The compiler. Change to `clang` to use Clang instead. |
| `CFLAGS` | `-Wall -Wextra -g -Isrc -MMD -MP -Iinclude` | Flags passed to every compilation step. |
| `BUILD` | `build` | Output directory for all `.o`, `.d`, and test binaries. |

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
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/main.c              -o build/main.o
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/maze/maze.c          -o build/maze.o
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/structures/stack.c   -o build/stack.o
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/structures/linked_list.c -o build/linked_list.o
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/engine/backtrack.c   -o build/backtrack.o
gcc -Wall -Wextra -g -Isrc -MMD -MP -Iinclude -c src/engine/renderer.c    -o build/renderer.o
```

The `-c` flag means "compile only, do not link." Each `.o` file contains machine code for its source but is not yet an executable.

**3. The object files are linked into the executable**

```sh
gcc ... build/main.o build/maze.o build/stack.o build/linked_list.o build/backtrack.o build/renderer.o -o maze
```

## Incremental builds

Make compares the modification timestamp of each output file against its inputs. If a source file has not changed since the last build, its `.o` file is up to date and Make skips it.

For example, after editing only `src/maze/maze.c`:

```
make: 'build/main.o' is up to date.        ← skipped
gcc ... -c src/maze/maze.c                  ← recompiled
make: 'build/stack.o' is up to date.       ← skipped
...
gcc ... -o maze ...                         ← relinked
```

## .PHONY

```makefile
.PHONY: all test test-visual clean
```

`all`, `test`, `test-visual`, and `clean` are rule names, not real files. Without `.PHONY`, if a file with one of those names ever existed, Make would skip the rule silently. `.PHONY` tells Make to always run them.

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
OBJ = $(BUILD)/main.o \
      $(BUILD)/maze.o \
      $(BUILD)/stack.o \
      ...
      $(BUILD)/new_module.o
```

`CFLAGS`, dependency tracking, and the link step update automatically — no other changes needed.
