#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H
#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include <string.h>

typedef short bool;
#define true 1
#define false 0

// Data Structures
struct process
{
    int id;
    int arrival_time;
    int running_time;
    int priority;
    int remainig_time;
    int start_time;
    int mem_size;
    char *state;
    pid_t pid;
    struct MemoryBlockB* memoryBlock;
};

struct MemoryBlockB
{
    int startAddress;
    int size;
    int processId; // -1 if the block is free
    bool is_free;  // true if the block is free, false otherwise
    struct MemoryBlockB *parent;
    struct MemoryBlockB *left;
    struct MemoryBlockB *right;
};

struct msgbuff
{
    long mtype;
    struct process process;
};

//========================================================================================
// Priority Queue
typedef struct pnode
{

    struct process data;
    int priority;

    struct pnode *next;
} pNode;

pNode *newNode(struct process d, int p);

pNode *newNode(struct process d, int p);

void ppop(pNode **head);

struct process ppeek(pNode **head);

void ppush(pNode **head, struct process d, int p);

int pisEmpty(pNode **head);

//========================================================================================

// Circular Queue
struct QueueC
{
    struct pnode *front, *rear;
};

struct pnode *newQueueCNode(struct process data);

struct QueueC *createQueueC();

int isQueueCEmpty(struct QueueC **q);

void enqueueC(struct QueueC **q, struct process data);

struct process dequeueC(struct QueueC **q);

void printCQueue(struct QueueC *q);

struct process peekC(struct QueueC **q);

void deleteCNode(struct QueueC **q, int id);
//----------------------------------------------------------------------------------------

//------------------------------Normal Queue----------------------------------------------
struct QueueNode
{
    struct process data;
    struct QueueNode *next;
};

struct QueueNode *newQueueNode(struct process data);

int isQueueEmpty(struct QueueNode **front);

void enqueueQueue(struct QueueNode **front, struct QueueNode **rear, struct process data);

struct process dequeueQueue(struct QueueNode **front, struct QueueNode **rear);
//----------------------------------------Normal Queue end--------------------------------

//------------------------------------UTILITY FUNCTIONS for HPF------------------------------------
void deleteNode(struct pnode **head, int id);

void printQueue(struct pnode *pq);
//------------------------------------UTILITY FUNCTIONS FOR MEMORY OPERATIONS------------------------------------
struct MemoryBlockB *createMemoryBlock(int startAddress, int size);
void printMemoryTree(struct MemoryBlockB *root, int level, char *prefix, bool isLeft);
struct MemoryBlockB *getSmallestSuitableBlock(struct MemoryBlockB *node, int size);
struct MemoryBlockB *occupyMemoryBlockB(struct MemoryBlockB *node, struct process *process);
bool freeMemoryBlockB(struct MemoryBlockB *node, int process_id);
struct process peekQueue(struct QueueNode **front);
#endif