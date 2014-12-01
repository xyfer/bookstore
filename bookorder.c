#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "queue.h"
#include "backend.h"


databasePTR customerDatabase;
int num_categories = 0;


queuePTR queue;


int alldone;


char * category_array[50];  // MAX of 50 Categoriess/threads


int checkFile(char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) 
    {
        return 0;
    }
    else 
    {
        return 1;
    }
}



void *consumer(void *args) 
{

    customerPTR customer;
    orderPTR order;
    receiptPTR receipt;
    char *category;
    char *input;

    input = (char *) args;
    category = strdup(input);

    while (!alldone|| !queueEmpty(queue)) 
    {

        
        pthread_mutex_lock(&queue->mutex);

        if (alldone && queueEmpty(queue))  // we're done here boys
        {
           
            pthread_mutex_unlock(&queue->mutex);
            return NULL;
        }

        if (!alldone && queueEmpty(queue))  // queue is empty at the moment but theres more coming, wait on the NOTempty flag
        {
            pthread_cond_wait(&queue->NOTempty, &queue->mutex);
        }

        order = (orderPTR) queuepeek(queue);

        if (strcmp(order->category, category) != 0)    // should be handled by a different thread, isn't in this threads categroy
        {
            pthread_mutex_unlock(&queue->mutex);
            sched_yield();
        }

        else 
        {

            order = (orderPTR) dequeue(queue);
            customer = findCustomer(customerDatabase, order->cid);
            
            if (!customer) 
            {
                printf("Invalid customer ID error \n");
            }
            else  // If we get this far, we can actually attempt to do something!
            {
                receipt = createReceipt(order->title, order->price, customer->credit_limit - order->price);

                // Can't buy that shit

                if (customer->credit_limit < order->price) 
                {
                    printf("%s has insufficient funds to purchase: %s" "\n\t Remaining balance: $%.2f\n\n", customer->name, order->title,customer->credit_limit);
                    enqueue(customer->failed_orders, receipt);
                }

                // It's all yours

                else 
                {
                    customer->credit_limit -= order->price;
                    printf(" %s has bought: %s \n" "Price: $%.2f\n" "Remaining credit: $%.2f\n\n",customer->name, order->title,order->price, customer->credit_limit);
                    enqueue(customer->orders, receipt);
                }
            }
            pthread_mutex_unlock(&queue->mutex); // we're done with the queue, give it to another thread if they need it
        }
    }

    free(category); // reset category
    return NULL;
}


void *producer(void *args) {
    FILE *file;
    char *category;
    char *linebuffer;
    char *title;
    char *file_path;
    const char *delims = "|\n\r";
    float price;
    int c_id;
    orderPTR order;
    size_t len;
    ssize_t read;

    len = 0;
    file_path = (char*) args;
    int fileintegrity = checkFile(file_path);


   
    if (!fileintegrity) 
    {
        printf("Problem opening file :( \n");
        exit(EXIT_FAILURE);
    }

    file = fopen(file_path, "r");

    linebuffer = NULL;

    while ( (read = getline(&linebuffer, &len, file)) != -1 ) 
    {

        title = strtok(linebuffer, delims);
        price = atof(strtok(NULL, delims));
        c_id = atoi(strtok(NULL, delims));
        category = strtok(NULL, delims);

   /*    for (int i = 0; i < 500; i++)
       {
        if (!strcmp(category_array[i], category))
        {
            break;
        }
       }  */

        order = createOrder(title, price, c_id, category);
        pthread_mutex_lock(&queue->mutex);
        enqueue(queue,(void*) order);
        pthread_mutex_unlock(&queue->mutex);
        pthread_cond_signal(&queue->NOTempty);

    }

    alldone = 1;
    pthread_cond_broadcast(&queue->NOTempty);
    free(linebuffer);
    fclose(file);
    return NULL;
}



 // Sets up the customer database
 
