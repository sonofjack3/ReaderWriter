/* Written by Cezar Campeanu, modified by Evan Jackson */
/* Defines reader and writer operations */

#include "reader_writer.h"

/* Performs self-suspends, waits for signals from monitor, and calls function for writing to buffer */
void *writer(int offset)
{
	printf("Writer with pid %d has suspended itself and is waiting to be awoken\n", getpid());
	pause(); //suspend this process and wait for signal from monitor
	printf("Writer with pid %d has been awoken\n");
    buffer_item item;
	while(1)
	{
		item = offset * rand() % 50; //random number to be written to the buffer
		if (write_item(item))
	    	printf("Writer error\n");
		else
	    	printf("Writer with pid %d has written %d\n", getpid(), item);
		printf("Writer with pid %d finished and is suspending itself\n", getpid());
		suspend(getpid()); //suspend this process when finished; wait for signal from monitor
	}
}

/* Performs self-suspends, waits for signals from monitor, and calls function for reading from buffer */
void *reader()
{
	printf("Reader with pid %d has suspended itself and is waiting to be awoken\n", getpid());
	pause(); //suspend this process and wait for signal from monitor
	printf("Reader with pid %d has been awoken\n", getpid());
    buffer_item item;
	while(1)
	{
		if (read_item(&item))
	    	printf("Reader error: empty buffer\n");
		else
    		printf("Reader with pid %d has read %d\n", getpid(), item);
		printf("Reader with pid %d has finished and is suspending itself\n", getpid());
		suspend(getpid()); //suspend this process when finished; wait for signal from monitor
	}
}

/* Writes an item to the buffer (two writers can not write simultaenously nor when a reader is reading)*/
int write_item(buffer_item item)
{
    buffer_item nextWritten = item;

    int *pin = pointer;

	if (!pthread_mutex_lock(wrt_shm)) //if not locked
	{
		/* Critical section */
		buffer[*pin] = nextWritten;
		printf("[%d] <== %d\n", *pin, buffer[*pin]);
		*pin = ((*pin + 1) % BUFFER_SIZE);
		pthread_mutex_unlock(wrt_shm);
		return 0;
	}
	else
		return 1;
}

/* Reads an item from the buffer (two readers may read simultaneously, but not when a writer is writing)*/
int read_item(buffer_item *item)
{
	buffer_item nextRead;
	int *pin = pointer;
	int *pout = pointer + 1;

	if (!pthread_mutex_lock(mutex_shm))
		reader_count++;
	if (reader_count == 1)
		while (pthread_mutex_lock(wrt_shm));
	pthread_mutex_unlock(mutex_shm);
	/* Critical section */
	nextRead = buffer[*pout];
	printf("[%d] == %d\n", *pout, buffer[*pout]);
	*pout = ((*pout+1) % BUFFER_SIZE);
	*item = nextRead;

	while (pthread_mutex_lock(mutex_shm));
	reader_count--;
	if (reader_count == 0)
		pthread_mutex_unlock(wrt_shm);
	pthread_mutex_unlock(mutex_shm);
	return 0;
}

/* Displays current buffer contents */
int show_data()
{
	int in = *pointer;
    int out = *(pointer+1);
    int i = 0;
    int localout = out;
    printf("Buffer contents:\n");
	if (in != localout)
    {
		while (in != localout)
		{
	    	printf("[%d] = %d\n", localout, buffer[localout]);
	    	localout = (localout + 1) % BUFFER_SIZE;
		}
		if (in == localout)
		{
			printf("[%d] = %d\n", localout, buffer[localout]);
		}
    }
    else
	    printf("[%d] = %d\n", localout, buffer[localout]);
}
