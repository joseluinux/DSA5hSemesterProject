#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>

/*
Print helpers
*/

static void print_list(const LinkedList *list)
{
    if (list_is_empty(list)) {
        printf("  List: (empty)\n");
        return;
    }
    printf("  List: ");
    ListNode *curr = (*list).head;
    while (curr) {
        printf("[ %d ]", (*curr).data);
        if ((*curr).next) printf(" -> ");
        curr = (*curr).next;
    }
    printf("\n");
}

static int read_int(const char *prompt)
{
    int val;
    printf("  %s", prompt);
    scanf("%d", &val);
    return val;
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
    LinkedList *list = list_create();
    int choice;

    do {
        clear_screen();
        printf("=== Singly Linked List ===\n");
        print_list(list);
        printf("  size: %zu\n\n", list_size(list));

        printf("  1. push_front      7. remove_at\n");
        printf("  2. push_back       8. peek_front\n");
        printf("  3. insert_at       9. peek_back\n");
        printf("  4. insert_ordered 10. get\n");
        printf("  5. pop_front      11. find\n");
        printf("  6. pop_back       12. reverse\n");
        printf("  13. clear          0. exit\n\n");
        printf("  > ");
        scanf("%d", &choice);
        printf("\n");

        int val, idx, result;
        ListNode *node;

        switch (choice) {
            case 1:
                val = read_int("value: ");
                result = list_push_front(list, val);
                printf("  push_front(%d) -> %s\n", val, result ? "ok" : "failed");
                break;
            case 2:
                val = read_int("value: ");
                result = list_push_back(list, val);
                printf("  push_back(%d) -> %s\n", val, result ? "ok" : "failed");
                break;
            case 3:
                idx = read_int("index: ");
                val = read_int("value: ");
                result = list_insert_at(list, (size_t)idx, val);
                printf("  insert_at(%d, %d) -> %s\n", idx, val, result ? "ok" : "failed");
                break;
            case 4:
                val = read_int("value: ");
                result = list_insert_ordered(list, val);
                printf("  insert_ordered(%d) -> %s\n", val, result ? "ok" : "failed");
                break;
            case 5:
                if (list_is_empty(list)) { printf("  list is empty\n"); break; }
                val = list_pop_front(list);
                printf("  pop_front() = %d\n", val);
                break;
            case 6:
                if (list_is_empty(list)) { printf("  list is empty\n"); break; }
                val = list_pop_back(list);
                printf("  pop_back() = %d\n", val);
                break;
            case 7:
                idx = read_int("index: ");
                result = list_remove_at(list, (size_t)idx, &val);
                if (result) printf("  remove_at(%d) = %d\n", idx, val);
                else        printf("  remove_at(%d) -> failed\n", idx);
                break;
            case 8:
                if (list_is_empty(list)) { printf("  list is empty\n"); break; }
                printf("  peek_front() = %d\n", list_peek_front(list));
                break;
            case 9:
                if (list_is_empty(list)) { printf("  list is empty\n"); break; }
                printf("  peek_back() = %d\n", list_peek_back(list));
                break;
            case 10:
                idx = read_int("index: ");
                if ((size_t)idx >= list_size(list)) { printf("  index out of bounds\n"); break; }
                printf("  get(%d) = %d\n", idx, list_get(list, (size_t)idx));
                break;
            case 11:
                val = read_int("value: ");
                node = list_find(list, val);
                printf("  find(%d) -> %s\n", val, node ? "found" : "not found");
                break;
            case 12:
                list_reverse(list);
                printf("  list reversed\n");
                break;
            case 13:
                list_destroy(list);
                list = list_create();
                printf("  list cleared\n");
                break;
            case 0:
                break;
            default:
                printf("  invalid option\n");
        }

        if (choice != 0) {
            printf("\n");
            print_list(list);
            printf("\n  press enter to continue...");
            getchar(); getchar();
        }

    } while (choice != 0);

    list_destroy(list);
    clear_screen();
    return 0;
}
