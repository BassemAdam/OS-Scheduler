#include "headers.h"

/* Modify this file as needed*/
int remainingtime;


void decrementTime(int signum)
{
    
    remainingtime--;
    printf("Scheduler: process with id: %d is running and  has remaining time %d at TIME: %d \n", getpid(), remainingtime ,getClk());
}

void terminate(int signum)
{
    destroyClk(false);
    kill(getppid(), SIGUSR2);
    exit(0);
}



int main(int agrc, char * argv[])
{
    remainingtime = atoi(argv[1]);
    printf("Scheduler: process with id: %d is ready with remaining time  %d \n", getpid() , remainingtime);
    
    signal(SIGUSR1, decrementTime);
    signal(SIGINT, terminate);
    initClk();
    //remaining time will be passed from the scheduler
   
    //TODO it needs to get the remaining time from somewhere
    while (remainingtime > 0)
    {
        //still running
    }
    //change color to purple
    printf("\033[0;35m");
    printf("Scheduler: process with id: %d has finished\n", getpid());
    printf("\033[0m");
    raise (SIGINT);

    
    return 0;
}
