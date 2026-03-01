#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include "matmul.h"

void *child_thread_core(void *arg)
{
		long i = (long) arg;

		printf("The child thread[%d] (tid:%d) created to calculate job #(%d/%d).\n", i, syscall(186), i, m);
		simulate_thread_crash(i);
		
		/** Design and implement child thread function.
			* Each child thread takes care of a part of the calculation.
			* Store the result in C_multi_thread. 
			**/
}

void mat_mult_multi_thread(int numThread)
{
		pthread_t threads[numThread];
		long i;
		
		/** Parent thread creates "m" child threads **/
		
		
		for(i = 0; i < numThread; i++)
		{
				pthread_create(&threads[i], NULL, &child_thread_core, (void*) i);
		}
		

		/** Parent thread waits for the children thread.
			* Design and implement the crash recovery **/

		 
		for(i = 0; i < numThread; i++)
		{
				pthread_join(threads[i], NULL);
		}
}

