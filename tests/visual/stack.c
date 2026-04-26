#include "stack.h"
#include <stdio.h>

/*
Print helpers
*/

static void print_stack(Stack *s)
{
    if (stack_is_empty(s)) {
        printf("  Stack: (empty)\n");
        return;
    }
    printf("  Stack (top -> bottom):\n");
    for (int i = (*s).top; i >= 0; i--)
        printf("    (%d, %d)\n", (*s).items[i].row, (*s).items[i].col);
}

static void clear_screen(void)
{
    printf("\033[2J\033[H");
}

/*
Entry point
*/

int main(void)
{
    Stack s;
    stack_init(&s);
    int choice;

    do {
        clear_screen();
        printf("=== Stack ===\n");
        print_stack(&s);
        printf("  size: %d\n\n", s.top + 1);

        printf("  1. push (row, col)\n");
        printf("  2. pop\n");
        printf("  3. peek\n");
        printf("  4. is_empty\n");
        printf("  5. is_full\n");
        printf("  6. clear\n");
        printf("  0. exit\n\n");
        printf("  > ");
        scanf("%d", &choice);
        printf("\n");

        int r, c;
        Position p, out;

        switch (choice) {
            case 1:
                printf("  row: "); scanf("%d", &r);
                printf("  col: "); scanf("%d", &c);
                p.row = r; p.col = c;
                if (stack_push(&s, p))
                    printf("  push(%d, %d) -> ok\n", r, c);
                else
                    printf("  push(%d, %d) -> failed (stack full)\n", r, c);
                break;
            case 2:
                out = stack_pop(&s);
                if (out.row == -1 && out.col == -1)
                    printf("  pop() -> stack was empty\n");
                else
                    printf("  pop() = (%d, %d)\n", out.row, out.col);
                break;
            case 3:
                out = stack_peek(&s);
                if (out.row == -1 && out.col == -1)
                    printf("  peek() -> stack is empty\n");
                else
                    printf("  peek() = (%d, %d)\n", out.row, out.col);
                break;
            case 4:
                printf("  is_empty() = %s\n", stack_is_empty(&s) ? "yes" : "no");
                break;
            case 5:
                printf("  is_full() = %s\n", stack_is_full(&s) ? "yes" : "no");
                break;
            case 6:
                stack_free(&s);
                stack_init(&s);
                printf("  stack cleared\n");
                break;
            case 0:
                break;
            default:
                printf("  invalid option\n");
        }

        if (choice != 0) {
            printf("\n");
            print_stack(&s);
            printf("\n  press enter to continue...");
            getchar(); getchar();
        }

    } while (choice != 0);

    clear_screen();
    return 0;
}
