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
    exit(0);
}


int main(int agrc, char * argv[])
{
    signal(SIGUSR1, decrementTime);
    signal(SIGINT, terminate);
    initClk();
    //remaining time will be passed from the scheduler
    remainingtime = atoi(argv[1]);
    
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    while (remainingtime > 0)
    {
        //still running
    }
    
    raise (SIGINT);

    
    return 0;
}
