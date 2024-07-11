/* ********************************************************** *
 * CS314 Project #2 solution                                  *
 * 							                                  *
 * Your Name: Trevor Wright                                   *
 * Your last-three:  560                                      *
 * Your course section #: 001                                 *
 *                                                            *
 * Spring 2024                                                *
 *                                                            *
 *                                                            *
 * ********************************************************** */
#include <stdio.h>      // for printf
#include <pthread.h>    // for pthread
#include <time.h>       // for time_t
#include <unistd.h>     // for sleep function
#include <signal.h>     // for signal, alarm

// Global variables
int schedule_vector[] = {4, 3, 2, 1, 0};        // Modify this as needed
int current_thread = 0;                         // The ID of the currently running thread
int schedule_index = 0;                         // The index in the schedule vector of the next thread to run
int loop_counter = 0;                           // loop counter for the interrupt handler

#define NUM_REPEATS  7                          // times to repeat
#define TIME_TO_WAIT 2                          // wait time for pthread_cond_timedwait (in sec.)
#define SCHEDULE_INTERVAL 1                     // scheduling interval in second
#define SLOWDOWN_FACTOR  6000000                // child thread display slowdown

// Mutex and condition variables
pthread_mutex_t mutex;
pthread_cond_t cond[5];

struct timespec timeToWait;         // for pthread "pthread_cond_timedwait"
time_t T;                           // time_t variable

int loop_counter;                   // loop counter for the interrupt handler

/* prototypes -------------------------------------------------------------- */
void clock_interrupt_handler(int singnal_number);      // the interrupt handler
void* child_thread_routine (void* arg);               // 



// MAIN ///////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {

    int i = 0;
    loop_counter = 0; // initalize the loop counter for the interrupt handler

    pthread_t threads[5];                   // decalre the thread identifiers
    int ids[5];

    // Perform all initializations
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < 5; i++) {
        pthread_cond_init(&cond[i], NULL);
    }

    /* initialize the schedule vector --------------------------------- */
    //schedule_vector[0] = 4;                         // the first thread
    //schedule_vector[1] = 3;                         // the second thread
    //schedule_vector[2] = 2;                         // the third thread
    //schedule_vector[3] = 1;                         // the fourth thread
    //schedule_vector[4] = 0;                         // the fifth thread

    /* create threads ------------------------------------------------- */
    for (int i = 0; i < 5; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, child_thread_routine, &ids[i]);
    }

    /* signal the condition variable to wake up T0_thread */
    //pthread_mutex_lock(&mutex1);
    //pthread_cond_signal(&condition1);
    //pthread_mutex_unlock(&mutex1);

   /* specify the clock interrupt to be sent to this process --- */
   signal(SIGALRM, clock_interrupt_handler);

   /* set the interrupt interval to 1 second --- */
   alarm(SCHEDULE_INTERVAL);

   /* infinite loop for the parent thread ------------------------------- */
   while(1)
   {

        /* just for keeping the parent thread sleep forever ------ */
        //pthread_mutex_lock(&condition_mutex);
            //pthread_cond_wait(&t_condtion, &condition_mutex);
        //pthread_mutex_unlock(&condition_mutex);

        i = i + 1;

   }

   /* delete semaphores and condition variables -------------------------- */
    //pthread_mutex_destroy(&condition_mutex);
    //pthread_mutex_destroy(&mutex1);
    //pthread_mutex_destroy(&mutex2);

    //pthread_cond_destroy(&t_condtion);
    //pthread_cond_destroy(&condition1);
    //pthread_cond_destroy(&condition2);


    /* the main (parent) thread terminates itself ----------------------- */
    return 0;
}

/* child thread implementation ------------------------------------------- */
void* child_thread_routine (void* arg) {
    int myid = *(int *) arg;    // child thread number (not ID)
    int my_counter = 0;         // local loop counter

    /*--------------------------------------------------------- */
    /* if you would like to add your local variable(s), you can */
    /* add one (them) here.                                     */
    /*----------------------------------------------------------*/

    /* declare the start of this child thread (required) ------ */
    printf("Child thread %d started ...\n", myid);

    /*-----------------------------------------------------------*/
    /* if you would like to perfrom some initialization to your  */
    /* local variable(s), you can do so here.                    */
    /*-----------------------------------------------------------*/
    pthread_mutex_lock(&mutex);
    while (myid != current_thread){
        pthread_cond_wait(&cond[myid], &mutex);     // Wait until it's this thread's turn to run
    }
    pthread_mutex_unlock(&mutex);
    
    /* infinite loop (required) ---------------------------------*/
    while(1){

        /* declare working of myself (required) ---------------- */
        my_counter++;
        if((my_counter % SLOWDOWN_FACTOR) == 0)
        {printf("Thread: %d is running ...\n", myid); }

    /*------------------------------------------------------------*/
    /* Here is your working space (to implement some mechanism)   */
    /* to synchronize with the interrupt handler.                 */
    /*------------------------------------------------------------*/
        pthread_mutex_lock(&mutex);
        while (myid != current_thread)
            pthread_cond_wait(&cond[myid], &mutex);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}



/* The interrupt handler for SIGALM interrupt -------------------------- */
void clock_interrupt_handler(int singnal_number)
{
    // Stop the currently running child thread
    pthread_mutex_lock(&mutex);
    current_thread = schedule_vector[schedule_index];
    schedule_index = (schedule_index + 1) % 5;          // Wrap around to the start of the schedule vector if necessary
    pthread_cond_signal(&cond[current_thread]);         // Signal the condition variable of the next thread to run
    pthread_mutex_unlock(&mutex);

   printf("\n\t\tI woke up on the timer interrupt (%d) ... \n", loop_counter++);

   /* scheduler wakes up again one second later ------------------- */
   alarm(SCHEDULE_INTERVAL);

}

/* END OF THE INTERRUPT HANDLER ---------------------------------------- */