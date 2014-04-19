#ifndef _SET_H_
#define _SET_H_

/* Written by Evan Jackson */
/* Defines set data and functions */

#include <stdlib.h>
#include <stdio.h>
#include "queue_set_data.h"

/* Represents a set element (stores an integer)*/
typedef struct Element
{
    int item;
	int read_or_write; //0 for reader, 1 for writer
    struct Element* next;
} Element;

/* Represents a set (linked list) of Elements */
typedef struct Set
{
    Element* head;
    Element* tail;
    void (*set_insert) (struct Set*, int item, int read_or_write);
    Element* (*set_remove) (struct Set*, int item);
	int (*set_is_empty) (struct Set*);
	void (*set_display) (struct Set*);
    int size;
} Set;

void set_insert(Set* set, int item, int read_or_write); 
Element* set_remove(Set* set, int item);
void set_display(Set* set);
int set_is_empty(Set* set);
Set create_set();

#endif
