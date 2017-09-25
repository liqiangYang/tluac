#include "tluac-thread.h"
#include "../tluac-epoll/tluac-epoll.h"
#include "../util/produce-consume.h"

int listenFd;

void thread_new() {
	memset(&buffer, 0, sizeof(buffer) );
	thread_create();
	thread_wait();
}

struct context ctxs[THREADS + 1];

void thread_create(void) {
	int temp;
	memset(&thread, 0, sizeof(thread));

	int i = 0;
	for (i = 1; i < THREADS + 1; i++) {
		init(&buffer[i]);
		ctxs[i].buffer = &buffer[i];
		ctxs[i].threadId = i;
		ctxs[i].event_num = -1;

		if ((temp = pthread_create(&thread[i], NULL, thread_worker, &ctxs[i])) != 0)
			printf("线程 %d 创建失败!\n", i);
		else
			printf("线程 %d 被创建 buffer : %p %p\n", i, &ctxs[i],  &buffer[i]);
	}

	struct context ctx_listen;
	ctxs[0] = ctx_listen;
	/*创建线程*/
	if ((temp = pthread_create(&thread[0], NULL, thread_listen, &ctx_listen)) != 0)
		printf("线程 监听 创建失败!\n");
	else
		printf("线程 监听 被创建\n");

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
	if (bind(listenFd, (const sockaddr*) &sin, sizeof(sin)) < 0){
		printf("bind error [%d] %s\n", errno, strerror(errno));
	}
	if (listen(listenFd, 5) < 0){
	    printf("listen error [%d] %s\n", errno, strerror(errno));
	}

	int l_epollFd;

	// create epoll
	l_epollFd = epoll_create(MAX_EVENTS);
	if (l_epollFd <= 0)
		printf("create epoll failed.%d\n", l_epollFd);

	struct context *ctx = (struct context *)arg;
	ctx->epollFd = l_epollFd;
	printf("thread listen :  %p %d %ld\n", ctx, listenFd,l_epollFd);
	// create & bind listen socket, and add to epoll, set non-blocking
	InitListenSocket(ctx, listenFd);
	epoll_new(ctx, 1);

	return (void *) 0;
}

void *thread_worker(void *arg) {
	struct context *ctx = (struct context*) arg;
	printf("thread :  %p \n", ctx);

	luanew(ctx);
	luareg(ctx);
	luadofile(ctx, "t.lua");

	epoll_new(ctx, 0);

	return (void *) 0;
}
