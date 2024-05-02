#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H
// Data Structures

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

void ppush(pNode **head, struct process d, int p);



#endif