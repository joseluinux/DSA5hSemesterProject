#include "linked_list.h"
#include <stdlib.h>

/*
Helpers
*/

/** @brief Allocate and initialise a single node. Returns NULL on failure. */
static ListNode *node_create(int data)
{
    ListNode *node = malloc(sizeof(ListNode));
    if (!node) return NULL;
    (*node).data = data;
    (*node).next = NULL;
    return node;
}

/*
Lifecycle
*/

/** @brief Allocate and zero-initialise the list struct. O(1). */
LinkedList *list_create(void)
{
    LinkedList *list = malloc(sizeof(LinkedList));
    if (!list) return NULL;
    (*list).head = NULL;
    (*list).tail = NULL;
    (*list).size = 0;
    return list;
}

/** @brief Walk the list freeing every node, then free the struct. O(n). */
void list_destroy(LinkedList *list)
{
    if (!list) return;
    ListNode *curr = (*list).head;
    while (curr) {
        ListNode *next = (*curr).next;
        free(curr);
        curr = next;
    }
    free(list);
}

/*
Insertion
*/

/** @brief Wire the new node in front of head. O(1). */
int list_push_front(LinkedList *list, int data)
{
    if (!list) return 0;
    ListNode *node = node_create(data);
    if (!node) return 0;

    (*node).next = (*list).head;
    (*list).head = node;
    if (!(*list).tail) (*list).tail = node;
    (*list).size++;
    return 1;
}

/** @brief Wire the new node after tail. O(1). */
int list_push_back(LinkedList *list, int data)
{
    if (!list) return 0;
    ListNode *node = node_create(data);
    if (!node) return 0;

    if (!(*list).tail) {
        (*list).head = node;
        (*list).tail = node;
    } else {
        (*(*list).tail).next = node;
        (*list).tail = node;
    }
    (*list).size++;
    return 1;
}

/** @brief Walk to index-1 then splice the new node in. O(n). */
int list_insert_at(LinkedList *list, size_t index, int data)
{
    if (!list || index > (*list).size) return 0;
    if (index == 0)            return list_push_front(list, data);
    if (index == (*list).size) return list_push_back(list, data);

    ListNode *node = node_create(data);
    if (!node) return 0;

    ListNode *curr = (*list).head;
    for (size_t i = 0; i < index - 1; i++)
        curr = (*curr).next;

    (*node).next = (*curr).next;
    (*curr).next = node;
    (*list).size++;
    return 1;
}

/** @brief Walk until the first node greater than data, then splice in. O(n). */
int list_insert_ordered(LinkedList *list, int data)
{
    if (!list) return 0;

    ListNode *curr = (*list).head;
    ListNode *prev = NULL;
    while (curr && (*curr).data <= data) {
        prev = curr;
        curr = (*curr).next;
    }

    ListNode *node = node_create(data);
    if (!node) return 0;

    (*node).next = curr;
    if (!prev) (*list).head = node;
    else       (*prev).next = node;
    if (!curr) (*list).tail = node;

    (*list).size++;
    return 1;
}

/*
Removal
*/

/** @brief Unlink head and return its value. O(1). */
int list_pop_front(LinkedList *list)
{
    ListNode *node = (*list).head;
    int data = (*node).data;

    (*list).head = (*node).next;
    if (!(*list).head) (*list).tail = NULL;
    free(node);
    (*list).size--;
    return data;
}

/** @brief Walk to the second-to-last node to unlink tail. O(n). */
int list_pop_back(LinkedList *list)
{
    if ((*list).head == (*list).tail) {
        int data = (*(*list).tail).data;
        free((*list).tail);
        (*list).head = NULL;
        (*list).tail = NULL;
        (*list).size--;
        return data;
    }

    ListNode *curr = (*list).head;
    while ((*curr).next != (*list).tail)
        curr = (*curr).next;

    int data = (*(*list).tail).data;
    free((*list).tail);
    (*list).tail = curr;
    (*(*list).tail).next = NULL;
    (*list).size--;
    return data;
}

/** @brief Walk to index-1, unlink the node, write value to out. O(n). */
int list_remove_at(LinkedList *list, size_t index, int *out)
{
    if (!list || index >= (*list).size) return 0;

    if (index == 0) {
        int val = list_pop_front(list);
        if (out) *out = val;
        return 1;
    }

    ListNode *prev = (*list).head;
    for (size_t i = 0; i < index - 1; i++)
        prev = (*prev).next;

    ListNode *node = (*prev).next;
    (*prev).next = (*node).next;
    if (!(*node).next) (*list).tail = prev;

    if (out) *out = (*node).data;
    free(node);
    (*list).size--;
    return 1;
}

/*
Access
*/

/** @brief Direct read from head. O(1). */
int list_peek_front(const LinkedList *list)
{
    return (*(*list).head).data;
}

/** @brief Direct read from tail. O(1). */
int list_peek_back(const LinkedList *list)
{
    return (*(*list).tail).data;
}

/** @brief Walk to index and return the value. O(n). */
int list_get(const LinkedList *list, size_t index)
{
    ListNode *curr = (*list).head;
    for (size_t i = 0; i < index; i++)
        curr = (*curr).next;
    return (*curr).data;
}

/*
Search
*/

/** @brief Linear scan comparing each node's value. O(n). */
ListNode *list_find(const LinkedList *list, int value)
{
    if (!list) return NULL;
    ListNode *curr = (*list).head;
    while (curr) {
        if ((*curr).data == value) return curr;
        curr = (*curr).next;
    }
    return NULL;
}

/*
Utilities
*/

/** @brief Return the cached size field. O(1). */
size_t list_size(const LinkedList *list)
{
    return list ? (*list).size : 0;
}

/** @brief Check if size is zero. O(1). */
int list_is_empty(const LinkedList *list)
{
    return !list || (*list).size == 0;
}

/**
 * @brief Reverse next pointers iteratively, then swap head and tail. O(n).
 *        Saves the old head as the new tail before the loop overwrites it.
 */
void list_reverse(LinkedList *list)
{
    if (!list || (*list).size <= 1) return;

    (*list).tail = (*list).head;

    ListNode *prev = NULL;
    ListNode *curr = (*list).head;
    while (curr) {
        ListNode *next = (*curr).next;
        (*curr).next = prev;
        prev = curr;
        curr = next;
    }
    (*list).head = prev;
}
