#include "dlinked_list.h"
#include <stdlib.h>

/*
Helpers
*/

/** @brief Allocate and initialise a single node. Returns NULL on failure. */
static DListNode *node_create(int data)
{
    DListNode *node = malloc(sizeof(DListNode));
    if (!node) return NULL;
    (*node).data = data;
    (*node).prev = NULL;
    (*node).next = NULL;
    return node;
}

/*
Lifecycle
*/

/** @brief Allocate and zero-initialise the list struct. O(1). */
DLinkedList *dlist_create(void)
{
    DLinkedList *list = malloc(sizeof(DLinkedList));
    if (!list) return NULL;
    (*list).head = NULL;
    (*list).tail = NULL;
    (*list).size = 0;
    return list;
}

/** @brief Walk the list freeing every node, then free the struct. O(n). */
void dlist_destroy(DLinkedList *list)
{
    if (!list) return;
    DListNode *curr = (*list).head;
    while (curr) {
        DListNode *next = (*curr).next;
        free(curr);
        curr = next;
    }
    free(list);
}

/*
Insertion
*/

/** @brief Wire the new node in front of head, updating head->prev. O(1). */
int dlist_push_front(DLinkedList *list, int data)
{
    if (!list) return 0;
    DListNode *node = node_create(data);
    if (!node) return 0;

    (*node).next = (*list).head;
    if ((*list).head) (*(*list).head).prev = node;
    (*list).head = node;
    if (!(*list).tail) (*list).tail = node;
    (*list).size++;
    return 1;
}

/** @brief Wire the new node after tail, updating tail->next. O(1). */
int dlist_push_back(DLinkedList *list, int data)
{
    if (!list) return 0;
    DListNode *node = node_create(data);
    if (!node) return 0;

    (*node).prev = (*list).tail;
    if ((*list).tail) (*(*list).tail).next = node;
    (*list).tail = node;
    if (!(*list).head) (*list).head = node;
    (*list).size++;
    return 1;
}

/** @brief Walk from the closer end to index, then call insert_before. O(n/2). */
int dlist_insert_at(DLinkedList *list, size_t index, int data)
{
    if (!list || index > (*list).size) return 0;
    if (index == 0)             return dlist_push_front(list, data);
    if (index == (*list).size)  return dlist_push_back(list, data);

    DListNode *curr;
    if (index <= (*list).size / 2) {
        curr = (*list).head;
        for (size_t i = 0; i < index; i++)
            curr = (*curr).next;
    } else {
        curr = (*list).tail;
        for (size_t i = (*list).size - 1; i > index; i--)
            curr = (*curr).prev;
    }

    return dlist_insert_before(list, curr, data);
}

/** @brief Splice new node between node->prev and node. O(1). */
int dlist_insert_before(DLinkedList *list, DListNode *node, int data)
{
    if (!list || !node) return 0;
    if (node == (*list).head) return dlist_push_front(list, data);

    DListNode *new = node_create(data);
    if (!new) return 0;

    (*new).prev = (*node).prev;
    (*new).next = node;
    (*(*node).prev).next = new;
    (*node).prev = new;
    (*list).size++;
    return 1;
}

/** @brief Splice new node between node and node->next. O(1). */
int dlist_insert_after(DLinkedList *list, DListNode *node, int data)
{
    if (!list || !node) return 0;
    if (node == (*list).tail) return dlist_push_back(list, data);

    DListNode *new = node_create(data);
    if (!new) return 0;

    (*new).next = (*node).next;
    (*new).prev = node;
    (*(*node).next).prev = new;
    (*node).next = new;
    (*list).size++;
    return 1;
}

/** @brief Walk until the first node greater than data, then insert before it. O(n). */
int dlist_insert_ordered(DLinkedList *list, int data)
{
    if (!list) return 0;

    DListNode *curr = (*list).head;
    while (curr && (*curr).data <= data)
        curr = (*curr).next;

    if (!curr)                return dlist_push_back(list, data);
    if (curr == (*list).head) return dlist_push_front(list, data);
    return dlist_insert_before(list, curr, data);
}

/*
Removal
*/

/** @brief Unlink head and update the new head's prev to NULL. O(1). */
int dlist_pop_front(DLinkedList *list)
{
    DListNode *node = (*list).head;
    int data = (*node).data;

    (*list).head = (*node).next;
    if ((*list).head) (*(*list).head).prev = NULL;
    else              (*list).tail = NULL;
    free(node);
    (*list).size--;
    return data;
}

