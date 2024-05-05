#include <stdlib.h>
#include "queue.h"

Queue* queue_init(int capacity) {
    Queue* queue = (Queue*) malloc(sizeof(Queue));

    queue->capacity = capacity;
    queue->head = queue->size = 0;
 
    queue->tail = capacity - 1;
    queue->array = (int*)malloc(
        queue->capacity * sizeof(int));
    return queue;
}

int is_full(Queue* queue)
{
    return (queue->size == queue->capacity);
}

int is_empty(Queue* queue)
{
    return (queue->size == 0);
}

// add an item to the queue.
void enqueue(Queue* queue, int item)
{
    if (is_full(queue))
        return;

    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->array[queue->tail] = item;
    queue->size = queue->size + 1;
}
 
int dequeue(Queue* queue)
{
    if (is_empty(queue))
        return -1;

    int item = queue->array[queue->head];
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

