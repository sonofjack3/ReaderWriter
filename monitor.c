/* Written by Evan Jackson */
/* Main monitor program creates and schedules readers and writers */

#include "monitor.h"

const int max_length = 30; //maximum number of operations
/* Determine shared memory segment sizes */
const int mutex_size = sizeof(pthread_mutex_t);
const int wrt_size = sizeof(pthread_mutex_t);
const int buffer_size = sizeof(buffer_item) * BUFFER_SIZE;
const int pointer_size = sizeof(int);
/* Queues for suspended processes */
Queue reader_queue;
Queue writer_queue;

/* Function for creating a reader process */
void fork_readers(int n)
{
	pid_t pid;
	pid = fork();
	if (pid < 0) //error
	{
		printf("Fork error\n");
	}
	if (pid == 0) //child process - reader
	{
		reader(); //this function suspends the process immediately (see reader_writer.c)
	}
	else if (pid > 0) //parent process - monitor
	{
		sleep(1); //wait for child to be created and suspended
		reader_queue.queue_push(&reader_queue, pid, READER); //push reader onto queue
		if (n > 1)
			fork_readers(n-1); //recursively create more children; ensures all children have same parent
	}
}

/* Function for creating a writer process */
void fork_writers(int n)
{
	pid_t pid;
	pid = fork();
	if (pid < 0) //error
	{
		printf("Fork error\n");
	}
	if (pid == 0) //child process - writer
	{
		writer(n); //this function suspends the process immediately (see reader_writer.c)
	}
	else if (pid > 0) //parent process - monitor
	{
		sleep(1); //wait for child to be created and suspended
		writer_queue.queue_push(&writer_queue, pid, WRITER); //push writer onto queue
		if (n > 1)
			fork_writers(n-1); //recursively create more children; ensures all children have same parent
	}
}

