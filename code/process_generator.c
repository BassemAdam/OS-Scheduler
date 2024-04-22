#include "headers.h"

void clearResources(int);

struct process
{
    int id;
    int arrival_time;
    int running_time;
    int priority;
};
struct process *processes;

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
        fscanf(file, "%d %d %d %d", &(*processes)[i].id, &(*processes)[i].arrival_time, &(*processes)[i].running_time, &(*processes)[i].priority);
        printf("%d %d %d %d\n", (*processes)[i].id, (*processes)[i].arrival_time, (*processes)[i].running_time, (*processes)[i].priority);
    }
    fclose(file);
    return count;
}

void requestAlgorithm(int *chosen_algorithm, int *quantum_time)
{
    printf("Please enter the chosen scheduling algorithm and its parameters\n");
    printf("1-HPF  2-SRTN  3-RR\n"); // 1 for HPF, 2 for SRTN, 3 for RR
    while (true)                     // loop until valid input
    {
        scanf("%d", chosen_algorithm);
        if (*chosen_algorithm != 1 && *chosen_algorithm != 2 && *chosen_algorithm != 3)
        {
            printf("Invalid input, please enter a valid number\n");
            continue;
        }
        break;
    }
    if (*chosen_algorithm == 3)
    {
        printf("Please enter the quantum time:\n");
        scanf("%d", quantum_time);
    }
}
void createClkScheduler(char *algorthmNo, char *quantum, char *countProcesses)
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
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    int count_processes = inputProcesses(&processes);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int chosen_algorithm, quantum_time;
    requestAlgorithm(&chosen_algorithm, &quantum_time);
    // 3. Initiate and create the scheduler and clock processes.
    char chosen_algorithm_str[10], quantum_time_str[10], count_processes_str[10];

    sprintf(chosen_algorithm_str, "%d", chosen_algorithm);
    sprintf(quantum_time_str, "%d", quantum_time);
    sprintf(count_processes_str, "%d", count_processes);
    createClkScheduler(chosen_algorithm_str, quantum_time_str, count_processes_str);
    initClk();
    // 4. Use this function after creating the clock process to initialize clock
    // To get time use this
    // initClk();
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    // Clears all resources in case of interruption
    free(processes);
    exit(0);
}
