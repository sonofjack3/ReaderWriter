/* Written by Cezar Campeanu, modified by Evan Jackson */
/* Defines signals and handler */

#include "process_sync.h"

/* Simply defines a handler for signals */
void alarmHandler()
{
}

/* Suspends the process with given pid */
int suspend(int pid)
{
    return kill(pid, SIGSTOP);
}

/* Resumes the process with given pid */
int wakeup(int pid)
{
    return kill(pid, SIGCONT);  
}
