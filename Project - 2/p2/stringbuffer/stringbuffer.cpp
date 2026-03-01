// // This file is used to mimic the StringBuffer bug in JDK1.4
// // Author: Jie Yu (jieyu@umich.edu)

// #include "stringbuffer.hpp"

// #include <cassert>
// #include <cstdio>
// #include <cstring>
// #include <pthread.h>

// StringBuffer *StringBuffer::null_buffer = new StringBuffer("null");

// // pthread_mutex_t order_lock = PTHREAD_MUTEX_INITIALIZER;
// // pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
// // int turn = 1;

// StringBuffer::StringBuffer() {
// 		value = new char[16];
// 		value_length = 16;
// 		count = 0;
// 		pthread_mutex_init(&mutex_lock, NULL);
// }

// StringBuffer::StringBuffer(int length) {
// 		value = new char[length];
// 		value_length = length;
// 		count = 0;
// 		pthread_mutex_init(&mutex_lock, NULL);
// }

// StringBuffer::StringBuffer(const char *str) {
// 		int length = strlen(str) + 16;
// 		value = new char[length];
// 		value_length = length;
// 		count = 0;
// 		pthread_mutex_init(&mutex_lock, NULL);
// 		append(str);
// }

// int StringBuffer::length() {
// 		pthread_mutex_lock(&mutex_lock);
// 		int ret = count;
// 		pthread_mutex_unlock(&mutex_lock);
// 		return ret;
// }

// void StringBuffer::getChars(int srcBegin, int srcEnd,char *dst, int dstBegin) {
// 		pthread_mutex_lock(&mutex_lock);
// 		if (srcBegin < 0 || srcEnd < 0 || srcBegin > srcEnd || srcEnd > count) {
// 				pthread_mutex_unlock(&mutex_lock);  // Unlock before returning
// 				// fprintf(stderr, "Error: Invalid range in getChars: srcBegin=%d, srcEnd=%d, count=%d\n", srcBegin, srcEnd, count);
// 				return;
// 		}

// 		memcpy(dst + dstBegin, value + srcBegin, srcEnd - srcBegin);
// 		pthread_mutex_unlock(&mutex_lock);
// }

// StringBuffer *StringBuffer::append(StringBuffer *sb) {
// 		pthread_mutex_lock(&mutex_lock);
// 		if (sb == NULL) {
// 				sb = null_buffer;
// 		}

// 		int len = sb->length();
// 		if (len == 0) {  // Check if the string buffer is empty before appending
//         pthread_mutex_unlock(&mutex_lock);
//         return this;
//     }
// 		int newcount = count + len;
// 		if (newcount > value_length)
// 				expandCapacity(newcount);
// 		// fprintf(stderr, "Debug: StringBuffer count=%d before getChars call\n", count);
		
// 		sb->getChars(0, len, value, count);
// 		count = newcount;
// 		pthread_mutex_unlock(&mutex_lock);
// 		return this;
// }

// StringBuffer *StringBuffer::append(const char *str) {
// 		pthread_mutex_lock(&mutex_lock);
// 		if (str == NULL) {
// 				str = "null";
// 		}

// 		int len = strlen(str);
// 		int newcount = count + len;
// 		if (newcount > value_length)
// 				expandCapacity(newcount);

// 		memcpy(value + count, str, len);
// 		count = newcount;
// 		pthread_mutex_unlock(&mutex_lock);
// 		return this;
// }

// StringBuffer *StringBuffer::erase(int start, int end) {
// 		pthread_mutex_lock(&mutex_lock);
// 		// if (start < 0 || start > count || end < 0 || end > count || start > end)
// 		// {
// 		// 	pthread_mutex_unlock(&mutex_lock);
//         // fprintf(stderr, "Error: Invalid range in erase: start=%d, end=%d, count=%d\n", start, end, count);
//         // return this;
// 		// }
// 		if (start < 0)
// 				assert(0);
// 		if (end > count)
// 				end = count;
// 		if (start > end)
// 				assert(0);

// 		int len = end - start;
// 		if (len > 0) {
// 				memcpy(value + start, value + start + len, count - end);
// 				count -= len;
// 		}
// 		pthread_mutex_unlock(&mutex_lock);
// 		return this;
// }

// void StringBuffer::print() {
// 	pthread_mutex_lock(&mutex_lock);
// 		for (int i = 0; i < count; i++) {
// 				printf("%c", *(value + i));
// 		}
// 		printf("\n");
// 		pthread_mutex_unlock(&mutex_lock);
// }

// void StringBuffer::expandCapacity(int minimumCapacity) {
// 		int newCapacity = (value_length + 1) * 2;
// 		if (newCapacity < 0) {
// 				newCapacity = INTEGER_MAX_VALUE;
// 		} else if (minimumCapacity > newCapacity) {
// 				newCapacity = minimumCapacity;
// 		}

// 		char *newValue = new char[newCapacity];
// 		if (count >0)
// 		{
// 			memcpy(newValue, value, count);
// 		}
// 		delete[] value;
// 		value = newValue;
// 		value_length = newCapacity;
// 		fprintf(stderr, "Expanded buffer to new capacity: %d\n", newCapacity);
// }

