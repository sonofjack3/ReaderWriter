#ifndef _SHARED_MEM_
#define _SHARED_MEM_

/* Written by Cezar Campeanu, modified by Evan Jackson */
/* Defines shared memory data */

pthread_mutex_t *mutex_shm; //pointer to the mutex shared memory segment
pthread_mutex_t *wrt_shm; //pointer to the WRT semaphore shared memory segment
buffer_item *buffer; //pointer to the buffer item shared memory segment
int* pointer; //pointer to the start of the buffer shared memory segment

/* Identifers for shared memory segments */
int mutex_segment_id;
int wrt_segment_id;
int buffer_segment_id;
int pointer_segment_id;

#endif