/* Main function reads in user input, creates readers and writers, and performs scheduling */
int main(int argc, char* argv[])
{
	if (argc != 1) //print usage statement on irregular number of arguments
	{
		fprintf(stderr, "Usage: ./readWrite\n");
		return 1;
	}	

	signal(SIGCONT, alarmHandler); //set up handler for continuing child processes
	int num_readers, num_writers; //reader and writer counts
	char ops[max_length]; //string storing read/write operations to be performed
	int num_ops; //total number of operations
	int i, j; //index counters
	
	/* Get number of readers and writers, number of operations, and buffer size from user */
	printf("Enter the number of readers: ");
	scanf("%d", &num_readers);
	printf("Enter the number of writers: ");
	scanf("%d", &num_writers);
	printf("Enter a string of r's and w's (r's represent reads, w's represent writes) - maximum %d: ", max_length);
	scanf("%s", ops);
	num_ops = strlen(ops);
	if (num_ops > max_length) //if passed number of operations exceed maxium allowed
	{
		fprintf(stderr, "Error: number of operations exceeds maximum of %d\n", max_length);
		return 1; //error
	}
	/* Print user inputs */
	printf("Number of readers: %d\n", num_readers);
	printf("Number of writers: %d\n", num_writers);
	printf("Operation string: %s\n", ops);
	printf("Total number of operations: %d\n", num_ops);

	/* Set up shared memory */
	/* Allocate mutex shared memory */
	mutex_segment_id = shmget(IPC_PRIVATE, mutex_size, S_IRUSR | S_IWUSR);
    mutex_shm = (pthread_mutex_t *)shmat(mutex_segment_id, NULL, 0); 
    printf("Mutex shared memory pointer=%x\n", mutex_shm);
	/* Allocate WRT semaphore shared memory */
	wrt_segment_id = shmget(IPC_PRIVATE, wrt_size, S_IRUSR | S_IWUSR);
	wrt_shm = (pthread_mutex_t *)shmat(wrt_segment_id, NULL, 0);
	printf("WRT semaphore memory pointer=%x\n", wrt_shm);
	/* Allocate buffer shared memory */
    buffer_segment_id = shmget(IPC_PRIVATE, buffer_size, S_IRUSR | S_IWUSR); 
    buffer = (buffer_item *)shmat(buffer_segment_id, NULL, 0); 
    printf("Buffer shared memory pointer=%x\n", buffer);
	/* Allocate buffer pointer shared memory */ 
    pointer_segment_id = shmget(IPC_PRIVATE, pointer_size, S_IRUSR | S_IWUSR);
    pointer = (int*)shmat(pointer_segment_id, NULL, 0); 
    printf("IN shared memory pointer=%x\n", pointer);

	/* Initialize mutex */
    pthread_mutex_init(mutex_shm, NULL);
	/* Initialize WRT semaphore */
	pthread_mutex_init(wrt_shm, NULL);

	/* Create queues for storing suspended processes */
	reader_queue = create_queue();
	writer_queue = create_queue();
	/* Create readers */
	printf("Creating readers...\n");
	fork_readers(num_readers);
	/* Create writers */
	printf("Creating writers...\n");
	fork_writers(num_writers);	

	/* Display queues */
	printf("Reader queue:\n");
	reader_queue.queue_display(&reader_queue);
	printf("Writer queue:\n");
	writer_queue.queue_display(&writer_queue);

	/* Iterate through the operations string and initiate processes to perform the operations */
	reader_count = 0; //active reader count
	Set set = create_set(); //set of active processes
	Node* curr_node; //current process in queue (suspended)
	Element* curr_element; //current process in set (active)
	int status, endID;
	for (i = 0; i < num_ops; i++)
	{
		/* Read operation */
		if (ops[i] == 'r')
		{
			//printf("Operation %d is a read\n", i);
			/* Pop reader off queue if the queue is not empty, otherwise create a new reader*/
			if(!reader_queue.queue_is_empty(&reader_queue))
				curr_node = reader_queue.queue_pop(&reader_queue);
			else
			{
				fork_readers(1); //create a new reader
				curr_node = reader_queue.queue_pop(&reader_queue);
			}
			/* Add process to active set */
			set.set_insert(&set, curr_node->item, curr_node->read_or_write); 
			/* Resume the process */
			wakeup(curr_node->item);
		}

		/* Write operation */
		else if (ops[i] == 'w')
		{
			//printf("Operation %d is a write\n", i);
			/* Pop writer off queue if the queue is not empty, otherwise create a new writer */
			if (!writer_queue.queue_is_empty(&writer_queue))
				curr_node = writer_queue.queue_pop(&writer_queue);
			else
			{
				fork_writers(1); //create a new writer
				curr_node = writer_queue.queue_pop(&writer_queue);
			}
			/* Add process to active set */
			set.set_insert(&set, curr_node->item, curr_node->read_or_write);
			/* Resume the process */
			wakeup(curr_node->item);
		}
		
		/* Once a process finishes, it automatically suspends itself (see reader_writer.c) */
		
		/* Loop through active set and add suspended processes to queue */
		for (curr_element = set.head; curr_element != NULL; curr_element = curr_element->next)
		{
			endID = waitpid(curr_element->item, &status, WNOHANG | WUNTRACED); //get status of current process
			if (endID == curr_element->item)
			{
				if (WIFSTOPPED(status)) //if current process has been stopped
				{
					curr_element = set.set_remove(&set, curr_element->item); //remove from active set
					if (curr_element->read_or_write == READER)
						reader_queue.queue_push(&reader_queue, curr_element->item, READER); //add reader to queue
					else if (curr_element->read_or_write == WRITER)
						writer_queue.queue_push(&writer_queue, curr_element->item, WRITER); //add writer to queue
				}
			}
		}
	}

	/* Allow remaining running processes to finish (ie: suspend themselves) */
	while (!set.set_is_empty(&set))
	{
		/* Loop through active set and add suspended processes to queue */
		for (curr_element = set.head; curr_element != NULL; curr_element = curr_element->next)
		{
			endID = waitpid(curr_element->item, &status, WNOHANG | WUNTRACED); //get status of current process
			if (endID == curr_element->item)
			{
				if (WIFSTOPPED(status)) //if current process has been stopped
				{
					curr_element = set.set_remove(&set, curr_element->item); //remove from active set
					if (curr_element->read_or_write == READER)
						reader_queue.queue_push(&reader_queue, curr_element->item, READER); //add reader to queue
					else if (curr_element->read_or_write == WRITER)
						writer_queue.queue_push(&writer_queue, curr_element->item, WRITER); //add writer to queue
				}
			}
		}
	}

	/* Kill processes and free memory */
	/* Kill all suspended readers */
	while (!reader_queue.queue_is_empty(&reader_queue))
	{
		curr_node = reader_queue.queue_pop(&reader_queue);
		kill(curr_node->item, SIGKILL);
	}
	/* Kill all suspended writers */
	while (!writer_queue.queue_is_empty(&writer_queue))
	{
		curr_node = reader_queue.queue_pop(&writer_queue);
		kill(curr_node->item, SIGKILL);
	}
	/* Debugging block: kill all active processes - this won't be needed, since at the end all processes will be suspended */
	printf("Active processes:\n");
	set.set_display(&set); //will display empty
	int curr_active_process;
	while (!set.set_is_empty(&set))
	{
		curr_active_process = set.head->item;
		kill(curr_active_process, SIGKILL);
		set.set_remove(&set, curr_active_process); 
	}
	
	/* Display buffer */
	show_data();

	/* Free memory */
	free(curr_node);
	free(curr_element);	

	return 0;
}
