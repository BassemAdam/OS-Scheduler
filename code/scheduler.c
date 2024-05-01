#include "headers.h"
#define decrementTime SIGUSR1

// Data Structures

//========================================================================================
// Priority Queue
typedef struct pnode
{

    struct process data;
    int priority;

    struct pnode *next;
} pNode;

pNode *newNode(struct process d, int p)
{
    pNode *temp = (pNode *)malloc(sizeof(pNode));
    temp->data = d;
    temp->priority = p;
    temp->next = NULL;

    return temp;
}

int msgq;
int countProcesses;
int remainingProcesses;
struct process currentlyRunningProcess;
pNode *pq = NULL;
pNode *ReadyQueueHPF = NULL;
int algorthmNo;

struct process ppeek(pNode **head)
{
    return (*head)->data;
}

void ppop(pNode **head)
{
    pNode *temp = *head;
    (*head) = (*head)->next;
    free(temp);
}

void ppush(pNode **head, struct process d, int p)
{
    pNode *start = (*head);

    pNode *temp = newNode(d, p);

    if ((*head)->priority > p)
    {

        temp->next = *head;
        (*head) = temp;
    }
    else
    {
        while (start->next != NULL &&
               start->next->priority < p)
        {
            start = start->next;
        }

        temp->next = start->next;
        start->next = temp;
    }
}

int pisEmpty(pNode **head) { return (*head) == NULL; }

//========================================================================================

// Circular Queue of pNodes
struct QueueC
{
    struct pnode *front, *rear;
};

// Function to create a new node with given data for the circular queue
struct pnode *newQueueCNode(struct process data)
{
    struct pnode *node = (struct pnode *)malloc(sizeof(struct pnode));
    node->data = data;
    node->next = NULL;
    return node;
}

// Function to create an empty circular queue
struct QueueC *createQueueC()
{
    struct QueueC *q = (struct QueueC *)malloc(sizeof(struct QueueC));
    q->front = q->rear = NULL;
    return q;
}

// Function to check if the circular queue is empty

int isQueueCEmpty(struct QueueC **q)
{
    return (*q)->front == NULL;
}

// Function to add a process to the end of the circular queue
void enqueueC(struct QueueC **q, struct process data)
{
    struct pnode *node = newQueueCNode(data);

    if ((*q)->rear == NULL)
    {
        (*q)->front = node;
        (*q)->rear = node;
        (*q)->rear->next = (*q)->front;
    }
    else
    {
        (*q)->rear->next = node;
        (*q)->rear = node;
        (*q)->rear->next = (*q)->front;
    }
}

// Function to remove a process from the front of the circular queue
struct process dequeueC(struct QueueC **q)
{
    if (isQueueCEmpty(q))
    {
        struct process emptyProcess;
        emptyProcess.id = -1;
        return emptyProcess; // Return an empty process when the queue is empty
    }

    struct pnode *temp = (*q)->front;
    struct process data = temp->data;

    if ((*q)->front == (*q)->rear)
    {
        (*q)->front = NULL;
        (*q)->rear = NULL;
    }
    else
    {
        (*q)->front = (*q)->front->next;
        (*q)->rear->next = (*q)->front;
    }

    free(temp);

    return data;
}

// Function to print the circular queue
void printCQueue(struct QueueC *q)
{
    if (isQueueCEmpty(&q))
    {
        printf("Queue is empty.\n");
        return;
    }

    struct pnode *temp = q->front;
    printf("Circular Queue: ");
    do
    {
        // change the color of the text to green
        printf("\033[0;32m");
        printf("P(id: %d, pid: %d, priority: %d, remaining_time: %d) ", temp->data.id, temp->data.pid, temp->data.priority, temp->data.remainig_time);
        temp = temp->next;
    } while (temp != q->front);

    printf("\033[0m"); // reset the text color to default
    printf("\n");
}

struct process peekC(struct QueueC **q, struct pnode *currentNode)
{
    struct process data = currentNode->data;
    currentNode = currentNode->next;
    return data;
}

