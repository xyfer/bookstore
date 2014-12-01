#include "backend.h"
#include "queue.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


orderPTR createOrder(char *book_title, float price, int c_id, char *category) 
{
    orderPTR order = (orderPTR) malloc(sizeof(struct order)); 
    
        order->price = price;
        order->title = strdup(book_title);
        order->cid = c_id;
        order->category = strdup(category);  

    return order;
}


receiptPTR createReceipt(char *title, float price, float credit) {
    
    receiptPTR receipt = (receiptPTR) malloc(sizeof(struct receipt));

        receipt->remaining_credit = credit;
        receipt->title = strdup(title);
        receipt->price = price;

    return receipt;
}




customerPTR createCustomer(char *name, int c_id, float credit_limit) {
    customerPTR customer = (customerPTR) malloc(sizeof(struct customer));

        customer->name = strdup(name);
        customer->cid = c_id;
        customer->credit_limit = credit_limit;
        
        customer->orders = createQueue();
        customer->failed_orders = createQueue();

        return customer;
}



databasePTR createDatabase() 
{
    databasePTR database = (databasePTR) malloc(sizeof(struct database));
    return database;
}



void addCustomer(databasePTR db, customerPTR c) 
{
    db->customers[c->cid] = c;
}


customerPTR findCustomer(databasePTR db, int cid) 
{
        return db->customers[cid];
}
