#ifndef TP2_SO_FIFO
#define TP2_SO_FIFO

#include "const.h"

typedef struct {
    short frame_addr;
    char dirty;
    char chance;
    long timestamp;
} page_table_entry;

typedef struct queue_element {
    unsigned page_addr;
    struct queue_element* next;
} queue_element;

typedef struct {
    queue_element* first;
    queue_element* last;
} queue;

queue* init_queue();
void delete_queue(queue* q);
void push_queue(queue* q, unsigned i);
unsigned pop_queue(queue *q);

unsigned get_lru(page_table_entry* page_table, unsigned* frame_table, unsigned frame_count);

unsigned get_second_chance_next(page_table_entry* page_table, unsigned* frame_table, unsigned* second_chance_p, unsigned frame_count);

#endif