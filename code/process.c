#include "headers.h"

/* Modify this file as needed*/
int remainingtime;


void decrementTime(int signum)
{
    remainingtime--;
}

void terminate(int signum)
{
    destroyClk(false);
    kill(getppid(), SIGUSR2);
    exit(0);
}


int main(int agrc, char * argv[])
{
    printf("hello \n");
    signal(SIGUSR1, decrementTime);
    signal(SIGINT, terminate);
    initClk();
    //remaining time will be passed from the scheduler
    remainingtime = atoi(argv[1]);
    
    //TODO it needs to get the remaining time from somewhere
    while (remainingtime > 0)
    {
        //still running
    }
    printf("Scheduler: process with id: %d has finished\n", getpid());
    raise (SIGINT);

    
    return 0;
}
