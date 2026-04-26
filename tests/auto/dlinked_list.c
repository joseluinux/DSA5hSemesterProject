#include "dlinked_list.h"
#include <stdio.h>

static int passed = 0;
static int failed = 0;

#define CHECK(cond, msg) \
    do { \
        if (cond) { printf("  PASS  %s\n", msg); passed++; } \
        else      { printf("  FAIL  %s\n", msg); failed++; } \
    } while (0)

static void section(const char *title)
{
    printf("\n%s\n", title);
}

int main(void)
{
    printf("=== Doubly Linked List ===\n");
    DLinkedList *list = dlist_create();

    section("Lifecycle");
    CHECK(list != NULL,           "dlist_create returns non-NULL");
    CHECK(dlist_is_empty(list),   "new list is empty");
    CHECK(dlist_size(list) == 0,  "new list has size 0");

    section("push_back / push_front");
    CHECK(dlist_push_back(list, 10),          "push_back(10) succeeds");
    CHECK(dlist_push_back(list, 20),          "push_back(20) succeeds");
    CHECK(dlist_push_front(list, 5),          "push_front(5) succeeds");
    CHECK(dlist_size(list) == 3,              "size is 3 after 3 insertions");
    CHECK(dlist_peek_front(list) == 5,        "peek_front is 5");
    CHECK(dlist_peek_back(list) == 20,        "peek_back is 20");

    section("insert_before / insert_after");
    /* list: [ 5, 10, 20 ] */
    DListNode *mid = dlist_find(list, 10);
    CHECK(dlist_insert_before(list, mid, 7),  "insert_before(10, 7) succeeds");
    CHECK(dlist_get(list, 1) == 7,            "get(1) == 7");
    CHECK(dlist_insert_after(list, mid, 15),  "insert_after(10, 15) succeeds");
    CHECK(dlist_get(list, 3) == 15,           "get(3) == 15");
    CHECK(dlist_size(list) == 5,              "size is 5");
    /* list: [ 5, 7, 10, 15, 20 ] */

    section("insert_ordered");
    DLinkedList *ord = dlist_create();
    dlist_insert_ordered(ord, 30);
    dlist_insert_ordered(ord, 10);
    dlist_insert_ordered(ord, 20);
    dlist_insert_ordered(ord,  5);
    dlist_insert_ordered(ord, 40);
    CHECK(dlist_get(ord, 0) ==  5,            "ordered: index 0 ==  5");
    CHECK(dlist_get(ord, 1) == 10,            "ordered: index 1 == 10");
    CHECK(dlist_get(ord, 2) == 20,            "ordered: index 2 == 20");
    CHECK(dlist_get(ord, 3) == 30,            "ordered: index 3 == 30");
    CHECK(dlist_get(ord, 4) == 40,            "ordered: index 4 == 40");
    dlist_destroy(ord);

    section("pop_front / pop_back (O(1))");
    /* list: [ 5, 7, 10, 15, 20 ] */
    CHECK(dlist_pop_front(list) == 5,         "pop_front == 5");
    CHECK(dlist_size(list) == 4,              "size decreases after pop_front");
    CHECK(dlist_pop_back(list) == 20,         "pop_back == 20");
    CHECK(dlist_size(list) == 3,              "size decreases after pop_back");
    CHECK(dlist_peek_front(list) == 7,        "new front is 7");
    CHECK(dlist_peek_back(list) == 15,        "new back is 15");

    section("remove_at / remove_node");
    /* list: [ 7, 10, 15 ] */
    int val;
    CHECK(dlist_remove_at(list, 1, &val),     "remove_at(1) succeeds");
    CHECK(val == 10,                          "removed value == 10");
    CHECK(dlist_size(list) == 2,              "size decreases after remove_at");

    dlist_push_back(list, 50);
    dlist_push_back(list, 60);
    /* list: [ 7, 15, 50, 60 ] */
    DListNode *target = dlist_find(list, 50);
    CHECK(dlist_remove_node(list, target, &val), "remove_node(50) succeeds");
    CHECK(val == 50,                             "removed value == 50");
    CHECK(dlist_size(list) == 3,                 "size decreases after remove_node");

    section("find / find_reverse");
    CHECK(dlist_find(list, 15) != NULL,          "find(15) returns node");
    CHECK(dlist_find(list, 99) == NULL,          "find(99) returns NULL");
    CHECK(dlist_find_reverse(list, 60) != NULL,  "find_reverse(60) returns node");
    CHECK(dlist_find_reverse(list, 99) == NULL,  "find_reverse(99) returns NULL");

    section("reverse");
    /* list: [ 7, 15, 60 ] */
    dlist_reverse(list);
    CHECK(dlist_peek_front(list) == 60,       "after reverse: front == 60");
    CHECK(dlist_peek_back(list) == 7,         "after reverse: back  ==  7");
    CHECK(dlist_get(list, 1) == 15,           "after reverse: middle == 15");

    dlist_destroy(list);

    printf("\n----------------------------------------\n");
    printf("Results: %d passed, %d failed\n", passed, failed);
    printf("----------------------------------------\n");

    return failed > 0 ? 1 : 0;
}
