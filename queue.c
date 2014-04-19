/* Written by Evan Jackson */
/* Defines operations for a queue of Nodes */

#include "queue.h"

/* Returns 1 if queue is empty, 0 otherwise */
int queue_is_empty (Queue* queue)
{
	if (queue->head == NULL)
		return 1;
	else
		return 0;
}

/* Inserts item at the end of the queue */
void queue_push (Queue* queue, int item, int read_or_write)
{
	Node* new = (Node*)malloc(sizeof(Node));
	new->item = item;
	new->read_or_write = read_or_write;
	new->next = NULL;

	if (queue->head == NULL) //queue is empty
		queue->head = new; //new becomes queue's head
	else
		queue->tail->next = new; //new becomes queue's tail
	queue->tail = new;
	queue->size++;
}

/* Removes and returns the head element from queue */
Node* queue_pop (Queue* queue)
{
	Node *head = queue->head;
	int item = head->item;
	queue->head = head->next;
	queue->size--;
	return head;
}	

/* Displays the queue */
void queue_display (Queue* queue)
{
	printf("Queue: ");
	if (queue->size == 0)
		printf("Empty\n");
	else
	{
		Node* head = queue->head;
		int i;
		int size = queue->size;
		printf("%d item(s)\n", queue->size);
		for (i = 0; i < size; i++)
		{
			if (i > 0) printf(", ");
			printf("%d", head->item);
			head = head->next;
		}
	}
	printf("\n");
}

/* Creates a new Queue */
Queue create_queue ()
{
	Queue queue;
	queue.size = 0;
	queue.head = NULL;
	queue.tail = NULL;
	queue.queue_is_empty = &queue_is_empty;
	queue.queue_push = &queue_push;
	queue.queue_pop = &queue_pop;
	queue.queue_display = &queue_display;
	return queue;
}
