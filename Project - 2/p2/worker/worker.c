#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define MAX_THREAD 10

typedef struct {
    int *data;
} __thread_data;

int total;
__thread_data* thread_data;
int thread_states;  // 1: running, 0: terminating
pthread_mutex_t lock; // Mutex for thread safety

void signalHandler(int sig) {
    printf("Main: caught SIGINT (Ctrl+C). Exiting gracefully.\n");

    thread_states = 0; // Notify threads to terminate
	int i;
    // Lock the mutex to safely update shared data
    pthread_mutex_lock(&lock);
    for (i = 0; i < MAX_THREAD; i++) {
        total += thread_data[i].data[0];
        total += thread_data[i].data[1];
        total += thread_data[i].data[2];
        free(thread_data[i].data); 
    }
    free(thread_data); // Free the thread_data array
    pthread_mutex_unlock(&lock); // Unlock the mutex
}

void* worker(void* arg) {
    long tid = (long)arg;

    // Initialize random seed
    srand(time(NULL) * tid);
    sleep(1);
    // Allocate memory for thread data
    thread_data[tid].data = (int*)malloc(sizeof(int) * 3);
    thread_data[tid].data[0] = 0;
    thread_data[tid].data[1] = 1; // Fixed to set the right index
    thread_data[tid].data[2] = 1; // Fixed to set the right index

    while (1) {
        if (thread_states == 0) {
            printf("Thread #%ld: terminating..\n", tid);
            return NULL;
        }

        // Lock the mutex before updating shared data
        pthread_mutex_lock(&lock);
        thread_data[tid].data[0] += rand() % 10;
        thread_data[tid].data[1] += rand() % 10;
        thread_data[tid].data[2] += rand() % 10;
        pthread_mutex_unlock(&lock); // Unlock the mutex

        usleep(1000); // Sleep for 1 millisecond
    }
    return NULL;
}

