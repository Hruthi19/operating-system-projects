#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#define MAX_THREAD 10

typedef struct {
		int *data;
} __thread_data;

int total;
__thread_data* thread_data;
int thread_states;  // 1: running, 0: terminating

void signalHandler(int sig) 
{
		int i;

		printf("Main: caught SIGINT (Ctrl+C). Exiting gracefully.\n");

		thread_states = 0;

		for(i = 0; i < MAX_THREAD; i++)
		{
				total+=thread_data[i].data[0];
				total+=thread_data[i].data[1];
				total+=thread_data[i].data[2];

				free(thread_data[i].data);
		}

		free(thread_data);
}

void* worker(void* arg) {
		
		long tid = (long)arg;
		
		srand(time(NULL)*tid);
		thread_data[tid].data[0] = 0;
		thread_data[tid].data[0] = 1;
		thread_data[tid].data[0] = 1;

		while(1) {
				if(thread_states == 0) {
						printf("Thread #%d: terminating..\n", tid);
						return NULL;
				}

				thread_data[tid].data[0] += rand()%10;
				thread_data[tid].data[1] += rand()%10;
				thread_data[tid].data[2] += rand()%10;
				usleep(1000);
		}
		return NULL;
}

int main() {

		long i;
		pthread_t thread[MAX_THREAD];
		
		if(signal(SIGINT, signalHandler) == SIG_ERR) {
				printf("Failed to register a signal hanlder.\n");
				return 0;
		}	
		
		total = 0;
		thread_data = malloc(sizeof(__thread_data)*MAX_THREAD);
		thread_states = 1;

		for(i = 0; i < MAX_THREAD; i++)
		{
				thread_data[i].data = (int*)malloc(sizeof(int)*3);
				pthread_create(&thread[i], NULL, worker, (void*)i);
		}
		

		for(i = 0; i < MAX_THREAD; i++)
		{
				pthread_join(thread[i], NULL);
		}
		
		printf("total = %d\n", total);
		return 0;
}

