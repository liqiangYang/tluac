#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../util/produce-consume.h"
#include "../tluac-lua/tluac-lua.h"
#define DEFAULT_PORT 8888
#define MAXLINE 4096
#define THREADS 5

struct prodcons buffer[THREADS + 1];
pthread_t thread[THREADS + 1];

void *thread_listen(void *arg);
void *thread_worker(void *arg);
void thread_create(void);
void thread_wait(void);
int    socket_fd;

void thread_new();
