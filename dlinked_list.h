/**
 * @file dlinked_list.h
 * @brief Doubly linked list of integers with O(1) front and back operations.
 *
 * Insertion functions return 1 on success, 0 on failure.
 * Access and removal functions (pop, peek, get) must only be called
 * on a non-empty list — use dlist_is_empty() first.
 * dlist_find() returns NULL on failure. dlist_remove_at() and
 * dlist_remove_node() return 1 on success, 0 on failure.
 */

#ifndef DLINKED_LIST_H
#define DLINKED_LIST_H

#include <stddef.h> /* size_t */

typedef struct DListNode {
    int data;
    struct DListNode *prev;
    struct DListNode *next;
} DListNode;

typedef struct {
    DListNode *head;
    DListNode *tail;
    size_t size;
} DLinkedList;

/** @brief Allocate an empty list. Returns NULL on allocation failure. */
DLinkedList *dlist_create(void);
/** @brief Destroy the list, freeing all nodes. */
void dlist_destroy(DLinkedList *list);

/*
Insertion
*/

/** @brief Prepend an element. O(1). Returns 1 on success, 0 on failure. */
int dlist_push_front(DLinkedList *list, int data);
/** @brief Append an element. O(1). Returns 1 on success, 0 on failure. */
int dlist_push_back(DLinkedList *list, int data);
/**
 * @brief Insert before the node at position index. O(n/2).
 *        index == list->size is equivalent to dlist_push_back().
 */
int dlist_insert_at(
    DLinkedList *list,
    size_t index,
    int data
);
/** @brief Insert immediately before node. O(1). Returns 1 on success, 0 on failure. */
int dlist_insert_before(
    DLinkedList *list,
    DListNode *node,
    int data
);
/** @brief Insert immediately after node. O(1). Returns 1 on success, 0 on failure. */
int dlist_insert_after(
    DLinkedList *list,
    DListNode *node,
    int data
);
/** @brief Insert keeping the list sorted in ascending order. O(n). */
int dlist_insert_ordered(DLinkedList *list, int data);

/*
Removal
*/

/** @brief Remove and return the front element. O(1). List must not be empty. */
int dlist_pop_front(DLinkedList *list);
/** @brief Remove and return the back element. O(1). List must not be empty. */
int dlist_pop_back(DLinkedList *list);
/**
 * @brief Remove the element at index and return 1 on success, 0 on error. O(n/2).
 *        The removed value is written to out if out is not NULL.
 */
int dlist_remove_at(
    DLinkedList *list,
    size_t index,
    int *out
);
/**
 * @brief Unlink node and return 1 on success, 0 on failure. O(1).
 *        node must belong to this list.
 *        The removed value is written to out if out is not NULL.
 */
int dlist_remove_node(
    DLinkedList *list,
    DListNode *node,
    int *out
);

/*
Access
*/

/** @brief Return the front element. O(1). List must not be empty. */
int dlist_peek_front(const DLinkedList *list);
/** @brief Return the back element. O(1). List must not be empty. */
int dlist_peek_back(const DLinkedList *list);
/** @brief Return the element at index. O(n/2). List must have at least index+1 elements. */
int dlist_get(const DLinkedList *list, size_t index);

/*
Search
*/

/**
 * @brief Find the first node (front to back) with the given value. O(n).
 *        Returns NULL if not found. Do not free the returned node directly.
 */
DListNode *dlist_find(const DLinkedList *list, int value);
/**
 * @brief Find the first node (back to front) with the given value. O(n).
 *        Returns NULL if not found.
 */
DListNode *dlist_find_reverse(const DLinkedList *list, int value);

/*
Utilities
*/

/** @brief Return the number of elements. O(1). */
size_t dlist_size(const DLinkedList *list);
/** @brief Return 1 if the list has no elements, 0 otherwise. O(1). */
int dlist_is_empty(const DLinkedList *list);
/** @brief Reverse the list in-place by relinking all nodes. O(n). */
void dlist_reverse(DLinkedList *list);

#endif /* DLINKED_LIST_H */