/** @brief Unlink tail and update the new tail's next to NULL. O(1). */
int dlist_pop_back(DLinkedList *list)
{
    DListNode *node = (*list).tail;
    int data = (*node).data;

    (*list).tail = (*node).prev;
    if ((*list).tail) (*(*list).tail).next = NULL;
    else              (*list).head = NULL;
    free(node);
    (*list).size--;
    return data;
}

/** @brief Walk from the closer end to index, then delegate to remove_node. O(n/2). */
int dlist_remove_at(DLinkedList *list, size_t index, int *out)
{
    if (!list || index >= (*list).size) return 0;

    if (index == 0) {
        int val = dlist_pop_front(list);
        if (out) *out = val;
        return 1;
    }
    if (index == (*list).size - 1) {
        int val = dlist_pop_back(list);
        if (out) *out = val;
        return 1;
    }

    DListNode *curr;
    if (index <= (*list).size / 2) {
        curr = (*list).head;
        for (size_t i = 0; i < index; i++)
            curr = (*curr).next;
    } else {
        curr = (*list).tail;
        for (size_t i = (*list).size - 1; i > index; i--)
            curr = (*curr).prev;
    }

    return dlist_remove_node(list, curr, out);
}

/** @brief Relink node's neighbours around it, then free. O(1). */
int dlist_remove_node(DLinkedList *list, DListNode *node, int *out)
{
    if (!list || !node) return 0;

    if ((*node).prev) (*(*node).prev).next = (*node).next;
    else              (*list).head = (*node).next;

    if ((*node).next) (*(*node).next).prev = (*node).prev;
    else              (*list).tail = (*node).prev;

    if (out) *out = (*node).data;
    free(node);
    (*list).size--;
    return 1;
}

/*
Access
*/

/** @brief Direct read from head. O(1). */
int dlist_peek_front(const DLinkedList *list)
{
    return (*(*list).head).data;
}

/** @brief Direct read from tail. O(1). */
int dlist_peek_back(const DLinkedList *list)
{
    return (*(*list).tail).data;
}

/** @brief Walk from the closer end to index and return the value. O(n/2). */
int dlist_get(const DLinkedList *list, size_t index)
{
    DListNode *curr;
    if (index <= (*list).size / 2) {
        curr = (*list).head;
        for (size_t i = 0; i < index; i++)
            curr = (*curr).next;
    } else {
        curr = (*list).tail;
        for (size_t i = (*list).size - 1; i > index; i--)
            curr = (*curr).prev;
    }
    return (*curr).data;
}

/*
Search
*/

/** @brief Linear scan from head comparing each node's value. O(n). */
DListNode *dlist_find(const DLinkedList *list, int value)
{
    if (!list) return NULL;
    DListNode *curr = (*list).head;
    while (curr) {
        if ((*curr).data == value) return curr;
        curr = (*curr).next;
    }
    return NULL;
}

/** @brief Linear scan from tail comparing each node's value. O(n). */
DListNode *dlist_find_reverse(const DLinkedList *list, int value)
{
    if (!list) return NULL;
    DListNode *curr = (*list).tail;
    while (curr) {
        if ((*curr).data == value) return curr;
        curr = (*curr).prev;
    }
    return NULL;
}

/*
Utilities
*/

/** @brief Return the cached size field. O(1). */
size_t dlist_size(const DLinkedList *list)
{
    return list ? (*list).size : 0;
}

/** @brief Check if size is zero. O(1). */
int dlist_is_empty(const DLinkedList *list)
{
    return !list || (*list).size == 0;
}

/**
 * @brief Swap each node's prev and next pointers, then swap head and tail. O(n).
 *        No data moves — only pointers are relinked.
 */
void dlist_reverse(DLinkedList *list)
{
    if (!list || (*list).size <= 1) return;

    DListNode *curr = (*list).head;
    while (curr) {
        DListNode *next = (*curr).next;
        (*curr).next = (*curr).prev;
        (*curr).prev = next;
        curr = next;
    }

    DListNode *tmp = (*list).head;
    (*list).head = (*list).tail;
    (*list).tail = tmp;
}
