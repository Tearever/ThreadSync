/* *********************************************************************** *
 * cv_sample.c (sleep version):                                            *
 *                                                                         *
 *   A sample for pthareds and pthread condition variables.                *
 *                                                                         *
 *    compile: cc cv_sample.c                                              *
 *    Environment: Fedora-X (os.cs.siue.edu)                               *
 *                                                                         *
 *    March 15, 2024 (hfujino@siue.edu)                                    *
 *                                                                         *
 * *********************************************************************** */
#include <stdio.h>                 // for printf
#include <pthread.h>               // for pthread
#include <time.h>                  // for time_t

#define NUM_REPEATS     7   // times to repeat
#define TIME_TO_WAIT    2   // wait time for pthread_cond_timedwait (in sec.)

pthread_mutex_t mutex1;            // mutex semaphore1
pthread_mutex_t mutex2;            // mutex semaphore1

pthread_cond_t condition1;         // condition variable
pthread_cond_t condition2;         // condition variable

struct timespec timeToWait;        // for pthread "pthread_cond_timedwait"
time_t T;                          // time_t variable

/* declare the two threads (T1 and T2) ------------------------------------ */
void T1 (void * arg);              // declaration of T1
void T2 (void * arg);              // declaration of T2

// MAIN ///////////////////////////////////////////////////////////////////////
void main(void)
{
    pthread_t T1_thread, T2_thread;       // decalre the thread identifiers

    pthread_mutex_init(&mutex1, NULL);     // initialize (pthread) semaphore
    pthread_cond_init(&condition1, NULL);  // initialize a condition variable

    pthread_mutex_init(&mutex2, NULL);     // initialize (pthread) semaphore
    pthread_cond_init(&condition2, NULL);  // initialize a condition variable

    /* create two threads ------------------------------------------------- */
    pthread_create(&T1_thread, NULL, T1, NULL);    // create thread T1
    pthread_create(&T2_thread, NULL, T2, NULL);    // create thread T2
    
    /* -------------------------------------------------------------------- */
    printf("\n");
    printf("the parent thread has created two threads ... \n\n");

    /* wait second(s) before the parent lets T1 to start ------------------ */
    sleep (TIME_TO_WAIT);

    /* "go" to T1 thread -------------------------------------------------- */
    pthread_mutex_lock(&mutex1);
    pthread_cond_signal(&condition1);
    pthread_mutex_unlock(&mutex1);

    /* -------------------------------------------------------------------- */
    printf("\n");
    printf("the parent thread waits for the two threads to terminate ... \n\n");

    /* wait for the two threads to finish --------------------------------- */
    pthread_join(T1_thread, NULL);             // wait for T1 to terminate
    pthread_join(T2_thread, NULL);             // wait for T2 to terminate
    
    /* delete semaphores and condition variables -------------------------- */
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_cond_destroy(&condition1);
    pthread_cond_destroy(&condition2);

    /* declare the end of my work ----------------------------------------- */
    printf("\n");
    printf("the parent thread is terminating ................ \n\n");
}

// T1 thread ///////////////////////////////////////////////////////////////////
void T1(void* arg)
{
    int i;    // loop counter

    /* let T1 wait for the parent thread ---------------------------------- */
    pthread_mutex_lock(&mutex1);
    pthread_cond_wait(&condition1, &mutex1);

    /* -------------------------------------------------------------------- */
    printf("        T1 starts  ... \n\n");

    /* T1 starts its work ------------------------------------------------ */
    for (i = 0; i < NUM_REPEATS; i++)
    {
       // Perform "event" and notifies it to the receiver
       pthread_mutex_lock(&mutex2);
       pthread_cond_signal(&condition2);
       pthread_mutex_unlock(&mutex2);

       // I did it -----------------------------------
       printf("        T1 signaled T2 ...\n");

       /* wait for second(s) before T1 signals T2 next time -------------- */
       sleep (TIME_TO_WAIT);
    }

    /* declare the end of my job ------------------------------------------ */
    printf("        T1 is terminating ... \n");

    return NULL;
}

// T2 thread ///////////////////////////////////////////////////////////////////
void T2(void* arg)
{
    int i;    // loop counter

    /* -------------------------------------------------------------------- */
    printf("        T2 starts  ... \n");

     /* T1 starts its work ------------------------------------------------ */
    for (i = 0; i < NUM_REPEATS; i++)
    {
       // wait for the sender to perform "event" 
       pthread_mutex_lock(&mutex2);
       pthread_cond_wait(&condition2, &mutex2);
       pthread_mutex_unlock(&mutex2);

       // I got it ............................
       printf("        T2 is signaled by T1!\n\n");
    }
    
    /* declare the end of my job ------------------------------------------ */
    printf("        T2 is terminating ... \n\n");

    return NULL;
}
