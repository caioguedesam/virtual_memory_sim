#ifndef TP2_SO_FIFO
#define TP2_SO_FIFO

typedef struct {
    unsigned page_number;
    unsigned* next;
} queue_element;

typedef struct {
    queue_element* first;
    queue_element* last;
} queue;

void push(queue* q);
unsigned pop(queue *q);

#endif