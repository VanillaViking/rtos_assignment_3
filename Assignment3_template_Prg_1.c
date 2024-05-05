/******************************************************************************* 
The assignment 3 for subject 48450 (RTOS) in University of Technology Sydney(UTS) 
This is a template of Program_1.c template. Please complete the code based on 
the assignment 3 requirement. Assignment 3 

------------------------------Program_1.c template------------------------------
*******************************************************************************/

#include <pthread.h> 	/* pthread functions and data structures for pipe */
#include <unistd.h> 	/* for POSIX API */
#include <stdlib.h> 	/* for exit() function */
#include <stdio.h>	/* standard I/O routines */
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "queue.h"

typedef struct {
   int pid;
   float arrive_t, burst_t;
   float wait_t;
   float turn_around_t;
} process;

typedef struct RR_Params {
   //add your variables here
   int pipeFile[2]; // [0] for read and [1] for write. use pipe for data transfer from thread A to thread B
   process *process_list;
   int num_processes;
   int timequant;

} ThreadParams;

pthread_attr_t attr;

//TODO: revert optimisation level in makefile

void initializeData(ThreadParams *params, int argc, char** argv);
double rr_serve(process* proc_list, int list_len, int process_idx, int quant);


/* this function calculates Round Robin (RR) with a time quantum of 4, writes waiting time and turn-around time to the RR */
void *worker1(void *params)
{
   ThreadParams *tp = (ThreadParams*) params;
   Queue *ready_queue = queue_init(10);

   int finished_processes = 0;
   double current_time = 0;

   while (finished_processes < 7) {
      double duration = tp->timequant;
      int selected_process = -1;

      if (!is_empty(ready_queue)) {
	 selected_process = dequeue(ready_queue);
	 // allow the selected process to run, while putting other ready processes on wait
	 duration = rr_serve(tp->process_list, tp->num_processes, selected_process, tp->timequant);
      }
      current_time += duration;

      // check if any more processes are ready
      for (int n = 0; n < tp->num_processes; n++) {
	 process proc = tp->process_list[n];

	 if (proc.arrive_t <= current_time) {
	    //process has arrived
	    enqueue(ready_queue, n);
	 }
      }
      if (selected_process >= 0 && tp->process_list[selected_process].burst_t > 0) {
	 enqueue(ready_queue, selected_process);
      } else if (selected_process >= 0) {
	 // the process has finished executing
	 finished_processes++;
      }
   }
}

/* reads the waiting time and turn-around time through the RR and writes to text file */
void *worker2()
{
}

/* this main function creates named pipe and threads */
int main(int argc, char** argv)
{

	/* creating a named pipe(RR) with read/write permission */
	 ThreadParams params; 	 

	/* initialize the parameters */
	 initializeData(&params, argc, argv);
	
	/* create threads */
	 pthread_t tid[2]; // two threads
	 int thread_a_exit = pthread_create(&(tid[0]), &attr, &worker1, (void*)(&params));
	 int thread_b_exit = pthread_create(&(tid[1]), &attr, &worker2, (void*)(&params));

	 if (thread_a_exit != 0 || thread_b_exit != 0) {
	    printf("%d", thread_a_exit);
	    printf("%d", thread_b_exit);
	    fprintf(stderr, "Failed to create threads\n");
	    exit(1);
	 }
	
	/* wait for the thread to exit */
	 pthread_join(tid[0], NULL);
	 pthread_join(tid[1], NULL);

	 return 0;
}

double rr_serve(process* proc_list, int list_len, int process_idx, int quant) {
   double duration = quant + 0.0;
   if (proc_list[process_idx].burst_t < quant) {
      duration = proc_list[process_idx].burst_t; 
   }

   for (int n = 0; n < list_len; n++) {
      if (n == process_idx) {
	 proc_list[n].burst_t -= duration;
      } else {
	 proc_list[n].wait_t += duration;
      }
   }

   return duration
}


void initializeData(ThreadParams *params, int argc, char** argv) {

  /* Verify the correct number of arguments were passed in */
  if (argc != 2) {
    fprintf(stderr, "USAGE:./assign2 timequant output.txt\n");
    exit(1);
  }

  params->timequant = atoi(argv[1]);

  if (params->timequant <= 0) {
    fprintf(stderr, "invalid time quantum\n");
    exit(1);
  }

   // initialize pipe
   int result = pipe(params->pipeFile);
   if (result < 0){
      perror("pipe error\n");
      exit(1);
   }

   params->num_processes = 7;

   params->process_list = malloc(sizeof(process)*params->num_processes);/* allocate memory for variable process */
   params->process_list[0].pid=1;
   params->process_list[0].arrive_t=8;
   params->process_list[0].burst_t=10;

   params->process_list[1].pid=2;
   params->process_list[1].arrive_t=10;
   params->process_list[1].burst_t=3;

   params->process_list[2].pid=3;
   params->process_list[2].arrive_t=14;
   params->process_list[2].burst_t=7;

   params->process_list[3].pid=4;
   params->process_list[3].arrive_t=9;
   params->process_list[3].burst_t=5;
   
   params->process_list[4].pid=5;
   params->process_list[4].arrive_t=16;
   params->process_list[4].burst_t=4;
  
   params->process_list[5].pid=6;
   params->process_list[5].arrive_t=21;
   params->process_list[5].burst_t=6;
 
   params->process_list[6].pid=7;
   params->process_list[6].arrive_t=26;
   params->process_list[6].burst_t=2;

}
