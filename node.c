#include "node.h"
#include <stdlib.h>


nodePTR createNode(void * data) 
{
    nodePTR node = (nodePTR) malloc(sizeof(struct node));

        node->data = data;
        return node;
}



