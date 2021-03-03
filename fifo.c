#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"

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