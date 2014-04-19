#ifndef _READ_WRITE_
#define _READ_WRITE_

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "buffer.h"
#include "shared_mem.h"
#include "process_sync.h"

int write_item(buffer_item item);
int read_item(buffer_item *item);
void* reader();
void* writer(int offset);
int usr_interrupt;
int reader_count;

int show_data();
int void_and_show_data();

#endif
