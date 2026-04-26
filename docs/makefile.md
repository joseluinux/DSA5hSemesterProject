# Makefile

## Usage

```sh
make                              # compile everything → produces ./lab
./lab                             # run (prompts for map file path)
make clean                        # delete compiled files
```

## Commands

| Command | What it does |
|---|---|
| `make` | Builds the `lab` executable. Only recompiles files that changed since the last build. |
| `make test` | Builds and runs the automated list tests, then cleans up. |
| `make test_visual` | Builds the interactive list tester. |
| `make test_stack` | Builds the manual stack tester. |
| `make clean` | Deletes all `.o` object files and executables. |


## Variables

| Variable | Value | Purpose |
|---|---|---|
| `CC` | `gcc` | The compiler. Change to `clang` to use Clang instead. |
| `CFLAGS` | `-Wall -Wextra -g -Iinclude` | Flags passed to every compilation step. |
| `SRCS` | `src/main.c src/lab.c ...` | All source files compiled into `lab`. |
| `OBJS` | derived from `SRCS` | `.c` → `.o` substitution, automatic. |
| `TARGET` | `lab` | Name of the final executable. |

**Compiler flags explained:**

| Flag | Effect |
|---|---|
| `-Wall` | Enables common warnings: unused variables, missing return values, implicit declarations, etc. |
| `-Wextra` | Enables additional warnings on top of `-Wall`. |
| `-g` | Embeds debug information so `gdb` can show line numbers and variable names. Remove for a release build. |
| `-Iinclude` | Adds `include/` to the header search path, so `#include "lab.h"` resolves without a path prefix. |

## How a build works

Running `make` triggers the following sequence:

**1. Make reads the dependency graph**

The `$(TARGET)` rule says: to build `lab`, first build all `.o` files listed in `$(OBJS)`.

**2. Each `.c` file is compiled into a `.o` object file**

```sh
gcc -Wall -Wextra -g -Iinclude -c src/main.c       # → src/main.o
gcc -Wall -Wextra -g -Iinclude -c src/lab.c        # → src/lab.o
gcc -Wall -Wextra -g -Iinclude -c src/linked_list.c  # → src/linked_list.o
gcc -Wall -Wextra -g -Iinclude -c src/dlinked_list.c # → src/dlinked_list.o
gcc -Wall -Wextra -g -Iinclude -c src/stack.c      # → src/stack.o
```

The `-c` flag means "compile only, do not link." Each `.o` file contains the compiled machine code for its source file but is not yet an executable.

**3. The object files are linked into the executable**

```sh
gcc -Wall -Wextra -g -Iinclude -o lab src/main.o src/lab.o src/stack.o src/linked_list.o src/dlinked_list.o
```

The linker combines all `.o` files, resolves calls between them, and produces the final `lab` executable.

## Incremental builds

Make compares the modification timestamp of each output file against its inputs. If a source file has not changed since the last build, its `.o` file is up to date and Make skips it.

For example, after editing only `src/lab.c`:

```
make: 'src/main.o' is up to date.         ← skipped
gcc ... -c src/lab.c                       ← recompiled
make: 'src/linked_list.o' is up to date.  ← skipped
...
gcc ... -o lab ...                         ← relinked
```

## .PHONY

```makefile
.PHONY: clean test
```

`clean` and `test` are rule names, not real files. Without `.PHONY`, if a file named `clean` or `test` ever existed, Make would skip the rule silently. `.PHONY` tells Make to always run them regardless.

## Adding a new source file

When a new `.c` file is created under `src/`, add it to `SRCS`:

```makefile
SRCS = src/main.c src/lab.c src/stack.c src/linked_list.c src/dlinked_list.c src/new_file.c
```

`OBJS` and the build rules update automatically — no other changes needed.
