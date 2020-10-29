/**
 * Module queue
 *
 * This module provides a simple queue (FIFO) data structure and functions
 * operating on it.
 *
 * @author   Alexandre Blondin Massé
 * @version  1.0
 * @date     June 18th, 2017
 */
#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "map_graph.h"

// --------------- //
// Data structures //
// --------------- //

struct QueueContent {         // Information contained in the queue
    struct MapCellNode *cell; // The node in the graph
    unsigned int priority;    // The priority associated with the cell
};

struct QueueNode {                // A node in the queue
    struct QueueContent *content; // The content of this node
    struct QueueNode *previous;   // The previous node in the queue
    struct QueueNode *next;       // The next node in the queue
};

typedef struct {             // A simple queue (FIFO)
    struct QueueNode *first; // The first node in the queue
    struct QueueNode *last;  // The last node in the queue
    unsigned int numNodes;   // The number of nodes in the queue
} Queue;

// --------- //
// Functions //
// --------- //

/**
 * Creates an empty queue.
 *
 * @return  The queue
 */
Queue queue_create();

/**
 * Destroys the given queue.
 *
 * @param  The queue to be destroyed
 */
void queue_delete(Queue *queue);

/**
 * Returns true if the given queue is empty.
 *
 * @param queue  The queue to be checked
 * @return       True if the queue is empty
 */
bool queue_isEmpty(const Queue *queue);

/**
 * Appends a new cell to the queue with given priority.
 *
 * @param queue     The queue to which the cell is appended
 * @param cell      The cell to be appended
 * @param priority  The priority to give to the cell
 */
void queue_enqueue(Queue *queue,
                   struct MapCellNode *cell,
                   unsigned int priority);

/**
 * Removes the first element in the queue and returns it.
 *
 * @param queue  The queue from which the element is removed
 * @return       The first element of the queue
 */
struct QueueContent queue_dequeue(Queue *queue);

/**
 * Prints the given queue.
 *
 * @param queue  The queue to be printed
 */
void queue_print(const Queue *queue);

#endif
