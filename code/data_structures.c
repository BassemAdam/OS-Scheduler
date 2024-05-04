#include "data_structures.h"
#include <stdbool.h>

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
//------------------------------UTILITY FUNCTIONS for MEMORY--------------------------------
struct MemoryBlockB
{
    int startAddress;
    int size;
    int processId;
    struct MemoryBlock *left;
    struct MemoryBlock *right;
};
struct MemoryBlockB *createMemoryBlock(int startAddress, int size)
{
    struct MemoryBlockB *block = (struct MemoryBlock *)malloc(sizeof(struct MemoryBlock));
    block->startAddress = startAddress;
    block->size = size;
    block->processId = -1; // No process is allocated to this block initially
    block->left = NULL;
    block->right = NULL;
    return block;
};
struct MemoryBlock* occupyMemoryBlockB(struct MemoryBlock* root, int size, int process_id) {
    if (root == NULL) {
        return NULL;
    }

    if (root->process_id == -1 && root->size == size) {
        root->process_id = process_id;
        printf("At time %d allocated %d bytes for process %d from %d to %d\n", getClk(), size, process_id, root->start, root->end);
        return root;
    }

    if (root->process_id != -1 || root->size < size) {
        return NULL;
    }

    root->left = createMemoryBlock(root->start, root->size / 2);
    root->right = createMemoryBlock(root->start + root->size / 2, root->size / 2);

    struct MemoryBlock* block = occupyMemoryBlock(root->left, size, process_id);
    if (block != NULL) {
        return block;
    }

    return occupyMemoryBlock(root->right, size, process_id);
}
bool freeMemoryBlockB(struct MemoryBlock* root, int process_id) {
    if (root == NULL) {
        return false;
    }

    if (root->process_id == process_id) {
        printf("At time %d freed %d bytes from process %d from %d to %d\n", getClk(), root->size, process_id, root->start, root->end);
        root->process_id = -1;  // Mark the block as free
        return true;
    }

    return freeMemoryBlock(root->left, process_id) || freeMemoryBlock(root->right, process_id);
}

struct MemoryBlock *createBlock(int size, int start, int end, int process_id, bool state, bool allocated, struct MemoryBlock *left, struct MemoryBlock *right, struct MemoryBlock *parent)
{
    struct MemoryBlock *block = (struct MemoryBlock *)malloc(sizeof(struct MemoryBlock));
    block->start = start;
    block->end = end;
    block->size = size;
    block->process_id = process_id;
    block->state = state;
    block->allocated = allocated;
    block->left = left;
    block->right = right;
    block->parent = parent;
    return block;
}
// Function to round up to the next highest power of 2
int nextPowerOf2(int n)
{
    if (n && !(n & (n - 1)))
    {
        return n;
    }

    int count = 0;
    while (n != 0)
    {
        n >>= 1;
        count += 1;
    }

    return 1 << count;
}

struct MemoryBlock *buddyAllocate(struct MemoryBlock *node, int size)
{
}

struct MemoryBlock *findBestFit(struct MemoryBlock *root, int size)
{
    if (root == NULL)
    {
        return NULL;
    }
    if (root->size < size)
    {
        return NULL;
    }
    if (root->size == size && !root->allocated)
    {
        return root;
    }
    struct MemoryBlock *left = findBestFit(root->left, size);
    struct MemoryBlock *right = findBestFit(root->right, size);
    if (left == NULL)
    {
        return right;
    }
    if (right == NULL)
    {
        return left;
    }
    if (left->size < right->size)
    {
        return left;
    }
    return right;
}

struct MemoryBlock *occupyMemoryBlock(struct MemoryBlock *root, int size, int process_id) //NOT TESTED YET
{
    struct MemoryBlock *bestFit = findBestFit(root, size);
    printf("Best fit block size: %d\n", bestFit->size);
    if (bestFit == NULL)
    {
        return NULL;
    }
    while (bestFit->size / 2 > size)
    {
        bestFit->left = createBlock(bestFit->size / 2, bestFit->start, bestFit->start + bestFit->size / 2, -1, false, false, NULL, NULL, bestFit);
        bestFit->right = createBlock(bestFit->size / 2, bestFit->start, bestFit->start + bestFit->size / 2, -1, false, false, NULL, NULL, bestFit);
        bestFit = bestFit->left;
    }
    bestFit->process_id = process_id;
    bestFit->allocated = true;
    return bestFit;
}

void freeMemoryBlock(struct MemoryBlock *root, int process_id) // NOT FINISHED YET AND NOT TESTED
{
    if (root == NULL)
    {
        return;
    }
    if (root->process_id == process_id)
    {
        root->allocated = false; // mark that there is no processes allocated in this block
        root->state = true;      // Mark the block as free ie:- no child blocks are allocated
        printf("Removed process %d from memory start %d and memory end %d\n", process_id, root->start, root->end);
        return;
    }
    freeMemoryBlock(root->left, process_id);
    freeMemoryBlock(root->right, process_id);
    if (root->left != NULL && root->right != NULL && root->left->state && root->right->state)
    {
        root->state = true;
        root->process_id = -1;
        root->allocated = false;
        free(root->left);
        free(root->right);
        root->left = NULL;
        root->right = NULL;
    }
}