void deleteCNode(struct QueueC **q, int id)
{
    struct pnode *temp = (*q)->front;
    struct pnode *prev = NULL;
    while (temp->data.id != id)
    {
        prev = temp;
        temp = temp->next;
    }
    if (temp == (*q)->front)
    {
        (*q)->front = temp->next;
        (*q)->rear->next = (*q)->front;
    }
    else if (temp == (*q)->rear)
    {
        prev->next = (*q)->front;
        (*q)->rear = prev;
    }
    else
    {
        prev->next = temp->next;
    }
    free(temp);
}

//========================================================================================

//------------------------------Normal Queue-----------------------------------
// Define the structure for a node in the normal queue
struct QueueNode
{
    struct process data;
    struct QueueNode *next;
};

// Function to create a new node with given data for the normal queue
struct QueueNode *newQueueNode(struct process data)
{
    struct QueueNode *node = (struct QueueNode *)malloc(sizeof(struct QueueNode));
    node->data = data;
    node->next = NULL;
    return node;
}

// Function to check if the normal queue is empty
int isQueueEmpty(struct QueueNode **front)
{
    return (*front) == NULL;
}

// Function to add a process to the end of the normal queue
void enqueueQueue(struct QueueNode **front, struct QueueNode **rear, struct process data)
{
    struct QueueNode *node = newQueueNode(data);

    if (*rear == NULL)
    {
        *front = *rear = node;
        return;
    }

    (*rear)->next = node;
    *rear = node;
}

// Function to remove a process from the front of the normal queue
struct process dequeueQueue(struct QueueNode **front, struct QueueNode **rear)
{
    if (isQueueEmpty(front))
    {
        struct process emptyProcess;
        emptyProcess.id = -1;
        return emptyProcess; // Return an empty process when the queue is empty
    }

    struct QueueNode *temp = *front;
    struct process data = temp->data;

    *front = (*front)->next;

    if (*front == NULL)
        *rear = NULL;

    free(temp);

    return data;
}
// Function to print the normal queue

//----------------------------------------Normal Queue end--------------------------------
//------------------------------------UTILITY FUNCTIONS for HPF------------------------------------
void deleteNode(struct pnode **head, int id)
{
    // Store head node
    struct pnode *temp = *head, *prev;

    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->data.id == id)
    {
        *head = temp->next; // Changed head
        free(temp);         // free old head
        return;
    }

    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->data.id != id)
    {
        prev = temp;
        temp = temp->next;
    }

    // If key was not present in linked list
    if (temp == NULL)
        return;

    // Unlink the node from linked list
    prev->next = temp->next;

    free(temp); // Free memory
}

void printQueue(struct pnode *pq)
{
    struct pnode *temp = pq;
    struct pnode *copy = NULL;
    while (temp != NULL)
    {
        if (copy == NULL)
            copy = newNode(temp->data, temp->data.priority);
        else
            ppush(&copy, temp->data, temp->data.priority);
        temp = temp->next;
    }
    printf("\n");
    printf("Ready queue = ");
    while (copy != NULL)
    {
        printf("\033[0;32m"); // Set the text color to green
        printf("P(id: %d, pid: %d, priority: %d, remaining_time: %d), ", copy->data.id, copy->data.pid, copy->data.priority, copy->data.remainig_time);
        printf("\033[0m"); // Resets the text color to default
        struct pnode *toFree = copy;
        copy = copy->next;
        free(toFree);
    }
    printf("\n");
}
//------------------------------------UTILITY FUNCTIONS for HPF end------------------------------------
//========================================================================================
// SIGINT handler
void terminateScheduler(int signum)
{
    destroyClk(false);
    msgctl(msgq, IPC_RMID, NULL);
    // signal to the process_genearator that the scheduler is done
    kill(getppid(), SIGINT);
    printf("Scheduler Terminating\n");
    exit(0);
}

// for RR
void alarmHandler(int signum)
{
    // This function will be called when the alarm signal is raised
    // You can use it to stop the currently running process
    if (currentlyRunningProcess.id != -1)
    {
        kill(currentlyRunningProcess.pid, SIGSTOP);
    }
}

