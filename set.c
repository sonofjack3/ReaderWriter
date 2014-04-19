/* Written by Evan Jackson */
/* Defines set operations */

#include "set.h"

/* Returns 1 if set is empty, 0 otherwise */
int set_is_empty (Set* set)
{
	if (set->head == NULL)
		return 1;
	else
		return 0;
}

/* Inserts item at the tail end of set */
void set_insert (Set* set, int item, int read_or_write)
{
	Element* newElement = (Element*)malloc(sizeof(Element));
	newElement->item = item;
	newElement->read_or_write = read_or_write;
	newElement->next = NULL;

	if (set->head == NULL) //set is empty
		set->head = newElement; //newElement becomes set's head
	else
		set->tail->next = newElement; //newElement becomes set's tail
	set->tail = newElement;
	set->size++;
}

/* Removes and returns item from set */
Element* set_remove (Set* set, int item)
{
	Element *head = set->head;
	Element *tail = set->tail;
	int head_item = head->item;
	int i;
	Element *curr = head;
	Element *match = head;
	Element *prev = NULL;

	/* Find first element in set matching item */
	for (i = 0; i < set->size; i++)
	{
		if (curr->item == item) //found match
		{
			match = curr;
			break;
		}
		prev = curr;
		curr = curr->next; //move to next element
	}
	/* If the matched element is the set's head */
	if (match->item == head->item)
	{
		set->head = head->next;
		set->size--;
	}
	/* If the matched element is the set's tail */
	else if (match->item == tail->item)
	{
		set->tail = prev;
		set->tail->next = NULL;
		set->size--;
	}
	/* If the matched element is not the set's head or tail */
	else
	{
		prev->next = match->next;
		set->size--;
	}
	return match;
}	

/* Displays the set */
void set_display (Set* set)
{
	printf("Set: ");
	if (set->size == 0)
		printf("Empty\n");
	else
	{
		Element* head = set->head;
		int i;
		int size = set->size;
		printf("%d item(s)\n", set->size);
		for (i = 0; i < size; i++)
		{
			if (i > 0) printf(", ");
			printf("%d", head->item);
			head = head->next;
		}
	}
	printf("\n");
}

/* Creates a new Set */
Set create_set ()
{
	Set set;
	set.size = 0;
	set.head = NULL;
	set.tail = NULL;
	set.set_is_empty = &set_is_empty;
	set.set_insert = &set_insert;
	set.set_remove = &set_remove;
	set.set_display = &set_display;
	return set;
}
