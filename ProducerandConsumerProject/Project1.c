/*
Syed Ali COSC 514 Project 1
Producer and Consumer Problem using pthreads/bounded-buffer/semaphores
Assignment Due 07/17/2020
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <semaphore.h>
#define TRUE 1
typedef int buffer_item;

#define BUFFER_SIZE 5

pthread_mutex_t In, Out;                                                                                                           // the mutex lock
sem_t full, empty;                                                                                                                 // the semaphores

buffer_item buffer[BUFFER_SIZE];                                                                                                   // the buffer
static int outPtr = 0;
static int inPtr = 0;
int counter;                                                                                                                       // buffer counter
pthread_t tid;                                                                                                                     // Thread ID
pthread_attr_t attr;                                                                                                               // Set of thread attributes

long i;

int insert_item(buffer_item item);
int remove_item(buffer_item *item);
void *producer(void *param);                                                                                                       // the producer thread
void *consumer(void *param);                                                                                                       // the consumer thread

int main(int argc, char *argv[])
{
    if(argc != 4)                                                                                                                  // Verify the correct number of arguments were passed in
    {
      fprintf(stderr, "Three arguments are required: 1 Sleep Time, 2 Number of Producers, 3 Number of Consumers\n");
        return 1;
    }
    
    int sleepTime = atoi(argv[1]);                                                                                                  // time in seconds for main to sleep
    int producerCount = atoi(argv[2]);                                                                                              // number of producer threads
    int consumerCount = atoi(argv[3]);                                                                                              // number of consumer threads
    
    if(sleepTime < 1 || producerCount < 1 || consumerCount < 1)
    {
      fprintf(stderr, "Please enter integers greater than 0 for all parameters\n");
        return 2;
    }
     
    pthread_mutex_init(&In, NULL);                                                                                                  // create the mutex lock
    pthread_mutex_init(&Out, NULL);
    sem_init(&full, 0, 0);                                                                                                          // create the full semaphore
    sem_init(&empty, 0, BUFFER_SIZE);                                                                                               // Create the empty semaphore
    pthread_attr_init(&attr);                                                                                                       // Get the default attributes
    
    for(i = 0; i < producerCount; i++)                                                                                              // create the producer threads
        pthread_create(&tid, &attr, producer, (void*) i);
    
    for(i = 0; i < consumerCount; i++)                                                                                              // create the consumer threads
        pthread_create(&tid, &attr, consumer, (void*) i);
    
    sleep(sleepTime);                                                                                                               // sleep for the specified amount of time
    
    printf("Program has exited\n");
    exit(0);
}

void *producer(void *param)
{
    long i = (long) param;
    buffer_item item;                                                                                                     
    while(1)
    {
        int r = rand() % 5 ;                                                                                                      // sleep for a random period of time
        sleep(r);
        item = rand();
        sem_wait(&empty);                                                                                                         //  acquire the empty lock
        pthread_mutex_lock(&In);                                                                                                  //  acquire the mutex lock

        if(insert_item(item))
            printf("Producer error");
        else
            printf("producer %ld produced %d\n", i, item);
        
        pthread_mutex_unlock(&In);                                                                                                // release the mutex lock
        sem_post(&full);                                                                                                          // signal full
    }
}

void *consumer(void *param)                                                                                                       // Consumer Thread
{
    long i = (long) param;
    buffer_item item;
    while(1)
    {
        int r = rand() % 5;                                                                                                       // sleep for a random period of time
        sleep(r);
        sem_wait(&full);                                                                                                          // acquire the full lock
        pthread_mutex_lock(&Out);                                                                                                 // acquire the mutex lock

        if(remove_item(&item))
            printf("Consumer error\n");
        else
            printf("consumer %ld consumer %d\n", i, item);
                   
        pthread_mutex_unlock(&Out);                                                                                              // release the mutex lock
        sem_post(&empty);                                                                                                        // signal empty
   }
}

int insert_item(buffer_item item)                                                                                                // add an item to the buffer
{
    
    buffer[outPtr] = item;
    outPtr = (outPtr + 1) % BUFFER_SIZE;
    return 0;
}

int remove_item(buffer_item *item)                                                                                              // item is pointed to here
{                                                                                                                               // removes an item from the buffer
    *item = buffer[inPtr];
    inPtr = (inPtr + 1) % BUFFER_SIZE;
    return 0;
}