struct process recieveProcess()
{
    struct msgbuff message;
    message.process.id = -1;
    message.process.pid = -1;
    if (msgrcv(msgq, &message, sizeof(message.process), 1, IPC_NOWAIT) == -1)
    {
        if (errno == ENOMSG)
        {
            // printf("no message Recieved \n");
        }
        else
        {
            perror("Error in receive");
            exit(-1);
        }
    }
    if (message.process.id != -1)
    {
        printf("Scheduler: received process with id: %d at time = %d \n", message.process.id, getClk());
        remainingProcesses--;
    }

    return message.process;
}

// Function to print the normal queue
void printQueueNrml(struct QueueNode *front)
{
    struct QueueNode *temp = front;
    while (temp != NULL)
    {
        printf("Process ID: %d, Process Priority: %d\n", temp->data.id, temp->data.priority);
        temp = temp->next;
    }
}
int quantumCounter = 0;
struct QueueC *readyQueueC = NULL;

int terminationFlag = 0;

void processTermination(int sig)
{
    terminationFlag = currentlyRunningProcess;
    
    //fprintf(logFile, "At time %d process %d finished arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
    if (algorthmNo == 1)
    {
        deleteNode(&pq, currentlyRunningProcess.id);
    }
    currentlyRunningProcess.id = -1;
    currentlyRunningProcess.pid = -1;

    printf("\033[0m"); // Resets the text color to default
    if (algorthmNo == 2)
    {
        ppop(&pq);
    }
}

struct QueueNode *waitingQfront = NULL;
struct QueueNode *waitingQrear = NULL;
struct pnode *currentCircularNode = NULL;