databasePTR init_database(char *file_path) 
{
    size_t len;
    ssize_t chars;
    FILE *customerFile;
    char *token;
    char *linebuffer;
    char* name;
    float credit;
    int c_id;
    
    databasePTR db = createDatabase();

    //make sure filepath is referencing a valid file
    if (!checkFile(file_path)) 
    {
        printf("Error with filepath \n", file_path);
        exit(EXIT_FAILURE);
    }

    customerFile = fopen(file_path, "r");

    len = 0;
    linebuffer = NULL;

    while ( (chars = getline(&linebuffer, &len, customerFile) != -1) )  // tokenize for each line in input file
    {

        token = strtok(linebuffer, "|");                                           // name is first
        if (!token) {printf("Invalid customer file format \n"); exit(1);}
        name = strdup(token);
        printf("name: %s \n", name);

        token = strtok(NULL, "|");                                                         // then ID
        if (!token) {printf("Invalid customer file format \n"); exit(1);}
        c_id = atoi(token);
        printf("cid: %d \n", c_id);

        token = strtok(NULL, "|");                                                     // then credit limit
        if (!token) {printf("Invalid customer file format \n"); exit(1);}
        credit = atof(token);

        customerPTR newCustomer = createCustomer(name, c_id, credit);
        addCustomer(db, newCustomer);

    }
    free(linebuffer);
    fclose(customerFile);
    return db;
}

fillCategories(char* file_path)
{
    char* linebuffy;
    FILE* f;
    size_t chars;
    char* token;
    char* cat;
    size_t len = 0;
    linebuffy = NULL;

    if (!checkFile(file_path))
    {
        printf("Error with filepath \n", file_path);
        exit(EXIT_FAILURE);
    }

    f = fopen(file_path, "r");

     while ( (chars = getline(&linebuffy, &len, f)) != -1 ) 
     {
        token = strtok(linebuffy, "\n\r");
        cat = strdup(token);
        category_array[num_categories] = cat;
        num_categories++;
     }

     free(linebuffy);
}


int main(int argc, char **argv) {
    

    char *category;
    customerPTR customer;
    float revenue;
    int i;
    receiptPTR receipt;
    void *placeholder;
    revenue = 0.0f;

    if (argc != 4) 
    {
        fprintf(stderr, "Wrong number of arguments\n");
        exit(EXIT_FAILURE);
    }

   fillCategories(argv[3]);
   pthread_t thread_ids[num_categories + 1];
   customerDatabase = init_database(argv[1]);
   queue = createQueue();


    pthread_create(&thread_ids[0], NULL, producer, (void *) argv[2]);  // make the producer thread



    for (i = 0; i < num_categories; i++) 
    {
        pthread_create(&thread_ids[i + 1], NULL, consumer, category_array[i]);   // make all the consumers
    }

    for (i = 0; i < num_categories + 1; i++)  // wait for all the consumer threads to finish before continuing
    {
        pthread_join(thread_ids[i], &placeholder);
    }

    

    for (i = 0; i < 500; i++) 
    {
        customer = customerDatabase->customers[i];
        if (!customer) 
        {
            continue;
        }

        printf("\n~~~~~~~~~~ Customer Purchase History ~~~~~~~~~~\n \n");
        printf("Name: %s\n", customer->name);
        printf("Customer ID: %d \n", customer->cid);
        printf("Remaining credit: %.2f\n", customer->credit_limit);

        // Successful book orders

        printf("\n::: Successful orders :::\n \n");

        if (queueEmpty(customer->orders)) 
        {
                printf("\tNone.\n");
        }
        else 
        {
            while ((receipt = (receiptPTR) dequeue(customer->orders)) != NULL) 
            {
                printf("%s| %.2f| %.2f \n", receipt->title, receipt->price,receipt->remaining_credit);
                revenue += receipt->price;
            }
        }

        // Failed book orders
        printf("\n::: Failed orders :::\n \n");

        if (queueEmpty(customer->failed_orders)) 
        {
            printf("No unsuccessful orders.\n \n");
        }
        else 
        {
            while ((receipt = (receiptPTR) dequeue(customer->failed_orders)) != NULL)  // print all failed
            {
                printf("%s|%.2f\n", receipt->title, receipt->price);

            }
        }
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    }

    printf("Bookstore Revenue: $%.2f\n", revenue);

    return;
}
