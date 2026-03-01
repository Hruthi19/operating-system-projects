#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "Head.h"
#include "Data.h"
#include "Defs.h"
#include "Misc.h"
#include "Download.h"

extern sigset_t signal_set;

unsigned int bwritten = 0;
pthread_mutex_t bwritten_mutex = PTHREAD_MUTEX_INITIALIZER;

void handle_signal(int signal) {
    if (signal == SIGINT) {
        printf("Caught SIGINT, waiting for threads to finish...\n");
        // Wait for threads to finish here, ensure proper logging before exiting.
        // Add logic to wait for threads using pthread_join if necessary
    }
}

void setup_signal_handler() {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

void *http_get(void *arg) {
    struct thread_data *td;
    int sd;
    char *rbuf, *s;
    int dr, dw, i;
    long foffset;
    pthread_t tid;
    sigset_t set;

    tid = pthread_self();

    // Block out all signals
    sigfillset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    // Set Cancellation Type to Asynchronous
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    td = (struct thread_data *)arg;
    foffset = td->foffset;

    if (td->soffset < 0 || td->soffset >= td->foffset) {
        td->status = STAT_OK;
        pthread_exit((void *)1);
        return NULL;
    }

    rbuf = (char *)calloc(MAXBUFSIZ, sizeof(char));
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        Log("<THREAD #%ld> socket creation failed: %s", tid, strerror(errno));
        pthread_exit((void *)1);
    }

    if ((connect(sd, (const struct sockaddr *)&td->sin, sizeof(struct sockaddr))) == -1) {
        Log("<THREAD #%ld> connection failed: %s", tid, strerror(errno));
        close(sd);  // Close socket to prevent resource leak
        pthread_exit((void *)1);
    }

    if ((send(sd, td->getstr, strlen(td->getstr), 0)) == -1) {
        Log("<THREAD #%ld> send failed: %s", tid, strerror(errno));
        close(sd);  // Close socket to prevent resource leak
        pthread_exit((void *)1);
    }

    if ((dr = recv(sd, rbuf, MAXBUFSIZ - 1, 0)) == -1) {
        Log("<THREAD #%ld> recv failed: %s", tid, strerror(errno));
        close(sd);  // Close socket to prevent resource leak
        pthread_exit((void *)1);
    }
    rbuf[dr] = '\0';

    handleHttpRetcode(rbuf);
    if ((strstr(rbuf, "HTTP/1.1 206")) == NULL) {
        fprintf(stderr, "Something unhandled happened, shutting down...\n");
        close(sd);  // Close socket to prevent resource leak
        free(rbuf); // Free allocated buffer
        exit(1);
    }
    s = rbuf;
    i = 0;
    while ((s - rbuf) < dr) {
        if (*s == '\n' && *(s - 1) == '\r' && *(s - 2) == '\n' && *(s - 3) == '\r') {
            s++;
            i++;
            break;
        }
        s++;
        i++;
    }

    if ((dr - i) > foffset)
        dw = pwrite(td->fd, s, (foffset - i), td->soffset);
    else
        dw = pwrite(td->fd, s, (dr - i), td->soffset);
    td->offset = td->soffset + dw;

    pthread_mutex_lock(&bwritten_mutex);
    bwritten += dw;
    pthread_mutex_unlock(&bwritten_mutex);

    pthread_testcancel(); // Check for pending cancel requests

    while (td->offset < foffset) {
        fd_set set;
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        FD_ZERO(&set);
        FD_SET(sd, &set);
        select(FD_SETSIZE, &set, NULL, NULL, NULL);

        memset(rbuf, 0, MAXBUFSIZ);
        dr = recv(sd, rbuf, MAXBUFSIZ, 0);

        pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

        if ((td->offset + dr) > foffset)
            dw = pwrite(td->fd, rbuf, foffset - td->offset, td->offset);
        else
            dw = pwrite(td->fd, rbuf, dr, td->offset);
        td->offset += dw;
        sleep(1);
        pthread_mutex_lock(&bwritten_mutex);
        bwritten += dw;
        
        pthread_mutex_unlock(&bwritten_mutex);

        pthread_testcancel(); // Check for pending cancel requests
    }

    if (td->offset == td->foffset)
        td->status = STAT_OK;

    close(sd);
    free(rbuf);  // Free allocated buffer to avoid memory leak
    pthread_exit(NULL);
    return NULL;
}