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
#include <stdio.h>                          // for printf
#include <pthread.h>                        // for pthread
#include <signal.h>                         // for time_t
#include <unistd.h>                         // for sleep function

/* Global variables ------------------------------------------------------- */
#define NUM_THREADS 5                       // Number of Threads
#define SCHEDULE_INTERVAL 1                 // scheduling interval in second
#define SLOWDOWN_FACTOR 6000000             // child thread display slowdown
int schedule_vector[NUM_THREADS];           // Declare Schedule Vector
int current_thread = 0;                     // The ID of the currently running thread
int schedule_index = 0;                     // The index in the schedule vector of the next thread to run
int loop_counter = 0;                       // loop counter for the interrupt handler
int interrupt_occurred = 0;                 // Flag to indicate if interrupt occurred

/* Mutex and condition variables ------------------------------------------- */
pthread_mutex_t mutex ;
pthread_cond_t cond[5] ;
pthread_mutex_t interrupt_mutex;
pthread_cond_t interrupt_cond;

pthread_mutex_t condition_mutex;
pthread_cond_t t_condtion;

/* prototypes -------------------------------------------------------------- */
void clock_interrupt_handler(int signal_number);
void *child_thread_routine(void *arg);

/* Main implementation --------------------------------------------------- */
int main(int argc, char *argv[]) {
    int i = 0;                              // the loop counter for the parent thread
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    /* Perform all initializations ------------------------------------ */
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&interrupt_mutex, NULL);
    pthread_mutex_init(&condition_mutex, NULL);
    pthread_cond_init(&interrupt_cond, NULL);
    pthread_cond_init(&t_condtion, NULL);
    for (int i = 0; i < 5; i++) {
        pthread_cond_init(&cond[i], NULL);
    }

    /* initialize the schedule vector --------------------------------- */
    schedule_vector[0] = 4;                 // the first thread
    schedule_vector[1] = 3;                 // the second thread
    schedule_vector[2] = 2;                 // the third thread
    schedule_vector[3] = 1;                 // the fourth thread
    schedule_vector[4] = 0;                 // the fifth thread

    /* create threads ------------------------------------------------- */
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, child_thread_routine, &ids[i]);
    }

    /* Specify the clock interrupt to be sent to this process --------- */
    signal(SIGALRM, clock_interrupt_handler);

    /* Set the interrupt interval -------------------------------------- */
    alarm(SCHEDULE_INTERVAL);

    /* Wait until the interrupt occurs ---------------------------------- */
    pthread_mutex_lock(&interrupt_mutex);
    while (!interrupt_occurred) {
        pthread_cond_wait(&interrupt_cond, &interrupt_mutex);
    }
    pthread_mutex_unlock(&interrupt_mutex);

    /* Now the interrupt occurred, let the child threads start ----------- */
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_cond_signal(&cond[i]);
    }

    /* Infinite loop for the parent thread -------------------------------- */
    while (1) {
        /* just for keeping the parent thread sleep forever ------ */
        pthread_mutex_lock(&condition_mutex);
            pthread_cond_wait(&t_condtion, &condition_mutex);
        pthread_mutex_unlock(&condition_mutex);

        i = i + 1;
    }

    /* Destroy mutexes and condition variables ----------------------------- */
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_cancel(threads[i]);         // Cancel child threads
        pthread_join(threads[i], NULL);     // Join child threads
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&interrupt_mutex);
    pthread_cond_destroy(&interrupt_cond);
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_cond_destroy(&cond[i]);
    }

    /* the main (parent) thread terminates itself ----------------------- */
    return 0;
}
/* END OF Main ---------------------------------------------------------- */

/* child thread implementation ------------------------------------------ */
void *child_thread_routine(void *arg) {
    int myid = *(int *)arg;                 // child thread number (not ID)
    int my_counter = 0;                     // local loop counter

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

    // Wait until the interrupt occurs
    pthread_mutex_lock(&interrupt_mutex);
    while (!interrupt_occurred) {
        pthread_cond_wait(&interrupt_cond, &interrupt_mutex);
    }
    pthread_mutex_unlock(&interrupt_mutex);

    /* infinite loop (required) ------------------------------- */
    while (1) {

        /* declare working of myself (required) ------------- */
        my_counter++;
        if ((my_counter % SLOWDOWN_FACTOR) == 0) 
        {printf("Thread: %d is running ...\n", myid);}

    /*------------------------------------------------------------*/
    /* Here is your working space (to implement some mechanism)   */
    /* to synchronize with the interrupt handler.                 */
    /*------------------------------------------------------------*/

        /* Wait for my turn to run -------------------------- */
        pthread_mutex_lock(&mutex);
        while (myid != current_thread) {
            pthread_cond_wait(&cond[myid], &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
/* END OF THE Child Thread Routine --------------------------------------- */

/* The interrupt handler for SIGALM interrupt ---------------------------- */
void clock_interrupt_handler(int signal_number) {
    /* Stop the currently running child thread --------------------- */
    pthread_mutex_lock(&mutex);
    current_thread = schedule_vector[schedule_index];
    schedule_index = (schedule_index + 1) % NUM_THREADS;    // Wrap around to the start of the schedule vector if necessary
    pthread_cond_signal(&cond[current_thread]);             // Signal the condition variable of the next thread to run
    pthread_mutex_unlock(&mutex);

    printf("\n\t\tI woke up on the timer interrupt (%d) ... \n", loop_counter++);

    /* Set the interrupt occurred flag and signal child threads ---- */
    pthread_mutex_lock(&interrupt_mutex);
    interrupt_occurred = 1;
    pthread_cond_broadcast(&interrupt_cond);
    pthread_mutex_unlock(&interrupt_mutex);

    /* scheduler wakes up again one second later ------------------- */
    alarm(SCHEDULE_INTERVAL);
}
/* END OF THE INTERRUPT HANDLER ------------------------------------------ */
