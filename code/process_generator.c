#include "headers.h"
#include "data_structures.h"
void clearResources(int);
struct process *processes;
int msgq;
pid_t clk_id, scheduler_id;
// This function reads the processes from the file and stores them in the processes list
int inputProcesses(struct process **processes)
{
    FILE *file = fopen("processes.txt", "r");
    if (file == NULL) // Check if the file exists
    {
        printf("Could not open file\n");
        return -1;
    }
    int count = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (buffer[0] == '#')
        {
            continue;
        }
        count++;
    }
    *processes = (struct process *)malloc(count * sizeof(struct process)); // creating process list to store data
    fseek(file, 0, SEEK_SET);
    fgets(buffer, sizeof(buffer), file); // reset file pointer to the beginning of the file (skip the first line)
    int i = 0;
    for (int i = 0; i < count; i++)
    {
        fscanf(file, "%d %d %d %d %d", &(*processes)[i].id, &(*processes)[i].arrival_time, &(*processes)[i].running_time, &(*processes)[i].priority, &(*processes)[i].mem_size);
        (*processes)[i].remainig_time = (*processes)[i].running_time;
        printf("%d %d %d %d %d\n", (*processes)[i].id, (*processes)[i].arrival_time, (*processes)[i].running_time, (*processes)[i].priority, (*processes)[i].mem_size);
    }
    fclose(file);
    return count;
}
//this function asks the user to enter the chosen algorithm and its parameters
void requestAlgorithm(int *chosenAlgorithm, int *quantumTime)
{
    printf("Please enter the chosen scheduling algorithm and its parameters\n");
    printf("1-HPF  2-SRTN  3-RR\n"); // 1 for HPF, 2 for SRTN, 3 for RR
    while (true)                     // loop until valid input
    {
        scanf("%d", chosenAlgorithm);
        if (*chosenAlgorithm != 1 && *chosenAlgorithm != 2 && *chosenAlgorithm != 3)
        {
            printf("Invalid input, please enter a valid number\n");
            continue;
        }
        break;
    }
    *quantumTime=-1;
    if (*chosenAlgorithm == 3)
    {
        printf("Please enter the quantum time:\n");
        scanf("%d", quantumTime);
    }
}
//this function creates the clock and scheduler processes
void forkClockScheduler(char *algorthmNo, char *quantum, char *countProcesses)
{
    int clk_pid = fork();
    if (clk_pid == -1)
    {
        perror("Error in fork");
        exit(-1);
    }
    else if (clk_pid == 0)
    {
        execl("./clk.out", "clk.out", NULL);
        sleep(3);
    }
    else
    {
        int scheduler_pid = fork();
        if (scheduler_pid == -1)
        {
            perror("Error in fork");
            exit(-1);
        }
        else if (scheduler_pid == 0)
        {
            execl("./scheduler.out", "scheduler.out", algorthmNo, quantum, countProcesses, NULL);
        }
    }
}
// This function sends the process to the scheduler
void sendToScheduler(int msqid, struct process process)
{
    struct msgbuff message;
    message.mtype = 1;
    message.process = process;
    if (msgsnd(msqid, &message, sizeof(message.process), !IPC_NOWAIT) == -1)
    {
        perror("Error in send");
        exit(-1);
    }
}
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    key_t key = ftok("keyFile", msgqKey);
    msgq = msgget(key, IPC_CREAT | 0666);
    int countProcesses = inputProcesses(&processes);
    printf("Processes count: %d\n", countProcesses);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int chosenAlgorithm, quantumTime;
    requestAlgorithm(&chosenAlgorithm, &quantumTime);
    // 3. Initiate and create the scheduler and clock processes.
    char chosenAlgorithmString[10], quantumTimeString[10], countProcessesString[10];

    sprintf(chosenAlgorithmString, "%d", chosenAlgorithm);
    sprintf(quantumTimeString, "%d", quantumTime);
    sprintf(countProcessesString, "%d", countProcesses);
    forkClockScheduler(chosenAlgorithmString, quantumTimeString, countProcessesString);
    initClk();
    printf("Clock and Scheduler created\n");
    // 4. Use this function after creating the clock process to initialize clock
    // To get time use this
    // initClk();
    // TODO Generation Main Loop
    int i = 0;
    while (i < countProcesses)
    {
        int currentTime = getClk();
        if (processes[i].arrival_time == currentTime)
        {
            sendToScheduler(msgq, processes[i]);
            printf("Sending process %d to the scheduler at time = %d\n", processes[i].id, getClk());
            i++;
        }
    }
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    while(1);
    destroyClk(false);
}

void clearResources(int signum)
{
    // Clears all resources
    kill(clk_id, SIGINT);
    kill(scheduler_id, SIGINT);
    free(processes);
    destroyClk(true);
    msgctl(msgq, IPC_RMID, (struct msqid_ds *)0);
    exit(0);
}
