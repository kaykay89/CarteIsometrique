#include "queue.h"
#include <stdio.h>
#include <assert.h>

Queue queue_create() {
    Queue queue;
    queue.first = NULL;
    queue.last = NULL;
    queue.numNodes = 0;
    return queue;
}

void queue_delete(Queue *queue) {
    while (queue->numNodes != 0) {
        queue_dequeue(queue);
    }
}

bool queue_isEmpty(const Queue *queue) {
    return queue->numNodes == 0;
}

void queue_enqueue(Queue *queue,
                   struct MapCellNode *cell,
                   unsigned int priority) {
    struct QueueNode *node =
        (struct QueueNode*)malloc(sizeof(struct QueueNode));
    node->content =
        (struct QueueContent*)malloc(sizeof(struct QueueContent));
    node->content->cell = cell;
    node->content->priority = priority;
    node->next = NULL;
    node->previous = queue->last;
    if (queue->numNodes == 0) {
        queue->first = node;
    } else {
        queue->last->next = node;
    }
    queue->last = node;
    ++queue->numNodes;
}

struct QueueContent queue_dequeue(Queue *queue) {
    assert(!queue_isEmpty(queue));
    struct QueueContent content = *(queue->first->content);
    free(queue->first->content);
    struct QueueNode *node = queue->first;
    queue->first = node->next;
    if (queue->numNodes > 1) {
        queue->first->previous = NULL;
    }
    free(node);
    --queue->numNodes;
    return content;
}

void queue_print(const Queue *queue) {
    printf("A queue of %d nodes: [ ", queue->numNodes);
    struct QueueNode *node;
    for (node = queue->first; node != NULL; node = node->next) {
        printf("Node(cell = (%d,%d,%d), priority = %d) ",
               node->content->cell->cell.row,
               node->content->cell->cell.column,
               node->content->cell->cell.layer,
               node->content->priority);
    }
    printf("]\n");
}
