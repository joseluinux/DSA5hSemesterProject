#include <stdlib.h>
#include <stdio.h>
#include <heap.h>

#define INIT_CAP 64

void heap_init(Heap *h) {
    (*h).data     = malloc(INIT_CAP * sizeof(HeapNode));
    if (!(*h).data) { fprintf(stderr, "malloc failed\n"); exit(1); }
    (*h).size     = 0;
    (*h).capacity = INIT_CAP;
}

void heap_free(Heap *h) {
    free((*h).data);
    (*h).data     = NULL;
    (*h).size     = 0;
    (*h).capacity = 0;
}

static void swap_nodes(HeapNode *a, HeapNode *b) {
    HeapNode tmp = *a;
    *a = *b;
    *b = tmp;
}

void heap_push(Heap *h, HeapNode node) {
    if ((*h).size == (*h).capacity) {
        (*h).capacity *= 2;
        HeapNode *tmp = realloc((*h).data, (*h).capacity * sizeof(HeapNode));
        if (!tmp) { fprintf(stderr, "malloc failed\n"); exit(1); }
        (*h).data = tmp;
    }
    (*h).data[(*h).size] = node;
    int i = (*h).size++;
    /* Sift up */
    while (i > 0) {
        int p = (i - 1) / 2;
        if ((*h).data[p].priority <= (*h).data[i].priority) break;
        swap_nodes(&(*h).data[p], &(*h).data[i]);
        i = p;
    }
}

HeapNode heap_pop(Heap *h) {
    HeapNode top = (*h).data[0];
    (*h).data[0] = (*h).data[--(*h).size];
    /* Sift down */
    int i = 0;
    for (;;) {
        int l = 2*i + 1, r = 2*i + 2, s = i;
        if (l < (*h).size && (*h).data[l].priority < (*h).data[s].priority) s = l;
        if (r < (*h).size && (*h).data[r].priority < (*h).data[s].priority) s = r;
        if (s == i) break;
        swap_nodes(&(*h).data[i], &(*h).data[s]);
        i = s;
    }
    return top;
}

int heap_is_empty(const Heap *h) {
    return (*h).size == 0;
}
