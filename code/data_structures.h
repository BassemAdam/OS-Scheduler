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
    struct MemoryBlock* memoryBlock;
};

struct MemoryBlock {
    int start;
    int end;
    int size;
    int process_id;
    bool state;
    bool allocated;
    struct MemoryBlock* left;
    struct MemoryBlock* right;
    struct MemoryBlock* parent;
    struct MemoryBlock* buddy;
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
struct MemoryBlock* createBlock(int size, int start, int end, int process_id, bool state, bool allocated, struct MemoryBlock* left, struct MemoryBlock* right, struct MemoryBlock* parent);
struct MemoryBlock* findBestFit(struct MemoryBlock* root, int size);
struct MemoryBlock* occupyMemoryBlock(struct MemoryBlock* root, int size, int process_id);
bool freeMemoryBlock(struct MemoryBlock* root, int process_id);

#endif