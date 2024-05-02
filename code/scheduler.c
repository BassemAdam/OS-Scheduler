#include "headers.h"
#include "data_structures.h"

#define decrementTime SIGUSR1

float *WTAArray;

float calculateSD(float data[], int processCount)
{
    float sum = 0.0, mean, SD = 0.0;
    int i;
    for (i = 0; i < processCount; ++i)
    {
        sum += data[i];
    }
    mean = sum / processCount;
    for (i = 0; i < processCount; ++i)
    {
        SD += pow(data[i] - mean, 2);
    }
    return sqrt(SD / processCount);
}

int msgq;
int countProcesses;
int remainingProcesses;

struct process currentlyRunningProcess;
struct QueueNode *waitingQfront = NULL;
struct QueueNode *waitingQrear = NULL;
struct pnode *currentCircularNode = NULL;

pNode *pq = NULL;
pNode *ReadyQueueHPF = NULL;

int algorthmNo;
int quantumCounter = 0;

struct QueueC *readyQueueC = NULL;

FILE *logFile;
FILE *perf;

float avgTA = 0;
float sumWTA = 0;
float CPUUtilization = 0;
float stdWTA = 0;
float avgWaiting = 0;
bool recievedProcessthisTimeStep = false;

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

struct process recieveProcess()
{
    struct msgbuff message;
    message.process.id = -1;
    message.process.pid = -1;
    if (msgrcv(msgq, &message, sizeof(message.process), 1, IPC_NOWAIT) == -1)
    {
        if (errno != ENOMSG)
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

void processTermination(int sig)
{

    fprintf(logFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time, getClk() - currentlyRunningProcess.arrival_time, (float)(getClk() - currentlyRunningProcess.arrival_time) / currentlyRunningProcess.running_time);
    if (algorthmNo == 1)
    {
        deleteNode(&pq, currentlyRunningProcess.id);
    }
    if (algorthmNo == 2)
    {
        ppop(&pq);
    }
    avgTA += getClk() - currentlyRunningProcess.arrival_time;
    sumWTA += (float)(getClk() - currentlyRunningProcess.arrival_time) / currentlyRunningProcess.running_time;
    CPUUtilization += (float)currentlyRunningProcess.running_time;

    WTAArray[currentlyRunningProcess.id - 1] = (float)(getClk() - currentlyRunningProcess.arrival_time) / currentlyRunningProcess.running_time;
    avgWaiting += getClk() - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time;

    currentlyRunningProcess.id = -1;
    currentlyRunningProcess.pid = -1;
    currentlyRunningProcess.state = "finished";
}

void printPerformance()
{
    fprintf(perf, "CPU utilization = %.2f %%\n", CPUUtilization / (getClk() - 1) * 100);
    fprintf(perf, "Avg WTA = %.2f\n", sumWTA / countProcesses);
    fprintf(perf, "Avg Waiting = %.2f\n", avgWaiting / countProcesses);
    fprintf(perf, "Std WTA = %.2f\n", calculateSD(WTAArray, countProcesses));
}

void runProcess()
{
    kill(currentlyRunningProcess.pid, SIGCONT);
    kill(currentlyRunningProcess.pid, decrementTime);
    currentlyRunningProcess.remainig_time--;
    currentlyRunningProcess.running_time++;
}

void HPF();
void SRTN();
void RR(int quantum);

int main(int argc, char *argv[])
{

    signal(SIGINT, terminateScheduler);
    signal(SIGUSR2, processTermination);
    initClk();

    logFile = fopen("Scheduler.log", "w");
    perf = fopen("Scheduler.perf", "w");

    algorthmNo = atoi(argv[1]);
    int quantum = atoi(argv[2]);
    countProcesses = atoi(argv[3]);
    WTAArray = malloc(countProcesses * sizeof(float));
    remainingProcesses = countProcesses;
    int previousTime = getClk();
    currentlyRunningProcess.pid = -1;
    currentlyRunningProcess.id = -1;
    currentlyRunningProcess.start_time = -1;
    currentlyRunningProcess.state = malloc(10 * sizeof(char));

    readyQueueC = createQueueC();

    key_t key = ftok("keyFile", msgqKey);
    msgq = msgget(key, IPC_CREAT | 0666);

    printf("Scheduler initialized with algorithm number: %d, quantum: %d, and count of processes: %d\n", algorthmNo, quantum, countProcesses);
    fprintf(logFile, "#At time x process y state arr w total z remain y wait k\n");

    // main while loop
    while (1)
    {

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
                    recievedProcess.start_time = -1;
                    recievedProcess.state = "waiting";
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

            switch (algorthmNo)
            {
            case 1:
                HPF();
                break;
            case 2:
                SRTN();
                break;
            case 3:
                RR(quantum);
                break;

            default:
                printf("invalid algorithm");
                break;
            }
            previousTime = currentTime;
        }
    }

    raise(SIGINT);
    fclose(logFile);
    fclose(perf);
}

void HPF()
{
    printf("Currently running process: %d\n", currentlyRunningProcess.id);
    // take from queue to priority Q
    struct process temp;
    while (!isQueueEmpty(&waitingQfront))
    {

        recievedProcessthisTimeStep = true;
        temp = dequeueQueue(&waitingQfront, &waitingQrear);
        if (pq == NULL)
            pq = newNode(temp, temp.priority);
        else
            ppush(&pq, temp, temp.priority);
    }
    printQueue(pq);

    // i want here to get the process that arrived first and then add it to ready queue

    // if no process is running
    if (currentlyRunningProcess.id == -1)
    {
        if (!pisEmpty(&pq))
        {

            currentlyRunningProcess = ppeek(&pq);
            currentlyRunningProcess.state = "running";
            printf("Currently running process: %d\n", currentlyRunningProcess.id);

            if (currentlyRunningProcess.start_time == -1)
            {
                currentlyRunningProcess.start_time = getClk() - 1;
                fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
            }
            else
            {
                fprintf(logFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
            }

            runProcess();
        }
    }
    else // process is running
    {
        currentlyRunningProcess.state = "running";
        runProcess();
    }

    recievedProcessthisTimeStep = false;

    // if queue is empty and remainingprocesses = 0
    if (remainingProcesses == 0 && pisEmpty(&pq) && pisEmpty(&pq))
    {
        printPerformance();
        kill(getppid(), SIGINT);
    }
}

void SRTN()
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

            currentlyRunningProcess = ppeek(&pq);
            if (currentlyRunningProcess.start_time == -1)
            {
                currentlyRunningProcess.start_time = getClk() - 1;
                fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
            }
            else
            {
                fprintf(logFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
            }
            runProcess();
            ppop(&pq);

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
                fprintf(logFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
                kill(currentlyRunningProcess.pid, SIGSTOP);
                currentlyRunningProcess = ppeek(&pq);
                if (currentlyRunningProcess.start_time == -1)
                {
                    currentlyRunningProcess.start_time = getClk() - 1;
                    fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
                }
                else
                {
                    fprintf(logFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
                }
                runProcess();
                ppop(&pq);

                if (pq == NULL)
                    pq = newNode(currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                else
                    ppush(&pq, currentlyRunningProcess, currentlyRunningProcess.remainig_time);
            }
            else
            {
                runProcess();
                ppop(&pq);

                if (pq == NULL)
                    pq = newNode(currentlyRunningProcess, currentlyRunningProcess.remainig_time);
                else
                    ppush(&pq, currentlyRunningProcess, currentlyRunningProcess.remainig_time);
            }
        }
        else // no recieved process this time step
        {

            runProcess();
            ppop(&pq);

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
        printPerformance();
        kill(getppid(), SIGINT);
    }
}

void RR(int quantum)
{
    struct process temp;
    while (!isQueueEmpty(&waitingQfront)) // arrival queue is not empty, receive and enqueue inside circular queue
    {
        recievedProcessthisTimeStep = true;

        temp = dequeueQueue(&waitingQfront, &waitingQrear);

        enqueueC(&readyQueueC, temp);
    }
    
    if (currentlyRunningProcess.id == -1) // if there is no running process, dequeue and run it
    {
        if (!isQueueCEmpty(&readyQueueC))
        {
            currentlyRunningProcess = dequeueC(&readyQueueC);
            printCQueue(readyQueueC);

            quantumCounter = 0;
            if (currentlyRunningProcess.start_time == -1)
            {
                currentlyRunningProcess.start_time = getClk() - 1;
                fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
            }
            else
            {
                fprintf(logFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
            }

            runProcess();
            quantumCounter++;
        }
    }
    else
    {

        if (quantumCounter < quantum)
        {

            runProcess();
            quantumCounter++;
        }
        else if (quantumCounter == quantum)
        {
            if (!isQueueCEmpty(&readyQueueC))
                fprintf(logFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
            kill(currentlyRunningProcess.pid, SIGSTOP);
            if (currentlyRunningProcess.remainig_time > 0)
                enqueueC(&readyQueueC, currentlyRunningProcess);

            int testID = currentlyRunningProcess.id;
            quantumCounter = 0;
            currentlyRunningProcess = dequeueC(&readyQueueC);
            printCQueue(readyQueueC);

            if (currentlyRunningProcess.start_time == -1)
            {
                currentlyRunningProcess.start_time = getClk() - 1;
                fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
            }
            else
            {
                if (testID != currentlyRunningProcess.id)
                    fprintf(logFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk() - 1, currentlyRunningProcess.id, currentlyRunningProcess.arrival_time, currentlyRunningProcess.running_time, currentlyRunningProcess.remainig_time, getClk() - 1 - currentlyRunningProcess.arrival_time - currentlyRunningProcess.running_time);
            }
            runProcess();
            quantumCounter++;
        }
    }
    recievedProcessthisTimeStep = false;

    // if queue is empty and remainingprocesses = 0
    if (remainingProcesses == 0 && isQueueCEmpty(&readyQueueC) && currentlyRunningProcess.id == -1)
    {

        printPerformance();
        kill(getppid(), SIGINT);
    }
}