#ifndef HEAP_H
#define HEAP_H

/* Binary min-heap ordered by priority (ascending).
 * For max-heap behaviour use negative priorities. */

typedef struct {
    int pos;       /* maze cell index */
    int priority;  /* ordering key — lower = higher priority */
    int g;         /* accumulated cost from start (steps or treasure) */
    int parent;    /* parent cell index, -1 at start */
} HeapNode;

typedef struct {
    HeapNode *data;
    int       size;
    int       capacity;
} Heap;

void     heap_init(Heap *h);
void     heap_free(Heap *h);
void     heap_push(Heap *h, HeapNode node);
HeapNode heap_pop(Heap *h);
int      heap_is_empty(const Heap *h);

#endif /* HEAP_H */
