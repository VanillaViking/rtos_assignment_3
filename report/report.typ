#align(center, text(20pt)[
  *Real-time Operating system - 48450*
])

#align(center, text(17pt)[
  Assignment 3 Report
])

#align(center, text(14pt)[
  Ashwin Rajesh - 14259321
])
#set heading(numbering: "1.1")

#show link: underline
#show link: set text(blue)
#show cite: set text(maroon)

#outline()
#pagebreak()

= Introduction

This assignment consists of two programs. Program 1 aims to simulate a round-robin CPU scheduling algorithm on 7 processes, with differing arrival times and burst times. The program then sends average wait and turnaround times to a different thread using a named pipe. Program 2 implements a FIFO page scheduling algorithm, and waits for a SIGINT interrupt from the user before outputting the total number of page faults from the algorithm. These results are summarised to show the strengths and weaknesses of algorithms for CPU and page scheduling in the realm of operating systems.

= Building the project

The code for both programs can be built using the makefile, as described below.
```bash
make -f makefile.txt
```

#figure(
  image("make.png", width: 70%),
  caption: [Build command],
) <make>

== Executing programs

Program 1:

```bash
./assign3_part1 4 output.txt
```
Program 2:
```bash
./assign3_part2 4
```
= Program 1

== Outline
Round Robin scheduling allocates each task an equal share of CPU time. At any given time, a process is selected and allowed to run on the CPU for a specified time quantum. After this time has passed, the task is 'pre-empted', and stopped mid-execution, after which the algorithm context-switches to a different process. This cycle is repeated until all processes in the ready-queue finish.

Once waiting times and turnaround times are calculated for each process, the averages are sent to thread 2 using a named pipe or FIFO. A named pipe is a method of inter-process communication in Unix systems, which is an extension to the traditional pipe concept. A named pipe can last as long as the system is up, beyond the life of the process.

== Gantt Chart

#figure(
  image("gantt.png", width: 100%),
  caption: [Round Robin Scheduling GANTT chart],
) <gantt>

== Implementation

=== Round Robin Scheduling (Thread 1)

The round robin algorithm presented in this assignment firstly initiates a ready queue as well as a current time variable. The current time variable keeps track of the time that has passed since the beginning of the algorithm. The ready queue is the data structure that holds all the processes that have arrived and are ready to be run. When a process has the same arrival time as the current time, it is added into the ready queue.

The algorithm pops the first process out of the ready queue and allows it to run for the specified time quantum. While the process executes, new processes that arrive are pushed to the back of the queue. When the time quantum for the current process ends, it's execution is paused and it is placed at the end of the ready queue. This process is repeated until all processes are finished executing. 

=== Output to file (Thread 2)

Once the algorithm has finished simulating round robin scheduling on the processes, the wait times and turnaround times that have been calculated for each process are averaged, and written to a named pipe. 

A named pipe or FIFO, is a method of inter-process communication which is an extension to the traditional pipe concept on Unix. It can last as long as the system is up, beyond the life of the process, and can be deleted if no longer used. In the program, the FIFO is initialised using the ```c mkfifo()``` command, which makes a special file using the given path. It is operated in a similar manner to an ordinary file, however, it has to be open at both ends simultaneously before input/output operations can take place.

In the worker2 thread, this filename can be used to read the contents that have been written to it using the ```c read()``` function. In the program, the average wait time and turnaround time is passed in by separating the two values using a comma. This is then decoded by the ```c strtok()``` function, which takes the source string and splits it according to a delimiter string. This way, worker2 is able to receive average wait and turnaround times and write it to the output file.

=== Results

#figure(
  image("prog1_results.png", width: 70%),
  caption: [Round Robin wait & turnaround times],
) <avgs>



= Program 2

== Outline

== Implementation

=== Memory management

=== Signal (Ctrl-C)

=== Results

= Conclusion
