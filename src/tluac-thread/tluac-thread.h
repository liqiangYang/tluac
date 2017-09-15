#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define DEFAULT_PORT 8888
#define MAXLINE 4096
pthread_t thread[2];
pthread_mutex_t mut;
void *thread1();
void *thread2();
void thread_create(void);
void thread_wait(void);
int    socket_fd;

void thread_new();