int main(int argc, char *argv[])
{
    signal(SIGINT, terminateScheduler);
    signal(SIGUSR2, processTermination);
    initClk();
    // execl("./scheduler.out", "scheduler.out", algorthmNo, quantum, countProcesses, NULL);
    FILE* logFile = fopen("Scheduler.logFile", "w");
    FILE* perf = fopen("Scheduler.perf", "w");

    algorthmNo = atoi(argv[1]);
    int quantum = atoi(argv[2]);
    countProcesses = atoi(argv[3]);
    remainingProcesses = countProcesses;
    int previousTime = getClk();
    currentlyRunningProcess.pid = -1;
    currentlyRunningProcess.id = -1;
    bool recievedProcessthisTimeStep = false;
    readyQueueC = createQueueC();

    key_t key = ftok("keyFile", msgqKey);
    msgq = msgget(key, IPC_CREAT | 0666);
    

    printf("Scheduler initialized with algorithm number: %d, quantum: %d, and count of processes: %d\n", algorthmNo, quantum, countProcesses);

    // main while loop
    while (1)
    {
        //check for terminated processes
        if (terminationFlag == -1)
        {
            terminationFlag = 0;
            
        }


        // ----------------------------------------------------------------Recieve Process----------------------------------------------------------------
        // WaitingQueue will contain all arrived processes
        struct process recievedProcess;
        recievedProcess.id = 1;

        while (recievedProcess.id != -1)
        {
            recievedProcess = recieveProcess();

            // if  recieved a process
            if (recievedProcess.id != -1)
            {
                // fork processes
                pid_t pid = fork();

                if (pid == 0)
                {
                    char str[10];
                    sprintf(str, "%d", recievedProcess.remainig_time);
                    execl("./process.out", "process.out", str, NULL);
                }
                else
                {
                    // sleep(1);
                    usleep(5 * 1000); // usleep takes sleep time in microseconds
                    recievedProcess.pid = pid;
                    recievedProcess.running_time = 0;
                    recievedProcess.arrival_time = getClk();

                    enqueueQueue(&waitingQfront, &waitingQrear, recievedProcess);
                }
                kill(recievedProcess.pid, SIGSTOP);
            }
        }

        int currentTime = getClk();
        // every time the clock ticks
        if (currentTime != previousTime)
        {
            printf("==============================Scheduler: current time = %d==============================\n", currentTime);
            //---------------------------------------------------------------Non-preemptive Highest Priority First HPF algorithm--------------------------------------------
            if (algorthmNo == 1)
            {

                printf("Currently running process: %d\n", currentlyRunningProcess.id);
                // take from queue to priority Q
                struct process temp;
                while (!isQueueEmpty(&waitingQfront))
                {

                    recievedProcessthisTimeStep = true;
                    temp = dequeueQueue(&waitingQfront, &waitingQrear);
                    if (pq == NULL)
                        pq = newNode(temp, temp.remainig_time);
                    else
                        ppush(&pq, temp, temp.remainig_time);
                    // printf("waiting queue iam here"); printQueue(pq);
                }
                printQueue(pq);

                // i want here to get the process that arrived first and then add it to ready queue

                // if no process is running
                if (currentlyRunningProcess.id == -1)
                {
                    if (!pisEmpty(&pq))
                    {
                        currentlyRunningProcess = ppeek(&pq);

                        printf("Currently running process: %d\n", currentlyRunningProcess.id);

                        kill(currentlyRunningProcess.pid, SIGCONT);
                        kill(currentlyRunningProcess.pid, decrementTime);

                        currentlyRunningProcess.remainig_time--;
                    }
                }
                else // process is running
                {
                    kill(currentlyRunningProcess.pid, SIGCONT);
                    kill(currentlyRunningProcess.pid, decrementTime);
                }

                recievedProcessthisTimeStep = false;

                // if queue is empty and remainingprocesses = 0
                if (remainingProcesses == 0 && pisEmpty(&pq) && pisEmpty(&pq))
                {
                    kill(getppid(), SIGINT);
                }
            }
            //---------------------------------------------------------------Non-preemptive Highest Priority First HPF algorithm END--------------------------------------------

            //---------------------------------------------------------------STRN algorithm----------------------------------------------------------------------------------
            if (algorthmNo == 2)
            {
                // take from queue to priority Q
                struct process temp;
                while (!isQueueEmpty(&waitingQfront))
                {

                    recievedProcessthisTimeStep = true;
                    temp = dequeueQueue(&waitingQfront, &waitingQrear);
                    if (pq == NULL)
                        pq = newNode(temp, temp.remainig_time);
                    else
                        ppush(&pq, temp, temp.remainig_time);
                }
                printQueue(pq);

                // if no process is running
                if (currentlyRunningProcess.id == -1)
                {
                    if (!pisEmpty(&pq))
                    {
                        fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk() - 1, ppeek(&pq).id, ppeek(&pq).arrival_time, ppeek(&pq).running_time, ppeek(&pq).remainig_time, getClk() - 1 - ppeek(&pq).arrival_time - ppeek(&pq).running_time);
                        currentlyRunningProcess = ppeek(&pq);
                        kill(currentlyRunningProcess.pid, SIGCONT);
                        kill(currentlyRunningProcess.pid, decrementTime);
                        ppop(&pq);
                        currentlyRunningProcess.remainig_time--;
                        if (pq == NULL)
                            pq = newNode(currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                        else
                            ppush(&pq, currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                    }
                }
                else // process is running
                {
                    if (recievedProcessthisTimeStep)
                    {

                        if (currentlyRunningProcess.remainig_time > ppeek(&pq).remainig_time)
                        {
                            kill(currentlyRunningProcess.pid, SIGSTOP);
                            currentlyRunningProcess = ppeek(&pq);
                            kill(currentlyRunningProcess.pid, SIGCONT);
                            kill(currentlyRunningProcess.pid, decrementTime);
                            ppop(&pq);
                            currentlyRunningProcess.remainig_time--;
                            if (pq == NULL)
                                pq = newNode(currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                            else
                                ppush(&pq, currentlyRunningProcess, currentlyRunningProcess.remainig_time);

                            if (currentlyRunningProcess.arrival_time == getClk()-1)
                            {
                                fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
                            }
                            else
                            {
                                fprintf(logFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
                            }
                        }
                        else
                        {
                            kill(currentlyRunningProcess.pid, decrementTime);
                            ppop(&pq);
                            currentlyRunningProcess.remainig_time--;
                            if (pq == NULL)
                                pq = newNode(currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                            else
                                ppush(&pq, currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                        }
                    }
                    else // no recieved process this time step
                    {

                        kill(currentlyRunningProcess.pid, decrementTime);
                        ppop(&pq);
                        currentlyRunningProcess.remainig_time--;
                        if (pq == NULL)
                            pq = newNode(currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                        else
                            ppush(&pq, currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                    }
                }

                recievedProcessthisTimeStep = false;

                // if queue is empty and remainingprocesses = 0
                if (remainingProcesses == 0 && pisEmpty(&pq))
                {
                    kill(getppid(), SIGINT);
                }
            }

            //---------------------------------------------------------------Round robin algorithm------------------------------------------------------------------------------
            if (algorthmNo == 3)
            {
                struct process temp;
                while (!isQueueEmpty(&waitingQfront)) // arrival queue is not empty, receive and enqueue inside circular queue
                {
                    recievedProcessthisTimeStep = true;

                    temp = dequeueQueue(&waitingQfront, &waitingQrear);

                    enqueueC(&readyQueueC, temp);
                }
                printCQueue(readyQueueC);
                if (currentlyRunningProcess.id == -1) // if there is no running process, dequeue and run it
                {
                    if (!isQueueCEmpty(&readyQueueC))
                    {
                        currentlyRunningProcess = dequeueC(&readyQueueC);

                        quantumCounter = 0;
                        fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
                        kill(currentlyRunningProcess.pid, SIGCONT);
                        kill(currentlyRunningProcess.pid, decrementTime);
                        currentlyRunningProcess.remainig_time--;
                        currentlyRunningProcess.running_time++;
                        quantumCounter++;
                    }
                }
                else
                {
                    // if (currentlyRunningProcess.remainig_time == 0)
                    // {
                    //     printf("twAT\n");
                    //     fprintf(logFile, "At time %d process %d finished arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
                    //     kill(currentlyRunningProcess.pid, SIGSTOP);
                    //     currentlyRunningProcess.id = -1;
                    //     currentlyRunningProcess.pid = -1;
                    //     if (!isQueueCEmpty(&readyQueueC))
                    //     {
                    //         currentlyRunningProcess = dequeueC(&readyQueueC);
                    //         quantumCounter = 0;
                    //     }
                    // }
                    if (quantumCounter < quantum)
                    {

                        kill(currentlyRunningProcess.pid, SIGCONT);
                        kill(currentlyRunningProcess.pid, decrementTime);
                        currentlyRunningProcess.remainig_time--;
                        currentlyRunningProcess.running_time++;
                        quantumCounter++;
                    }
                    else if (quantumCounter == quantum)
                    {
                        fprintf(logFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
                        kill(currentlyRunningProcess.pid, SIGSTOP);
                        if (currentlyRunningProcess.remainig_time > 0)
                            enqueueC(&readyQueueC, currentlyRunningProcess);
                        currentlyRunningProcess.id = -1;
                        currentlyRunningProcess.pid = -1;
                        quantumCounter = 0;
                        currentlyRunningProcess = dequeueC(&readyQueueC);
                        if (currentlyRunningProcess.arrival_time == getClk())
                        {
                            fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
                        }
                        else
                        {
                            fprintf(logFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
                        }
                        kill(currentlyRunningProcess.pid, SIGCONT);
                        kill(currentlyRunningProcess.pid, decrementTime);
                        currentlyRunningProcess.remainig_time--;
                        currentlyRunningProcess.running_time++;
                        quantumCounter++;
                    }
                }
                recievedProcessthisTimeStep = false;

                // if queue is empty and remainingprocesses = 0
                if (remainingProcesses == 0 && isQueueCEmpty(&readyQueueC) && currentlyRunningProcess.id == -1)
                {
                    kill(getppid(), SIGINT);
                }
            }

            //---------------------------------------------------------------Round robin algorithm END -------------------------------------------------------------------------

            previousTime = currentTime;
        }
    }

    raise(SIGINT);
    fclose(logFile);
    fclose(perf);
}