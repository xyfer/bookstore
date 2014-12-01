#ifndef BACKEND_H
#define BACKEND_H

#include "queue.h"


struct customer 
{
    char *name;
    int cid;           // customer ID
    float credit_limit;
    queuePTR orders;
    queuePTR failed_orders;
};

typedef struct customer* customerPTR;


struct receipt 
{
    char *title;
    float price;
    float remaining_credit;
};

typedef struct receipt* receiptPTR;


struct order 
{
    char *title;
    float price;
    int cid;
    char *category;
};

typedef struct order* orderPTR;

struct database 
{
    customerPTR customers[500];          // Doubt we'd have more than that!
};

typedef struct database* databasePTR;



orderPTR createOrder(char *, float, int, char *);
receiptPTR createReceipt(char *, float, float);
customerPTR createCustomer(char *, int, float);
databasePTR createDatabase(); 
void addCustomer(databasePTR, customerPTR); 
customerPTR findCustomer(databasePTR, int); 

#endif
