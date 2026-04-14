/**
 * @file linked_list.h
 * @brief Singly linked list of integers.
 *
 * Insertion functions return 1 on success, 0 on failure.
 * Access and removal functions (pop, peek, get) must only be called
 * on a non-empty list — use list_is_empty() first.
 * list_find() and list_remove_at() return NULL / 0 on failure.
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h> /* size_t */

typedef struct ListNode {
    int data;
    struct ListNode *next;
} ListNode;

typedef struct {
    ListNode *head;
    ListNode *tail;
    size_t size;
} LinkedList;

/** @brief Allocate an empty list. Returns NULL on allocation failure. */
LinkedList *list_create(void);
/** @brief Destroy the list, freeing all nodes. */
void list_destroy(LinkedList *list);

/*
Insertion
*/

/** @brief Prepend an element. O(1). Returns 1 on success, 0 on failure. */
int list_push_front(LinkedList *list, int data);
/** @brief Append an element. O(1). Returns 1 on success, 0 on failure. */
int list_push_back(LinkedList *list, int data);
/**
 * @brief Insert before the node at position index. O(n).
 *        index == list->size is equivalent to list_push_back().
 */
int list_insert_at(
    LinkedList *list,
    size_t index,
    int data
);
/** @brief Insert keeping the list sorted in ascending order. O(n). */
int list_insert_ordered(LinkedList *list, int data);

/*
Removal
*/

/** @brief Remove and return the front element. O(1). List must not be empty. */
int list_pop_front(LinkedList *list);
/** @brief Remove and return the back element. O(n). List must not be empty. */
int list_pop_back(LinkedList *list);
/**
 * @brief Remove the element at index and return 1 on success, 0 on error. O(n).
 *        The removed value is written to out if out is not NULL.
 */
int list_remove_at(
    LinkedList *list,
    size_t index,
    int *out
);

/*
Access
*/

/** @brief Return the front element. O(1). List must not be empty. */
int list_peek_front(const LinkedList *list);
/** @brief Return the back element. O(1). List must not be empty. */
int list_peek_back(const LinkedList *list);
/** @brief Return the element at index. O(n). List must have at least index+1 elements. */
int list_get(const LinkedList *list, size_t index);

/*
Search
*/

/**
 * @brief Find the first node with the given value. O(n).
 *        Returns NULL if not found. Do not free the returned node directly.
 */
ListNode *list_find(const LinkedList *list, int value);

/*
Utilities
*/

/** @brief Return the number of elements. O(1). */
size_t list_size(const LinkedList *list);
/** @brief Return 1 if the list has no elements, 0 otherwise. O(1). */
int list_is_empty(const LinkedList *list);
/** @brief Reverse the list in-place. O(n). */
void list_reverse(LinkedList *list);

#endif /* LINKED_LIST_H */
