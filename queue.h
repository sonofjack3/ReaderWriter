#ifndef _QUEUE_
#define _QUEUE_

/* Written by Evan Jackson */
/* Defines queue data and operations */

#include <stdlib.h>
#include <stdio.h>
#include "queue_set_data.h"

/* Struct representing a Queue node (stores an integer)*/
typedef struct Node
{
    int item;
	int read_or_write; //value: READER or WRITER
    struct Node* next;
} Node;

/* Struct representing a FIFO queue of Nodes */
typedef struct Queue
{
    Node* head;
    Node* tail;
    void (*queue_push) (struct Queue*, int item, int read_or_write);
    Node* (*queue_pop) (struct Queue*);
	int (*queue_is_empty) (struct Queue*);
	void (*queue_display) (struct Queue*);
    int size;
} Queue;

void queue_push(Queue* queue, int item, int read_or_write); 
Node* queue_pop(Queue* queue);
void queue_display(Queue* queue);
int queue_is_empty(Queue* queue);
Queue create_queue();

#endif
