#ifndef NODE_H
#define NODE_H


struct node 
{
    void *data;
    struct node * next;
};

typedef struct node* nodePTR;


nodePTR createNode(void* data);

#endif
