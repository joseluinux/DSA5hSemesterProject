# C Coding Patterns

Patterns used in `linked_list.h` and `dlinked_list.h`.

## Table of Contents

1. [Include Guards](#1-include-guards)
2. [File-Level Documentation Block](#2-file-level-documentation-block)
3. [Minimal Includes](#3-minimal-includes)
4. [typedef struct](#4-typedef-struct)
5. [Self-Referential Structs](#5-self-referential-structs)
6. [Return Conventions](#6-return-conventions)
7. [const Correctness](#7-const-correctness)
8. [size_t for Sizes and Indices](#8-size_t-for-sizes-and-indices)
9. [Naming Conventions](#9-naming-conventions)
10. [Doxygen Documentation](#10-doxygen-documentation)
11. [Inline Member Documentation](#11-inline-member-documentation)

## 1. Include Guards

```c
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

/* ... entire file contents ... */

#endif /* LINKED_LIST_H */
```

A header can be included multiple times in the same translation unit — directly, or pulled in by other headers. Without protection, the compiler would see every `typedef` and `struct` definition twice and raise a redefinition error.

The guard works in three steps:

1. `#ifndef LINKED_LIST_H` — "only enter this block if the macro is not yet defined."
2. `#define LINKED_LIST_H` — defines it immediately, so any subsequent `#include` of this file is skipped.
3. `#endif` — closes the conditional block.

The macro name is the filename in `UPPER_SNAKE_CASE` with the `.` replaced by `_`. This is the universal C convention and avoids collisions with other headers.

## 2. File-Level Documentation Block

```c
/**
 * @file linked_list.h
 * @brief Singly linked list of integers.
 *
 * Insertion functions return 1 on success, 0 on failure.
 * Access and removal functions must only be called on a non-empty list...
 */
```

Every header opens with a block that answers before the reader looks at any code:

- **What is this?** (`@brief` — one line)
- **How does it behave?** (return conventions, preconditions)

This block is also processed by Doxygen to generate HTML/PDF API documentation automatically.

## 3. Minimal Includes

```c
#include <stddef.h> /* size_t */
```

Headers should include only what they directly use, and nothing more. Including unnecessary headers:

- slows down compilation (every `.c` that includes this header pays the cost),
- creates hidden dependencies (the `.c` file might accidentally rely on types pulled in transitively),
- pollutes the global namespace with macros and names from unrelated headers.

`<stddef.h>` is the correct minimal header for `size_t`. It does not pull in `printf`, `malloc`, or anything else from `<stdio.h>` / `<stdlib.h>`.

## 4. typedef struct

```c
typedef struct {
    ListNode *head;
    ListNode *tail;
    size_t    size;
} LinkedList;
```

In C (unlike C++), declaring `struct Foo { ... };` requires writing `struct Foo` everywhere it is used. `typedef` creates an alias so callers write `LinkedList` instead of `struct LinkedList`.

The pattern has two common forms:

| Form | When to use |
|---|---|
| `typedef struct { ... } Name;` | The struct does not need to refer to itself. |
| `typedef struct Name { ... } Name;` | The struct contains a pointer to its own type (see §5). |

## 5. Self-Referential Structs

```c
typedef struct ListNode {
    int              data;
    struct ListNode *next;  /* <-- points to its own type */
} ListNode;
```

A struct cannot contain a value of its own type (that would be infinitely large), but it can contain a *pointer* to its own type. This is the foundation of every linked structure: lists, trees, graphs.

The typedef alias (`ListNode`) is not yet available inside the struct body — the compiler has not finished reading the definition. The tag name (`struct ListNode`) must be used inside the body. The typedef is only resolved after the closing `}`.

## 6. Return Conventions

These headers use three conventions depending on what a function does:

**Insertion functions** return `int` — 1 on success, 0 if allocation failed:

```c
int list_push_back(LinkedList *list, int data);
int list_insert_ordered(LinkedList *list, int data);
```

**Pop and peek functions** return the element value directly. They require the list to be non-empty — the caller must check `list_is_empty()` first:

```c
int list_pop_front(LinkedList *list);   /* call only when non-empty */
int list_peek_back(const LinkedList *list);
```

**Removal by index** uses an output parameter for the value and returns 1/0 for success:

```c
int list_remove_at(LinkedList *list, size_t index, int *out);
/* out receives the removed value when not NULL */
```

This split exists because popping an integer has no sentinel value that reliably signals "empty" (0 and -1 are valid integers), while removal by index can fail for two distinct reasons (empty list or bad index) that warrant a status return.

## 7. const Correctness

```c
/* Read-only operations take a const pointer */
size_t    list_size      (const LinkedList *list);
int       list_is_empty  (const LinkedList *list);
int       list_peek_front(const LinkedList *list);
ListNode *list_find      (const LinkedList *list, int value);

/* Mutating operations take a non-const pointer */
int       list_push_back (LinkedList *list, int data);
int       list_pop_front (LinkedList *list);
```

`const T *` means "pointer to a T that this function promises not to modify." Applying it consistently:

- Lets callers pass a `const LinkedList *` to read-only functions without a cast.
- Serves as machine-checked documentation: if a function takes `const LinkedList *`, the reader knows it will not alter the list.

The rule of thumb: **add `const` to every pointer parameter that the function does not need to modify.**

## 8. size_t for Sizes and Indices

`size_t` is an unsigned integer type used for sizes and counts. It is defined in `<stddef.h>` and is what `sizeof` returns and what `malloc` expects.

```c
size_t    size;                                              /* struct field  */
size_t    list_size(const LinkedList *list);                 /* return type   */
int       list_insert_at(LinkedList *list, size_t index, int data); /* param */
```

It exists instead of plain `int` for two reasons:

1. **It's always unsigned** — a size or index can never be negative, so the type reflects that. Using `int` would allow storing `-1` in a field that should never be negative.
2. **It matches the platform's address space** — on a 32-bit system it's 32 bits; on a 64-bit system it's 64 bits. A plain `int` is always 32 bits even on 64-bit systems, meaning it cannot represent the size of an object larger than ~2 GB.

For a small project the difference is never observable in practice, but using `size_t` eliminates signed/unsigned comparison warnings from the compiler and matches the convention of the entire C standard library.

## 9. Naming Conventions

| Thing | Convention | Example |
|---|---|---|
| Types | `PascalCase` | `LinkedList`, `ListNode` |
| Functions | `module_verb[_noun]` | `list_push_back`, `list_is_empty` |
| Macros / include guards | `UPPER_SNAKE_CASE` | `LINKED_LIST_H`, `MAX_PILHA` |
| Parameters / locals | `lower_snake_case` | `index`, `out` |

The `module_` prefix on every function (`list_`, `dlist_`) serves as a namespace. C has no namespaces, so without a prefix, a function named `push_back` in one library would collide with a function of the same name in another. The prefix makes the origin of every function obvious and prevents linker errors.

## 10. Doxygen Documentation

Doxygen reads specially formatted comments and generates HTML or PDF documentation. The comment style used here:

```c
/**
 * @brief One-sentence summary.
 *
 * @param list   Description of the list parameter.
 * @param index  Position to operate on (0-based).
 * @return       What the function returns and under what conditions.
 */
```

Key tags:

| Tag | Purpose |
|---|---|
| `@brief` | One-line summary shown in index pages. |
| `@param name` | Documents a single parameter. |
| `@return` | Documents the return value. |
| `@note` | Highlights a non-obvious constraint. |
| `@p name` | Renders `name` in monospace, used to reference a parameter in prose. |
| `@c token` | Renders `token` in monospace, used for types or keywords. |
| `@code` / `@endcode` | Wraps a code block in the generated output. |

Doxygen is run from the command line after creating a `Doxyfile` configuration:

```sh
doxygen -g          # generate default Doxyfile
doxygen Doxyfile    # generate docs/html/index.html
```

## 11. Inline Member Documentation

```c
typedef struct {
    ListNode *head; /**< First node, or NULL when empty. */
    ListNode *tail; /**< Last node, or NULL when empty.  */
    size_t    size; /**< Current number of nodes.        */
} LinkedList;
```

`/**< ... */` (note the `<`) is the Doxygen syntax for documenting the member immediately to its **left**. Regular `/** ... */` documents the element immediately **below** it.

This keeps the documentation physically close to the field it describes, making the struct self-contained without requiring a separate comment block.
