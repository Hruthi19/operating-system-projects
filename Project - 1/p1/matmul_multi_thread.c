#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matmul.h"

#define MAX_RETRIES 5

typedef struct {
    long idx;
    int numThread;
    int *done;
    pthread_mutex_t *mutex;
} Thread;

void *child_thread_core(void *arg) {
    Thread *data = (Thread *)arg;
    long index = data->idx;
    int numThread = data->numThread;
    int *done = data->done;
    pthread_mutex_t *mutex = data->mutex;

    printf("The child thread[%ld] created to calculate job #%ld.\n", index, index);

    // Simulate crash
    if (rand() % 100 < crashRate) {
        printf("Thread[%ld] crashed.\n", index);
        pthread_mutex_lock(&mutex[index]);
        done[index] = 0;  
        pthread_mutex_unlock(&mutex[index]);
        pthread_exit(NULL);
    }

    
    long row, col;
    int start = (m / numThread) * index;  
    int end = (index == numThread - 1) ? m : (m / numThread) * (index + 1);  

    // matrix multiplication
    for (row = start; row < end; row++) {
        for (col = 0; col < p; col++) {
            C_multi_thread[row][col] = linear_mult(A[row], B_tran[col], n);
        }
    }

    printf("Thread[%ld] completed work\n", index);
    pthread_mutex_lock(&mutex[index]);
    done[index] = 1;  
    pthread_mutex_unlock(&mutex[index]);
    return NULL;
}

void mat_mult_multi_thread(int numThread) {
    pthread_t *threads = malloc(numThread * sizeof(pthread_t));  
    Thread *threadArray = malloc(numThread * sizeof(Thread)); 
    int *done = malloc(numThread * sizeof(int));
    pthread_mutex_t *mutex = malloc(numThread * sizeof(pthread_mutex_t));
    int retries[numThread];
    int i;

    for (i = 0; i < numThread; i++) {
        done[i] = 0;
        retries[i] = 0;
        pthread_mutex_init(&mutex[i], NULL);
    }

    for (i = 0; i < numThread; i++) {
        threadArray[i].idx = i;
        threadArray[i].numThread = numThread;
        threadArray[i].done = done;
        threadArray[i].mutex = mutex;
        pthread_create(&threads[i], NULL, child_thread_core, &threadArray[i]);
    }

    //crash recovery
    int allDone = 0;
    while (!allDone) {
        allDone = 1;  

        for (i = 0; i < numThread; i++) {
            pthread_mutex_lock(&mutex[i]);
            if (done[i] == 0 ) {
                pthread_mutex_unlock(&mutex[i]);
                pthread_join(threads[i], NULL);  
                pthread_mutex_lock(&mutex[i]);
                if (done[i] == 0) {
                printf("Recreating thread[%d] to complete the task.\n", i);
                pthread_create(&threads[i], NULL, child_thread_core, &threadArray[i]);
                allDone = 0;  
            }
                
            }
            pthread_mutex_unlock(&mutex[i]);
        }
    }

    for (i = 0; i < numThread; i++) {
        if(done[i] ==0)
        {
            pthread_join(threads[i], NULL);
        } 
    }

    printf("All threads completed successfully.\n");

     for (i = 0; i < numThread; i++) {
        pthread_mutex_destroy(&mutex[i]);  
    }

    free(threads);
    free(threadArray);
    free(done);
}