// StringBuffer::~StringBuffer() {
// 		pthread_mutex_lock(&mutex_lock);
// 		delete[] value;
// 		pthread_mutex_unlock(&mutex_lock);
// 		pthread_mutex_destroy(&mutex_lock);
// 		if (this == null_buffer) {
//         delete null_buffer;
//     	}
// }

// void wait_for_turn(int thread_id) {
//     pthread_mutex_lock(&order_lock);
//     while (turn != thread_id) {
//         pthread_cond_wait(&cond, &order_lock);
//     }
	
//     pthread_mutex_unlock(&order_lock);
// }

// void signal_next(int next_thread_id) {
//     pthread_mutex_lock(&order_lock);
//     turn = next_thread_id;
//     pthread_cond_signal(&cond);
//     pthread_mutex_unlock(&order_lock);
// }

// StringBuffer::~StringBuffer() {
//      pthread_mutex_lock(&mutex_lock);
//     delete[] value;
//     pthread_mutex_unlock(&mutex_lock);
//     pthread_mutex_destroy(&mutex_lock);
//     // Clean up the global null_buffer
//     if (this == null_buffer) {
//         delete null_buffer;
//     }
// }

#include "stringbuffer.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <stdexcept>
#include <iostream>

StringBuffer *StringBuffer::null_buffer = new StringBuffer("null");

StringBuffer::StringBuffer() {
    value = new char[16];
    value_length = 16;
    count = 0;
    pthread_mutex_init(&mutex_lock, NULL);
}

StringBuffer::StringBuffer(int length) {
    value = new char[length];
    value_length = length;
    count = 0;
    pthread_mutex_init(&mutex_lock, NULL);
}

StringBuffer::StringBuffer(const char *str) {
    int length = strlen(str) + 1; // Include space for null terminator
    value = new char[length];
    value_length = length;
    count = 0;
    pthread_mutex_init(&mutex_lock, NULL);
    append(str);
}

StringBuffer::~StringBuffer() {
    
    delete[] value;
    pthread_mutex_destroy(&mutex_lock);
}

int StringBuffer::length() {
    pthread_mutex_lock(&mutex_lock);
    int ret = count;
    pthread_mutex_unlock(&mutex_lock);
    return ret;
}

bool StringBuffer::getChars(int srcBegin, int srcEnd, char *dst, int dstBegin) {
    pthread_mutex_lock(&mutex_lock);
    if (srcBegin < 0 || srcEnd < 0 || srcEnd > count || srcBegin > srcEnd) {
        pthread_mutex_unlock(&mutex_lock);
        return false;
 
    }
    memcpy(dst + dstBegin, value + srcBegin, srcEnd - srcBegin);
    pthread_mutex_unlock(&mutex_lock);
    return true;
}

StringBuffer *StringBuffer::append(StringBuffer *sb) {
    pthread_mutex_lock(&mutex_lock);
    if (sb == NULL) {
        sb = null_buffer;
    }

    int len = sb->length();
    if (len < 0) { // Check for negative length
        pthread_mutex_unlock(&mutex_lock);
        throw std::runtime_error("Invalid length for append.");
    }
    int newcount = count + len;
    if (newcount > value_length)
        expandCapacity(newcount);
    sb->getChars(0, len, value, count);
    count = newcount;
    pthread_mutex_unlock(&mutex_lock);
    return this;
}

StringBuffer *StringBuffer::append(const char *str) {
    pthread_mutex_lock(&mutex_lock);
    if (str == NULL) {
        str = "null";
    }

    int len = strlen(str);
    // if (len < 0) { // Check for negative length
    //     pthread_mutex_unlock(&mutex_lock);
    //     throw std::runtime_error("Invalid length for append.");
    // }
    int newcount = count + len;
    if (newcount > value_length)
        expandCapacity(newcount);
    memcpy(value + count, str, len);
    count = newcount;
    pthread_mutex_unlock(&mutex_lock);
    return this;
}

StringBuffer *StringBuffer::erase(int start, int end) {
    pthread_mutex_lock(&mutex_lock);
    if (start < 0 || end < 0 || end > count || start > end) {
        pthread_mutex_unlock(&mutex_lock);
        throw std::runtime_error("Invalid indices for erase.");
    }

    int len = end - start;
    if (len > 0) {
        memcpy(value + start, value + end, count - end);
        count -= len;
    }
    pthread_mutex_unlock(&mutex_lock);
    return this;
}

void StringBuffer::print() {
    pthread_mutex_lock(&mutex_lock);
    for (int i = 0; i < count; i++) {
        printf("%c", *(value + i));
    }
    printf("\n");
    pthread_mutex_unlock(&mutex_lock);
}

void StringBuffer::expandCapacity(int minimumCapacity) {
    pthread_mutex_lock(&mutex_lock);
    int newCapacity = (value_length + 1) * 2;
    if (newCapacity < 0) {
        newCapacity = INTEGER_MAX_VALUE;
    } else if (minimumCapacity > newCapacity) {
        newCapacity = minimumCapacity;
    }

    char *newValue = new char[newCapacity];
    
    memcpy(newValue, value, count);
    delete[] value;
    value = newValue;
    value_length = newCapacity;
    pthread_mutex_unlock(&mutex_lock);
}
