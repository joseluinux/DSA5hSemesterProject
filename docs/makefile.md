# Makefile

## Usage

```sh
make                                     # compile everything → produces ./lab
./lab Tabletops/labyrinth_10x10.txt      # run
make clean                               # delete compiled files
```

## Commands

| Command | What it does |
|---|---|
| `make` | Builds the `lab` executable. Only recompiles files that changed since the last build. |
| `make clean` | Deletes all `.o` object files and the `lab` executable. |


## Variables

| Variable | Value | Purpose |
|---|---|---|
| `CC` | `gcc` | The compiler. Change to `clang` to use Clang instead. |
| `CFLAGS` | `-Wall -Wextra -g` | Flags passed to every compilation step. |
| `SRCS` | `main.c lab.c ...` | All source files in the project. |
| `OBJS` | `main.o lab.o ...` | Derived automatically from `SRCS` by replacing `.c` with `.o`. |
| `TARGET` | `lab` | Name of the final executable. |

**Compiler flags explained:**

| Flag | Effect |
|---|---|
| `-Wall` | Enables common warnings: unused variables, missing return values, implicit declarations, etc. |
| `-Wextra` | Enables additional warnings on top of `-Wall`. |
| `-g` | Embeds debug information so `gdb` can show line numbers and variable names. Remove for a release build. |

## How a build works

Running `make` triggers the following sequence:

**1. Make reads the dependency graph**

The `$(TARGET)` rule says: to build `lab`, first build `main.o lab.o linked_list.o dlinked_list.o`.

**2. Each `.c` file is compiled into a `.o` object file**

```sh
gcc -Wall -Wextra -g -c main.c          # → main.o
gcc -Wall -Wextra -g -c lab.c           # → lab.o
gcc -Wall -Wextra -g -c linked_list.c   # → linked_list.o
gcc -Wall -Wextra -g -c dlinked_list.c  # → dlinked_list.o
```

The `-c` flag means "compile only, do not link." Each `.o` file contains the compiled machine code for its source file but is not yet an executable.

**3. The object files are linked into the executable**

```sh
gcc -Wall -Wextra -g -o lab main.o lab.o linked_list.o dlinked_list.o
```

The linker combines all `.o` files, resolves calls between them (e.g. `main.c` calling `list_create` defined in `linked_list.c`), and produces the final `lab` executable.

## Incremental builds

Make compares the modification timestamp of each output file against its inputs. If a source file has not changed since the last build, its `.o` file is up to date and Make skips it.

For example, after editing only `lab.c`:

```
make: 'main.o' is up to date.         ← skipped
gcc -Wall -Wextra -g -c lab.c         ← recompiled
make: 'linked_list.o' is up to date.  ← skipped
make: 'dlinked_list.o' is up to date. ← skipped
gcc -Wall -Wextra -g -o lab ...        ← relinked
```

This matters little for a four-file project, but saves significant time as projects grow.

## .PHONY

```makefile
.PHONY: clean
```

`clean` is a rule name, not a real file. Without `.PHONY`, if a file named `clean` ever existed in the project directory, Make would see that the "file" is already up to date and silently skip the rule. `.PHONY` tells Make to always run the rule regardless.

## Adding a new source file

When a new `.c` file is created, add it to `SRCS`:

```makefile
SRCS = main.c lab.c linked_list.c dlinked_list.c new_file.c
```

`OBJS` and the build rules update automatically — no other changes needed.
