#include <stdio.h>
#include <stdlib.h>
#include "matmul.h"

void child_process_core(int i)
{
		printf("The child process (pid:%d) created to calculate job #(%d/%d).\n", getpid(), i, m);
		simulate_process_crash(i);
		
		/** Design and implement child processes function.
			* Each child process takes care of a part of the calculation.
			* Send the result to the parent via IPC. 
			**/
}

void mat_mult_multi_proc(int numProc)
{
		int pid[numProc];
		int i, wstatus;

		for(i = 0; i < numProc; i++)
		{
				/** Fork [numProc] child processes and store their pid in pid[n].
					*  Child executes <child_process_core> function.
					*  Prepare IPC, if needed.
					**/
				
				pid[i] = fork();
				
				if(pid[i] == 0) {
					 child_process_core(i);
						exit(0);
				} else if(pid[i] < 0) {
						printf("Fork failed\n");
						exit(0);
				}
		}
		
		/** Parent process waits for the children processes.
			* Each child process sends the result to the parent via IPC, and the parent stores them into C_multi_proc.
			* Design and implement the crash recovery **/

		for(i = 0; i < numProc; i++)
		{
				waitpid(pid[i], &wstatus, 0);
		}
}

