// A structure to represent a queue
typedef struct {
    int head; 
    int tail;
    int size;
    int capacity;
    int* array;
} Queue;

Queue* queue_init(int capacity);
int dequeue(Queue* queue);
void enqueue(Queue* queue, int item);
int is_empty(Queue* queue);
int is_full(Queue* queue);
