/*
 * test_stack.c
 * Test application for all Stack operations defined in stack.h / stack.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../Include/stack.h"

static int tests_run    = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { printf("  [TEST] %-40s ", name); tests_run++; } while (0)

#define PASS() \
    do { printf("PASSED\n"); tests_passed++; } while (0)

#define FAIL(msg) \
    do { printf("FAILED: %s\n", msg); } while (0)

/* ------------------------------------------------------------------ */

static void test_init(void) {
    TEST("stack_init sets stack to empty");
    Stack s;
    stack_init(&s);
    if (stack_is_empty(&s))
        PASS();
    else
        FAIL("stack should be empty after init");
}

static void test_push_single(void) {
    TEST("stack_push single element");
    Stack s;
    stack_init(&s);
    Position p = {3, 7};
    int ok = stack_push(&s, p);
    if (ok && !stack_is_empty(&s))
        PASS();
    else
        FAIL("push should succeed and stack should not be empty");
}

static void test_pop_single(void) {
    TEST("stack_pop returns pushed element");
    Stack s;
    stack_init(&s);
    Position p = {5, 10};
    stack_push(&s, p);
    Position out = stack_pop(&s);
    if (out.row == 5 && out.col == 10 && stack_is_empty(&s))
        PASS();
    else
        FAIL("popped position should match pushed position");
}

static void test_peek(void) {
    TEST("stack_peek returns top without removing");
    Stack s;
    stack_init(&s);
    Position p = {2, 4};
    stack_push(&s, p);
    Position top = stack_peek(&s);
    if (top.row == 2 && top.col == 4 && !stack_is_empty(&s))
        PASS();
    else
        FAIL("peek should return top and leave stack unchanged");
}

static void test_lifo_order(void) {
    TEST("stack maintains LIFO order");
    Stack s;
    stack_init(&s);
    Position a = {1, 1}, b = {2, 2}, c = {3, 3};
    stack_push(&s, a);
    stack_push(&s, b);
    stack_push(&s, c);

    Position p3 = stack_pop(&s);
    Position p2 = stack_pop(&s);
    Position p1 = stack_pop(&s);

    if (p3.row == 3 && p2.row == 2 && p1.row == 1)
        PASS();
    else
        FAIL("elements should come out in reverse order");
}

static void test_is_empty_on_new_stack(void) {
    TEST("stack_is_empty on freshly initialized stack");
    Stack s;
    stack_init(&s);
    if (stack_is_empty(&s))
        PASS();
    else
        FAIL("new stack should be empty");
}

static void test_is_empty_after_push(void) {
    TEST("stack_is_empty returns 0 after push");
    Stack s;
    stack_init(&s);
    Position p = {0, 0};
    stack_push(&s, p);
    if (!stack_is_empty(&s))
        PASS();
    else
        FAIL("stack with one element should not be empty");
}

static void test_is_empty_after_push_pop(void) {
    TEST("stack_is_empty after push then pop");
    Stack s;
    stack_init(&s);
    Position p = {0, 0};
    stack_push(&s, p);
    stack_pop(&s);
    if (stack_is_empty(&s))
        PASS();
    else
        FAIL("stack should be empty after popping only element");
}

static void test_is_full(void) {
    TEST("stack_is_full at MAX_STACK elements");
    Stack s;
    stack_init(&s);
    for (int i = 0; i < MAX_STACK; i++) {
        Position p = {i / MAX_COLS, i % MAX_COLS};
        stack_push(&s, p);
    }
    if (stack_is_full(&s))
        PASS();
    else
        FAIL("stack should be full after MAX_STACK pushes");
}

static void test_push_overflow(void) {
    TEST("stack_push returns 0 on overflow");
    Stack s;
    stack_init(&s);
    for (int i = 0; i < MAX_STACK; i++) {
        Position p = {0, 0};
        stack_push(&s, p);
    }
    Position extra = {99, 99};
    int ok = stack_push(&s, extra);
    if (!ok)
        PASS();
    else
        FAIL("push to full stack should return 0");
}

static void test_pop_empty(void) {
    TEST("stack_pop on empty stack returns sentinel");
    Stack s;
    stack_init(&s);
    Position out = stack_pop(&s);
    if (out.row == -1 && out.col == -1)
        PASS();
    else
        FAIL("pop on empty stack should return {-1, -1}");
}

static void test_peek_empty(void) {
    TEST("stack_peek on empty stack returns sentinel");
    Stack s;
    stack_init(&s);
    Position out = stack_peek(&s);
    if (out.row == -1 && out.col == -1)
        PASS();
    else
        FAIL("peek on empty stack should return {-1, -1}");
}

static void test_free_resets(void) {
    TEST("stack_free resets stack to empty");
    Stack s;
    stack_init(&s);
    Position p = {1, 1};
    stack_push(&s, p);
    stack_push(&s, p);
    stack_free(&s);
    if (stack_is_empty(&s))
        PASS();
    else
        FAIL("stack should be empty after free");
}

static void test_reuse_after_free(void) {
    TEST("stack is reusable after stack_free");
    Stack s;
    stack_init(&s);
    Position a = {10, 20};
    stack_push(&s, a);
    stack_free(&s);

    Position b = {30, 40};
    stack_push(&s, b);
    Position out = stack_pop(&s);
    if (out.row == 30 && out.col == 40 && stack_is_empty(&s))
        PASS();
    else
        FAIL("stack should work normally after free + new push");
}

/* ------------------------------------------------------------------ */

int main(void) {
    printf("========================================\n");
    printf("  Stack Test Suite\n");
    printf("========================================\n\n");

    test_init();
    test_push_single();
    test_pop_single();
    test_peek();
    test_lifo_order();
    test_is_empty_on_new_stack();
    test_is_empty_after_push();
    test_is_empty_after_push_pop();
    test_is_full();
    test_push_overflow();
    test_pop_empty();
    test_peek_empty();
    test_free_resets();
    test_reuse_after_free();

    printf("\n========================================\n");
    printf("  Results: %d / %d passed\n", tests_passed, tests_run);
    printf("========================================\n");

    return (tests_passed == tests_run) ? EXIT_SUCCESS : EXIT_FAILURE;
}
