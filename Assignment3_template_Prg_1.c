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

// A structure to represent a queue
typedef struct {
    int head; 
    int tail;
    int size;
    int capacity;
    int* array;
} Queue;

Queue* queue_init(int capacity);
int dequeue(Queue* queue);
void enqueue(Queue* queue, int item);
int is_empty(Queue* queue);
int is_full(Queue* queue);

typedef struct {
   int pid;
   float arrive_t, burst_t;
   float wait_t;
   float turn_around_t;
   float last_active_t;
   int arrived;
} process;

typedef struct RR_Params {
   //add your variables here
   char* fifo_filename;
   process *process_list;
   int num_processes;
   int timequant;
   char* output_filename;
} ThreadParams;

pthread_attr_t attr;

void initializeData(ThreadParams *params, int argc, char** argv);
double rr_serve(process* proc_list, int process_idx, int quant, double current_time);
void sort(process p[], int num);


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
      
      // serve a process for a time quantum, or until it ends
      if (!is_empty(ready_queue)) {
	 selected_process = dequeue(ready_queue);
	 duration = rr_serve(tp->process_list, selected_process, tp->timequant, current_time);
      }
      current_time += duration;

      // check if any more processes are ready
      for (int n = 0; n < tp->num_processes; n++) {
	 process proc = tp->process_list[n];
	 if (!proc.arrived && proc.arrive_t <= current_time) {
	    tp->process_list[n].arrived = 1;
	    enqueue(ready_queue, n);
	 }
      }
      
      // put currently selected process back in queue if it is yet to finish
      // it is important that this is done AFTER pushing new processes into the queue
      if (selected_process >= 0) {
	 if (tp->process_list[selected_process].burst_t > 0) {
	    enqueue(ready_queue, selected_process);
	 } else {
	    finished_processes++;
	    tp->process_list[selected_process].turn_around_t = current_time - tp->process_list[selected_process].arrive_t;
	 }
      }
   }

   float avg_wait;
   float avg_turnaround;
   float total_wait = 0;
   float total_turnaround = 0;

   for (int i = 0; i < tp->num_processes; i++) {
      total_wait += tp->process_list[i].wait_t;
      total_turnaround += tp->process_list[i].turn_around_t;
   }

   avg_wait = total_wait / tp->num_processes;
   avg_turnaround = total_turnaround / tp->num_processes;

   int fifo_fd = open(tp->fifo_filename, O_WRONLY);

   char message[80];
   sprintf(message, "%.2f, %.2f", avg_wait, avg_turnaround);
   int write_result = write(fifo_fd, message, strlen(message)+1);
   if (write_result <= 0) {
      fprintf(stderr, "Failed to write to fifo.");
      exit(1);
   }
   close(fifo_fd);
   
   return NULL;
}

