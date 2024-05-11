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
//------------------------------UTILITY FUNCTIONS for MEMORY--------------------------------

struct MemoryBlockB *createMemoryBlock(int startAddress, int size)
{
    struct MemoryBlockB *block = (struct MemoryBlockB *)malloc(sizeof(struct MemoryBlockB));
    block->startAddress = startAddress;
    block->size = size;
    block->processId = -1; // -1 indicates the block is free
    block->is_free = true;
    block->parent = NULL;
    block->left = NULL;
    block->right = NULL;
    return block;
}

/*
 * This function traverses the binary tree of memory blocks to find the smallest block
 * that is large enough to accommodate the requested size and is currently free.
 * It uses a depth-first search strategy, checking both the left and right child nodes
 * and returning the smallest suitable block found.
 */
void printMemoryTree(struct MemoryBlockB *root, int level, char *prefix, bool isLeft)
{
    if (root == NULL)
    {
        return;
    }

    // Print the current node
    printf("%s", prefix);
    printf("%s [Block size: %d, process id: %d, is_free: %d]\n", isLeft ? "├──" : "└──", root->size, root->processId, root->is_free);

    // Prepare the prefixes for child nodes
    int prefixLength = strlen(prefix);
    char *childPrefix = malloc((prefixLength + 5) * sizeof(char));
    strcpy(childPrefix, prefix);
    strcat(childPrefix, isLeft ? "│   " : "    "); // Adds two spaces for indentation

    // Recursive call for the left child
    printMemoryTree(root->left, level + 1, childPrefix, true);

    // Recursive call for the right child
    printMemoryTree(root->right, level + 1, childPrefix, false);

    // Free dynamically allocated memory
    free(childPrefix);
}
struct MemoryBlockB *getSmallestSuitableBlock(struct MemoryBlockB *node, int size)
{
    // If the node has both left and right children
    if (node->left && node->right)
    {
        // Recursively search in the left and right subtrees
        struct MemoryBlockB *leftBlock = getSmallestSuitableBlock(node->left, size);
        struct MemoryBlockB *rightBlock = getSmallestSuitableBlock(node->right, size);

        // If no suitable block is found in either subtree, return NULL
        if (leftBlock == NULL && rightBlock == NULL)
        {
            return NULL;
        }
        // If no suitable block is found in the left subtree, return the block from the right subtree
        if (leftBlock == NULL)
        {
            return rightBlock;
        }
        // If no suitable block is found in the right subtree, return the block from the left subtree
        if (rightBlock == NULL)
        {
            return leftBlock;
        }
        // If suitable blocks are found in both subtrees, return the smallest one
        if (leftBlock->size <= rightBlock->size)
        {
            return leftBlock;
        }
        else
        {
            return rightBlock;
        }
    }
    else
    {
        // If the node has no children (i.e., it's a leaf node), check if it's large enough and free
        if (node->size >= size && node->processId == -1)
        {
            return node;
        }
        else
        {
            return NULL;
        }
    }
}

struct MemoryBlockB *occupyMemoryBlockB(struct MemoryBlockB *node, struct process *process)
{
    if (node == NULL)
    {
        return NULL;
    }

    int size = process->mem_size;
    int process_id = process->id;

    struct MemoryBlockB *temp = getSmallestSuitableBlock(node, size);
    if (temp == NULL)
    {
        return NULL;
    }
    while (temp->size / 2 >= size)
    {
        temp->left = createMemoryBlock(temp->startAddress, temp->size / 2);
        temp->left->parent = temp;
        temp->right = createMemoryBlock(temp->startAddress + temp->size / 2, temp->size / 2);
        temp->right->parent = temp;
        temp = temp->left;
    }
    temp->processId = process_id;
    // printf("\033[0;33mAt time %d allocated %d bytes for process %d from %d to %d\n\033[0m", getClk(), size, process_id, temp->startAddress, temp->startAddress + temp->size - 1);
    process->memoryBlock = temp;
    temp->is_free = false;
    struct MemoryBlockB *parent = temp->parent;
    while (parent != NULL)
    {
        parent->is_free = false;
        parent = parent->parent;
    }
    return temp;
}

bool freeMemoryBlockB(struct MemoryBlockB *node, int process_id)
{
    if (node == NULL)
    {
        return false;
    }
    if (node->processId == process_id)
    {
        node->processId = -1;
        node->is_free = true;
        printf("\033[0;33mFreed block for process %d from %d to %d\n\033[0m", process_id, node->startAddress, node->startAddress + node->size - 1);
        return true;
    }
    bool freedInLeft = freeMemoryBlockB(node->left, process_id);
    bool freedInRight = freeMemoryBlockB(node->right, process_id);
    if (node->left && node->right)
    {
        if (node->left->is_free && node->right->is_free)
        {
            // printf("\033[0;33m joined two memory slots from %d to %d and from %d to %d\n\033[0m", node->left->startAddress, node->left->startAddress + node->left->size - 1, node->right->startAddress, node->right->startAddress + node->right->size - 1);
            node->is_free = true;
            free(node->left);
            free(node->right);
            node->left = NULL;
            node->right = NULL;
        }
    }
    return freedInLeft || freedInRight;
}
struct process peekQueue(struct QueueNode **front)
{
    // Check if queue is empty
    if (isQueueEmpty(front))
    {
        struct process emptyProcess;
        emptyProcess.id = -1; // Indicate an empty process with id -1
        return emptyProcess;
    }

    // Return the process at the front of the queue
    return (*front)->data;
}