int main() {
    long i;
    pthread_t thread[MAX_THREAD];

    // Register the signal handler for SIGINT
    if (signal(SIGINT, signalHandler) == SIG_ERR) {
        printf("Failed to register a signal handler.\n");
        return 0;
    }

    total = 0;
    thread_data = malloc(sizeof(__thread_data) * MAX_THREAD);
    thread_states = 1;

    pthread_mutex_init(&lock, NULL); // Initialize the mutex

    for (i = 0; i < MAX_THREAD; i++) {
        // Create threads
        pthread_create(&thread[i], NULL, worker, (void*)i);
    }

    for (i = 0; i < MAX_THREAD; i++) {
        pthread_join(thread[i], NULL);
        usleep(100000); 
    }

    printf("Total = %d\n", total);
    pthread_mutex_destroy(&lock); // Destroy the mutex
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <signal.h>
// #include <unistd.h>

// #define MAX_THREAD 10 

// typedef struct {
// 		int *data; 
// } __thread_data;

// int total; 
// __thread_data* thread_data; 
// int thread_states;  // 1: running, 0: terminating
// pthread_mutex_t lock;


// void signalHandler(int sig) 
// {
// 		int i;

// 		printf("Main: caught SIGINT (Ctrl+C). Exiting gracefully.\n");

// 		thread_states = 0; 

// 		for(i = 0; i < MAX_THREAD; i++)
// 		{
// 				total+=thread_data[i].data[0];
// 				total+=thread_data[i].data[1];
// 				total+=thread_data[i].data[2];

// 				free(thread_data[i].data);
// 		}

// 		free(thread_data);
// }

// void* worker(void* arg) {
		
// 		long tid = (long)arg;
		
// 		srand(time(NULL)*tid);
// 		thread_data[tid].data = (int*)malloc(sizeof(int) * 3);
// 		thread_data[tid].data[0] = 0;
// 		thread_data[tid].data[0] = 1;
// 		thread_data[tid].data[0] = 1;

// 		while(1) {
// 				if(thread_states == 0) {
// 						printf("Thread #%d: terminating..\n", tid);
// 						return NULL;
// 				}
// 				 pthread_mutex_lock(&lock);
// 				//  sleep(1);
// 				//generates random numbers and adds it to the
// 				thread_data[tid].data[0] += rand()%10;
// 				thread_data[tid].data[1] += rand()%10;
// 				thread_data[tid].data[2] += rand()%10;
// 				pthread_mutex_unlock(&lock);
// 				// usleep(1000);
// 		}
// 		return NULL;
// }

// int main() {

// 		long i;
// 		pthread_t thread[MAX_THREAD];
		
// 		if(signal(SIGINT, signalHandler) == SIG_ERR) {
// 				printf("Failed to register a signal hanlder.\n");
// 				return 0;
// 		}	
// 		pthread_mutex_init(&lock, NULL);
// 		total = 0;
// 		thread_data = malloc(sizeof(__thread_data)*MAX_THREAD);
// 		thread_states = 1;

// 		for(i = 0; i < MAX_THREAD; i++)
// 		{
// 				thread_data[i].data = (int*)malloc(sizeof(int)*3);
// 				pthread_create(&thread[i], NULL, worker, (void*)i);
// 				sleep(100);
// 		}
		

// 		for(i = 0; i < MAX_THREAD; i++)
// 		{
// 				pthread_join(thread[i], NULL);
// 		}
// 		pthread_mutex_lock(&lock);
//     for (i = 0; i < MAX_THREAD; i++) {
//         total += thread_data[i].data[0];
//         total += thread_data[i].data[1];
//         total += thread_data[i].data[2];

//         free(thread_data[i].data);
//     }
//     pthread_mutex_unlock(&lock);

//     free(thread_data);
//     printf("total = %d\n", total);
// 	pthread_mutex_destroy(&lock);	
// 		return 0;
// }



// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <signal.h>

// #define MAX_THREAD 10

// typedef struct {
// 		int *data;
// } __thread_data;

// int total;
// __thread_data* thread_data;
// int thread_states;  // 1: running, 0: terminating

// void signalHandler(int sig) 
// {
// 		int i;

// 		printf("Main: caught SIGINT (Ctrl+C). Exiting gracefully.\n");

// 		thread_states = 0;

// 		for(i = 0; i < MAX_THREAD; i++)
// 		{
// 				total+=thread_data[i].data[0];
// 				total+=thread_data[i].data[1];
// 				total+=thread_data[i].data[2];

// 				free(thread_data[i].data);
// 		}

// 		free(thread_data);
// }

// void* worker(void* arg) {
		
// 		long tid = (long)arg;
		
// 		srand(time(NULL)*tid);
// 		thread_data[tid].data[0] = 0;
// 		thread_data[tid].data[0] = 1;
// 		thread_data[tid].data[0] = 1;

// 		while(1) {
// 				if(thread_states == 0) {
// 						printf("Thread #%d: terminating..\n", tid);
// 						return NULL;
// 				}
// 				sleep(1);

// 				thread_data[tid].data[0] += rand()%10;
// 				thread_data[tid].data[1] += rand()%10;
// 				thread_data[tid].data[2] += rand()%10;
// 				usleep(1000);
// 		}
// 		return NULL;
// }

// int main() {

// 		long i;
// 		pthread_t thread[MAX_THREAD];
		
// 		if(signal(SIGINT, signalHandler) == SIG_ERR) {
// 				printf("Failed to register a signal hanlder.\n");
// 				return 0;
// 		}	
		
// 		total = 0;
// 		thread_data = malloc(sizeof(__thread_data)*MAX_THREAD);
// 		thread_states = 1;

// 		for(i = 0; i < MAX_THREAD; i++)
// 		{
// 				thread_data[i].data = (int*)malloc(sizeof(int)*3);
// 				pthread_create(&thread[i], NULL, worker, (void*)i);
// 		}
		

// 		for(i = 0; i < MAX_THREAD; i++)
// 		{
// 				pthread_join(thread[i], NULL);
// 		}
		
// 		printf("total = %d\n", total);
// 		return 0;
// }

