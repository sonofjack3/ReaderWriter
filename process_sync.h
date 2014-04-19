#ifndef _PROCESS_SYNC_
#define _PROCESS_SYNC_

/* Written by Cezar Campeanu, modified by Evan Jackson */
/* Defines signals and handler */

#include <signal.h>

sigset_t mask, oldmask;

int wakeup(int pid); //resumes process with pid
int suspend(int pid); //suspends process with pid
void alarmHandler(); //signal handler

#endif
