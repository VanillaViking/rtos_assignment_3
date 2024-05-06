/******************************************************************************* 
  The assignment 3 for subject 48450 (RTOS) in University of Technology Sydney(UTS) 
  This is a framelate of Program_2.c framelate. Please complete the code based on 
  the assignment 3 requirement. Assignment 3 

  ------------------------------Program_2.c framelate------------------------------
 *******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include "queue.h"

//Number of pagefaults in the program
int pageFaults = 0;

//Function declaration
void SignalHandler(int signal);

/**
  Main routine for the program. In charge of setting up threads and the FIFO.

  @param argc Number of arguments passed to the program.
  @param argv array of values passed to the program.
  @return returns 0 upon completion.
  */
int main(int argc, char* argv[])
{
	//Register Ctrl+c(SIGINT) signal and call the signal handler for the function.
	//add your code here

	int i;
	// reference number
	int REFERENCESTRINGLENGTH=24;
	//Argument from the user on the frame size, such as 4 frameSize in the document
	int frameSize = atoi(argv[1]);
	//Frame where we will be storing the references. -1 is equivalent to an empty value
	uint frame[REFERENCESTRINGLENGTH];
	//Reference string from the assignment outline
	int referenceString[24] = {7,0,1,2,0,3,0,4,2,3,0,3,0,3,2,1,2,0,1,7,0,1,7,5};
	//Next position to write a new value to.
	int nextWritePosition = 0;
	//Boolean value for whether there is a match or not.
	bool match = false;
	//Current value of the reference string.
	int currentValue;

	//Initialise the empty frame with -1 to simulate empty values.
	for(i = 0; i < frameSize; i++)
	{
		frame[i] = -1;
	}

	//Loop through the reference string values.
	for(i = 0; i < REFERENCESTRINGLENGTH; i++)
	{
		int keep_value = 0;
		for(int n = 0; n < frameSize; n++)  
		{  
			if(referenceString[i] == frame[n]) //if the page = the current fame content/value 
			{  
				keep_value++;  		// indciate "don't change this frame content/value" 
				pageFaults--;  // -1 of pageFaults to balance the pageFaults++ 
			}  
		}  
        pageFaults++;  // increase 1 regardless "referenceString[m] == frame[n]"
	
	if (keep_value == 0) {
		if((pageFaults <= frameSize)) // string is counted within the 1st 3 frameSize  
		{  
			frame[i] = referenceString[i]; //just load the currnet page number into frame[m] 
		}  
		else // neither the page = farme content/value nor the 1st 3 frameSize!
		{  
			/* replace the current content/value of array frame[] with referenceString[m]
Ex: 3%3 = 0,frame[0]; 4%3=1,frame[1]; 5%3=2,frame[2]; 6%3=0,frame[0]; 7%3=1,frame[1]; 
8%3=2,frame[2]; 9%3=0,frame[0] with the pageFaults 3,4,5,6,7,8,9 . . . . . .  
*/
			frame[(pageFaults - 1) % frameSize] = referenceString[i]; 
		}  
	}

	printf("\nCurrent page: %d\n", referenceString[i]);
	printf("Frames:\n");
	for (int x = 0; x < frameSize; x++) {
		printf("%d\n", frame[x]);
	}
	}

	//Sit here until the ctrl+c signal is given by the user.
	while(1)
	{
		sleep(1);
	}

	return 0;
}

/**
  Performs the final print when the signal is received by the program.

  @param signal An integer values for the signal passed to the function.
  */
void SignalHandler(int signal)
{

	//add your code
	printf("\nTotal page faults: %d\n", pageFaults);
	exit(0);
}
