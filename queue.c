#include <stdlib.h>

#include "node.h"
#include "queue.h"
#include "backend.h"
#include <pthread.h>
#include <stdio.h>

queuePTR createQueue() 
{  
    queuePTR q = (queuePTR) malloc(sizeof(struct queue));
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->NOTempty, NULL);
    q->head = NULL;

    return q;
}

enqueue(queuePTR q, void* data)
{
    nodePTR newNode = (nodePTR) createNode(data);

    if (!q)
    {
        return;        // no queue
    }

    if ((q->head) == NULL)   // nothing in q 
    {
        q->head = newNode;
        newNode->next = NULL;
        return;
    }

    else   // general case
    {
               nodePTR curr;

        for (curr = q->head; curr->next != NULL; curr = curr->next)   
        {
            // iterate to open node spot
        }

        curr->next = newNode;
        newNode->next = NULL;
        return;
    }
}

const void* dequeue(queuePTR q)
{

    if (!q || !(q->head))
    {
        return NULL; // no q :(
    }

    else
    {
        void* data;
        nodePTR deleteme = q->head;
        data = q->head->data;
        q->head = q->head->next;
        free(deleteme);
        return data;
    }


}


const void* queuepeek(queuePTR q)
{
    if (!q)
    {
        return; // no q :(
    }

    else
    {
        return q->head->data;
    }
}

int queueEmpty(queuePTR q) 
{
    if (q->head == NULL || !q)
    {
        return 1;
        
    }

    return 0;
}

