// This file is used to mimic the StringBuffer bug in JDK1.4
// Author: Jie Yu (jieyu@umich.edu)

#include "stringbuffer.hpp"
#include <stdio.h>
#include <unistd.h>

StringBuffer *buffer = new StringBuffer("abc");

void *thread1 (void *args) {
		int n = 0;

  while (1) {
    buffer->erase(0, 3);
				usleep(100);
    buffer->append("abc");
				if(n++ % 100 == 0) printf("%dth try - Thread 1: erase and append: %d\n", n-1);
  }
}

void *thread2 (void *args) {
		int n;

  while (1) {
    StringBuffer *sb = new StringBuffer();
				usleep(100);
    sb->append(buffer);
				if(n++ % 100 == 0) printf("%dth try - Thread 2: create and append: %d\n", n-1);
  }
}

int main(int argc, char *argv[]) {
  pthread_t th1, th2;
  int rc1, rc2;

  rc1 = pthread_create(&th1, NULL, thread1, NULL);
  rc2 = pthread_create(&th2, NULL, thread2, NULL);

		
		pthread_join(th1, NULL);
		pthread_join(th2, NULL);
  return 0;
}

