#ifndef TP2_SO_FIFO
#define TP2_SO_FIFO

typedef struct {
    short frame_addr;
    char dirty;
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

#endif