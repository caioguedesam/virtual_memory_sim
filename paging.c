#include <stdio.h>
#include <stdlib.h>
#include "paging.h"

queue* init_queue() {
    queue* q = (queue*)malloc(sizeof(queue));
    q->first = NULL;
    q->last = NULL;
    return q;
}

void delete_queue(queue *q) {
    queue_element* current = q->first;
    queue_element* tmp;
    while(current != NULL) {
        tmp = current->next;
        free(current);
        current = tmp;
    }
    free(q);
}

void push_queue(queue *q, unsigned i) {
    queue_element* element = (queue_element*)malloc(sizeof(queue_element));
    element->page_addr = i;
    element->next = NULL;

    if(q->first == NULL) {
        q->first = element;
    }
    else {
        q->last->next = element;
    }
    q->last = element;
}

unsigned pop_queue(queue *q) {
    if(q->first == NULL) {
        return -1;
    }
    queue_element* element = q->first;
    q->first = element->next;
    unsigned page_addr = element->page_addr;
    free(element);
    return page_addr;
}

unsigned get_lru(page_table_entry* page_table, unsigned* frame_table, unsigned frame_count) {
    unsigned lru = frame_table[0];
    for(unsigned i = 0; i < frame_count; i++) {
        unsigned page = frame_table[i];
        if(page_table[page].timestamp < page_table[lru].timestamp)
            lru = page;
    }
    return lru;
}