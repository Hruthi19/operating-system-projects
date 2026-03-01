#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "matmul.h"

void compute_multiply_matrices(int start_idx, int end_idx, int *shared_output);

void child_process_core(int id,  int numProc, int *shared_output)
{
	    int rows = m / numProc;
    	int start_idx = id * rows;
    	int end_idx = (id + 1) * rows - 1;
    	if (id == numProc - 1)
    	{
        	end_idx = m - 1;
    	}
		printf("The child process (pid:%d) created to calculate job #(%d/%d).\n", getpid(), id, m);
		simulate_process_crash(id);
		
		compute_multiply_matrices(start_idx, end_idx, shared_output);
}

void mat_mult_multi_proc(int numProc)
{
		int pid[numProc];
		int status;
    	int shared_mem_key;
    	int *shared_output;
    	int proc_idx, row_idx, col_idx;

		shared_mem_key = shmget(IPC_PRIVATE, m * p * sizeof(int), IPC_CREAT | 0666);
		if (shared_mem_key < 0)
		{
			perror("shmget");
			exit(1);
		}
    
		shared_output = (int *)shmat(shared_mem_key, NULL, 0);
		if (shared_output == (int *)-1)
		{
			perror("Failed to attach shared memory");
			exit(1);
		}
		for (proc_idx = 0; proc_idx < m * p; proc_idx++)
    	{
        	shared_output[proc_idx] = 0;
    	}

		for(proc_idx = 0; proc_idx < numProc; proc_idx++)
		{
			pid[proc_idx] = fork();
			if(pid[proc_idx] == 0) {
				child_process_core(proc_idx, numProc, shared_output);
				exit(0);
			} else if(pid[proc_idx] < 0) {
					printf("Fork failed\n");
					exit(0);
			}
		}

		for(proc_idx = 0; proc_idx < numProc; proc_idx++)
		{
				if (waitpid(pid[proc_idx], &status, 0) != -1)
			{
				if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
				{
					printf("Process %d crashed. Retrying...\n", pid[proc_idx]);
					if ((pid[proc_idx] = fork()) == 0)
					{
						child_process_core(proc_idx, numProc, shared_output);
						exit(0);
					}
					proc_idx--;  
				}
			}
		}
		for (row_idx = 0; row_idx < m; row_idx++)
		{
			for (col_idx = 0; col_idx < p; col_idx++)
			{
				C_multi_proc[row_idx][col_idx] = shared_output[row_idx * p + col_idx];
			}
		}

    shmdt(shared_output);
    shmctl(shared_mem_key, IPC_RMID, NULL);

}
void compute_multiply_matrices(int start_idx, int end_idx, int *shared_output)
{
    int row_idx, col_idx, k_idx;
    for (row_idx = start_idx; row_idx <= end_idx; row_idx++)
    {
        for (col_idx = 0; col_idx < p; col_idx++)
        {
            shared_output[row_idx * p + col_idx] = 0;
            for (k_idx = 0; k_idx < n; k_idx++)
            {
                shared_output[row_idx * p + col_idx] += A[row_idx][k_idx] * B_tran[col_idx][k_idx];
            }
        }
    }
}

