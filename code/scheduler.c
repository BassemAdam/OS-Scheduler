#include "headers.h"
#define decrementTime SIGUSR1



//Data Structures
//========================================================================================
//Priority Queue
typedef struct pnode {

    struct process data;
    int priority;

    struct pnode* next;
} pNode;

pNode * newNode(struct process d, int p)
{
    pNode * temp = (pNode*)malloc(sizeof(pNode));
    temp->data = d;
    temp->priority = p;
    temp->next = NULL;

    return temp;
}

int msgq;
int countProcesses;
int remainingProcesses;
struct process currentlyRunningProcess;
pNode * pq  = NULL;

struct process ppeek(pNode** head)
{
    return (*head)->data;
}

void ppop(pNode** head)
{
    pNode* temp = *head;
    (*head) = (*head)->next;
    free(temp);
}

void ppush(pNode** head, struct process d, int p)
{
    pNode* start = (*head);

    pNode* temp = newNode(d, p);

    if ((*head)->priority > p) {

        temp->next = *head;
        (*head) = temp;
    }
    else {
        while (start->next != NULL &&
            start->next->priority < p) {
            start = start->next;
        }

        temp->next = start->next;
        start->next = temp;
    }
}

int pisEmpty(pNode** head) { return (*head) == NULL; }

//========================================================================================
//SIGINT handler
void terminateScheduler(int signum)
{
    destroyClk(false);
    msgctl(msgq, IPC_RMID, NULL);
    //signal to the process_genearator that the scheduler is done
    kill(getppid(), SIGINT);
    printf("Scheduler Terminating\n");
    exit(0);
}
//========================================================================================

struct process recieveProcess()
{
    struct msgbuff message;
    message.process.id = -1;
    message.process.pid = -1;
    if (msgrcv(msgq, &message, sizeof(message.process),1, IPC_NOWAIT) == -1)
    {
            if (errno == ENOMSG)
            {
               // printf("no message Recieved \n");
                
            }else
            {
                perror("Error in receive");
                exit(-1);
            }    
            
    }
    if (message.process.id != -1)
    {
        printf("Scheduler: received process with id: %d \n", message.process.id);
        remainingProcesses--;
    }
    
    return message.process;
}

void processTermination(int sig)
{
    
    currentlyRunningProcess.id = -1;
    currentlyRunningProcess.pid = -1;
    ppop(&pq);

}






int main(int argc, char * argv[])
{
    signal(SIGINT, terminateScheduler);
    signal(SIGUSR2, processTermination);
    initClk();
    //execl("./scheduler.out", "scheduler.out", algorthmNo, quantum, countProcesses, NULL);
    int algorthmNo = atoi(argv[1]);
    int quantum = atoi(argv[2]);
    countProcesses = atoi(argv[3]);
    remainingProcesses = countProcesses;
    int previousTime = getClk();
    currentlyRunningProcess.pid = -1;
    currentlyRunningProcess.id = -1;
    bool recievedProcessthisTimeStep = false;
    

    key_t key = ftok("keyFile", msgqKey);
    msgq = msgget(key, IPC_CREAT | 0666);
  

     
    printf("Scheduler initialized with algorithm number: %d, quantum: %d, and count of processes: %d\n", algorthmNo, quantum, countProcesses);

    
    //main while loop
    while(1)
    {

        int currentTime = getClk();
        //every time the clock ticks
        if (currentTime != previousTime)
        {

            //---------------------------------------------------------------STRN algorithm----------------------------------------------------------------------------------
            if (algorthmNo == 2)
            {
                struct process recievedProcess;
                recievedProcess.id = 1;

                while (recievedProcess.id  != -1)
                {
                    recievedProcess = recieveProcess();

                    //if  recieved a process
                    if (recievedProcess.id != -1)
                    {
                       
                        //fork processes
                        pid_t pid = fork();
                        if (pid == 0)
                        {
                            char str[10];
                            sprintf(str, "%d", recievedProcess.remainig_time);
                            execl("./process.out", "process.out", str, NULL);
                            
                        }
                        else
                        {
                            sleep(1);
                            recievedProcess.pid = pid;
                            kill(recievedProcess.pid, SIGSTOP);
                        }

                        if(pq == NULL)
                            pq = newNode(recievedProcess, recievedProcess.remainig_time); 
                        else
                            ppush(&pq, recievedProcess, recievedProcess.remainig_time);

                        
                        if (currentlyRunningProcess.id == -1) // no running process
                        {   
                            currentlyRunningProcess = ppeek(&pq);
                            kill(currentlyRunningProcess.pid, SIGCONT);
                            kill(currentlyRunningProcess.pid, decrementTime);
                            ppop(&pq);
                            currentlyRunningProcess.remainig_time--;
                            if(pq == NULL)
                            pq = newNode(currentlyRunningProcess, currentlyRunningProcess.remainig_time); 
                        else
                            ppush(&pq, currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                            
                        }else // running process and recieved a process 
                        {
                            struct process temp = ppeek(&pq);
                            if(temp.id == currentlyRunningProcess.id) // still the highest priority
                            {

                                kill(currentlyRunningProcess.pid, decrementTime);
                                
                                 ppop(&pq);
                            currentlyRunningProcess.remainig_time--;
                            if(pq == NULL)
                            pq = newNode(currentlyRunningProcess, currentlyRunningProcess.remainig_time); 
                        else
                            ppush(&pq, currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                                
                            }
                            else // not the highest priority
                            {
                                kill(currentlyRunningProcess.pid, SIGSTOP);
                                currentlyRunningProcess = ppeek(&pq);
                                kill(currentlyRunningProcess.pid, SIGCONT);
                                kill(currentlyRunningProcess.pid, decrementTime);
                                
                                 ppop(&pq);
                            currentlyRunningProcess.remainig_time--;
                            if(pq == NULL)
                            pq = newNode(currentlyRunningProcess, currentlyRunningProcess.remainig_time); 
                        else
                            ppush(&pq, currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                            }
                        }

                            recievedProcessthisTimeStep = true;
                        
                    }

            
                }
                
                if(recievedProcessthisTimeStep) 
                {
                    recievedProcessthisTimeStep = false;
                    previousTime = currentTime;
                    continue;
                }
                //no recieved process and ther is no running process
                if (currentlyRunningProcess.id == -1)
                {
                    if(!pisEmpty(&pq))
                    {
                        currentlyRunningProcess = ppeek(&pq);
                        kill(currentlyRunningProcess.pid, SIGCONT);
                        
                        kill(currentlyRunningProcess.pid, decrementTime);
                         ppop(&pq);
                            currentlyRunningProcess.remainig_time--;
                            if(pq == NULL)
                            pq = newNode(currentlyRunningProcess, currentlyRunningProcess.remainig_time); 
                        else
                            ppush(&pq, currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                       
                    }
                   
                }else // no recieved process but there is a running process
                {
                    
                    kill(currentlyRunningProcess.pid, decrementTime);
                    
                     ppop(&pq);
                            currentlyRunningProcess.remainig_time--;
                            if(pq == NULL)
                            pq = newNode(currentlyRunningProcess, currentlyRunningProcess.remainig_time); 
                        else
                            ppush(&pq, currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                    
                }
               
                
            }
            
    
           
           



            previousTime = currentTime;
        }
        
        if(remainingProcesses == 0  && pisEmpty(&pq))
            kill(getppid(), SIGINT);
        

        
        
        
    }







    
    raise (SIGINT);
}
