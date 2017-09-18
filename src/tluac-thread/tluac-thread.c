#include "tluac-thread.h"
#include "../tluac-epoll/tluac-epoll.h"
#include "../tluac-lua/tluac-lua.h"
#include "../util/produce-consume.h"

struct prodcons buffer[THREADS];
pthread_t thread[THREADS + 1];
int listenFd;

void thread_new() {
	thread_create();
	thread_wait();
}

void thread_create(void) {
	int temp;
	memset(&thread, 0, sizeof(thread));
	struct context ctx_listen;
	/*创建线程*/
	if ((temp = pthread_create(&thread[0], NULL, thread_listen, &ctx_listen)) != 0)
		printf("线程 监听 创建失败!\n");
	else
		printf("线程 监听 被创建\n");

	int i = 0;
	for (i = 1; i < THREADS + 1; i++) {
		init(&buffer[i]);
		struct context ctx;
		ctx.buffer = buffer[i];

		if ((temp = pthread_create(&thread[i], NULL, thread_worker, &ctx)) != 0)
			printf("线程 %d 创建失败!\n", i);
		else
			printf("线程 %d 被创建\n", i);
	}
}

void thread_wait(void) {
	/*等待线程结束*/
	int i = 0;
	for (; i < THREADS + 1; i ++)
	{
		if (thread[i] != 0) {
			pthread_join(thread[i], NULL);
			printf("线程 %d 已经结束\n", i);
		}
	}
}

void *thread_listen(void *arg) {
	listenFd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(listenFd, F_SETFL, O_NONBLOCK); // set non-blocking
	printf("server listen fd=%d\n", listenFd);
	// bind & listen
	sockaddr_in sin;
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(8888);
	bind(listenFd, (const sockaddr*) &sin, sizeof(sin));
	listen(listenFd, 5);

	int l_epollFd;

	unsigned short port = 8888; // default port
	// create epoll
	l_epollFd = epoll_create(MAX_EVENTS);
	if (l_epollFd <= 0)
		printf("create epoll failed.%d\n", l_epollFd);
	// create & bind listen socket, and add to epoll, set non-blocking
	InitListenSocket(l_epollFd, port);

	struct context ctx = *(struct context *)arg;
	printf("thread listen :  %p \n", ctx);
	thread_new(ctx);

	return (void *) 0;
}

void *thread_worker(void *arg) {
	struct context ctx = *(struct context*) arg;
	printf("thread :  %p \n", ctx);

	epoll_new(ctx);

	return (void *) 0;
}
