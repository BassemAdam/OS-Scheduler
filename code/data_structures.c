#include "data_structures.h"


// Priority Queue
pNode *newNode(struct process d, int p)
{
    pNode *temp = (pNode *)malloc(sizeof(pNode));
    temp->data = d;
    temp->priority = p;
    temp->next = NULL;

    return temp;
}

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
// Circular Queue

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
    printf("Queue: ");
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

struct process peekC(struct QueueC **q)
{
    return (*q)->front->data;
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
// Normal Queue
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