/* reads the waiting time and turn-around time through the RR and writes to text file */
void *worker2(void* params)
{
   ThreadParams *tp = (ThreadParams*) params;

   int fifo_fd = open(tp->fifo_filename, O_RDONLY);

   char message[80];
   if (read(fifo_fd, message, sizeof(message)) <= 0) {
      fprintf(stderr, "Failed to read from fifo.");
      exit(1);
   }

   char* delim = ",";

   float wait_t = atof(strtok(message, delim));
   float turnaround_t = atof(strtok(NULL, delim));

   FILE* output_fp = fopen(tp->output_filename, "w");

   printf("Wait Time: %.2f\nTurnaround Time: %.2f\n", wait_t, turnaround_t);
   printf("written to file %s\n", tp->output_filename);
   fprintf(output_fp, "Wait Time: %.2f\nTurnaround Time: %.2f\n", wait_t, turnaround_t);
   fclose(output_fp);

   return NULL;
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

// allow the selected process to run, & update it's waiting time
double rr_serve(process* proc_list, int process_idx, int quant, double current_time) {
   double duration = quant + 0.0;
   if (proc_list[process_idx].burst_t < quant) {
      duration = proc_list[process_idx].burst_t; 
   }

   proc_list[process_idx].burst_t -= duration;
   proc_list[process_idx].wait_t += current_time - proc_list[process_idx].last_active_t;
   proc_list[process_idx].last_active_t = current_time + duration;

   return duration;
}


void initializeData(ThreadParams *params, int argc, char** argv) {

  /* Verify the correct number of arguments were passed in */
  if (argc != 3) {
    fprintf(stderr, "USAGE:./assign2 timequant output.txt\n");
    exit(1);
  }

  params->timequant = atoi(argv[1]);
  params->output_filename = argv[2];

  if (params->timequant <= 0) {
    fprintf(stderr, "invalid time quantum\n");
    exit(1);
  }
   
   // remove file from previous sessions to ensure that fifo creation does not fail
   remove("/tmp/task3fifo");

   // initialize fifo
   params->fifo_filename = "/tmp/task3fifo";
   int fifo_result = mkfifo(params->fifo_filename, 0666);
   if (fifo_result != 0) {
    fprintf(stderr, "Failed to initialise named pipe.\n");
    exit(1);
   }

   params->num_processes = 7;

   params->process_list = malloc(sizeof(process)*params->num_processes);/* allocate memory for variable process */
   params->process_list[0].pid=1;
   params->process_list[0].arrive_t=8;
   params->process_list[0].burst_t=10;
   params->process_list[0].arrived=0;
   params->process_list[0].last_active_t=params->process_list[0].arrive_t;

   params->process_list[1].pid=2;
   params->process_list[1].arrive_t=10;
   params->process_list[1].burst_t=3;
   params->process_list[1].arrived=0;
   params->process_list[1].last_active_t=params->process_list[1].arrive_t;

   params->process_list[2].pid=3;
   params->process_list[2].arrive_t=14;
   params->process_list[2].burst_t=7;
   params->process_list[2].arrived=0;
   params->process_list[2].last_active_t=params->process_list[2].arrive_t;

   params->process_list[3].pid=4;
   params->process_list[3].arrive_t=9;
   params->process_list[3].burst_t=5;
   params->process_list[3].arrived=0;
   params->process_list[3].last_active_t=params->process_list[3].arrive_t;
   
   params->process_list[4].pid=5;
   params->process_list[4].arrive_t=16;
   params->process_list[4].burst_t=4;
   params->process_list[4].arrived=0;
   params->process_list[4].last_active_t=params->process_list[4].arrive_t;
  
   params->process_list[5].pid=6;
   params->process_list[5].arrive_t=21;
   params->process_list[5].burst_t=6;
   params->process_list[5].arrived=0;
   params->process_list[5].last_active_t=params->process_list[5].arrive_t;
 
   params->process_list[6].pid=7;
   params->process_list[6].arrive_t=26;
   params->process_list[6].burst_t=2;
   params->process_list[6].arrived=0;
   params->process_list[6].last_active_t=params->process_list[6].arrive_t;

   //sort by arrive time to simulate real world process scheduling
   sort(params->process_list, params->num_processes);

}

void sort(process p[], int num)
{
   int i,j;
   process temp;
   for (i = 0; i<num; i++)
   {
        for (j = i+1; j<num; j++)
        {
 
	if(p[i].arrive_t > p[j].arrive_t) 	
           {
                temp = p[i];
                p[i] = p[j];
                p[j] = temp;
           }
        }
   }
}

Queue* queue_init(int capacity) {
    Queue* queue = (Queue*) malloc(sizeof(Queue));

    queue->capacity = capacity;
    queue->head = queue->size = 0;
 
    queue->tail = capacity - 1;
    queue->array = (int*)malloc(
        queue->capacity * sizeof(int));
    return queue;
}

int is_full(Queue* queue)
{
    return (queue->size == queue->capacity);
}

int is_empty(Queue* queue)
{
    return (queue->size == 0);
}

// add an item to the queue.
void enqueue(Queue* queue, int item)
{
    if (is_full(queue))
        return;

    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->array[queue->tail] = item;
    queue->size = queue->size + 1;
}
 
int dequeue(Queue* queue)
{
    if (is_empty(queue))
        return -1;

    int item = queue->array[queue->head];
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}
