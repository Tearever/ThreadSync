/* *********************************************************************** *
 * interrupt_hanlder.c                                                     *
 *                                                                         *
 *    A sample of the interrupt handler.                                   *
 *                                                                         *
 *    compile: cc handler.c                                                *
 *    Environment: Fedora-X (os.cs.siue.edu)                               *
 *                                                                         *
 *    March 15, 2024 (hfujino@siue.edu)                                    *
 *                                                                         *
 * *********************************************************************** */
#include <stdio.h>                       // for printf                    
#include <signal.h>                      // for signal, alarm
#include <pthread.h>                     // p-thread implementation

#define SCHEDULE_INTERVAL  2             // scheduling interval in second

/* prototypes --------------------------------------------------------- */
void clock_interrupt_handler(void);       // the interrupt handler 

/* the main (the parent thread) --------------------------------------- */
int main(int argc,char *argv[])
{
    /* a condition variable just for holding the parent process */
    pthread_mutex_t mutex;                // mutex semaphore
    pthread_cond_t condition;             // condition variable

    /* set up the condition variable -------------------------- */
    pthread_mutex_init(&mutex, NULL);     // initialize a semaphore
    pthread_cond_init(&condition, NULL);  // initialize a CV

    /* declare the activation of the interrupt handler -------- */
    printf("\n"); 
    printf("I am activating the interrupt handler .....\n");
    printf("the interrupt interval: %d second(s)\n", SCHEDULE_INTERVAL);  
    printf("\n\n"); 

    /* specify the clock interrupt to be sent to this process - */
    signal(SIGALRM, clock_interrupt_handler);

    /* set the interrupt interval to 1 second ----------------- */
    alarm(SCHEDULE_INTERVAL);

    /* I start sleeping forever ------------------------------- */
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&condition, &mutex);  // no one will wake me up

    /* just for making sure that I (the parent process) never do this */
    printf("I (the parent process) should never print this line .....\n");  
}
/* === END of main ==================================================== */

/* The interrupt handler for SIGALM interrupt ========================= */
void clock_interrupt_handler(void)
{
   /* I woke up --------------------------------------------------- */
   printf ("the interrupt handler woke up ..... \n");

   /* scheduler wakes up again one second later ------------------- */
   alarm(SCHEDULE_INTERVAL);
}

/* END OF THE INTERRUPT HANDLER ======================================= */


