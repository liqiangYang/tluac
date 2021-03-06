//
// a simple echo server using epoll in linux
//
// 2009-11-05
// 2013-03-22:修改了几个问题，1是/n格式问题，2是去掉了原代码不小心加上的ET模式;
// 本来只是简单的示意程序，决定还是加上 recv/send时的buffer偏移
// by sparkling
//
#include "tluac-epoll.h"
// set event
void EventSet(struct myevent_s *ev, int fd, void (*call_back)(struct context *, int, int, void *),
		void *arg) {

	printf("EventSet %d\n",fd);
	ev->fd = fd;
	ev->call_back = call_back;
	ev->events = 0;
	ev->arg = arg;
	ev->status = 0;
	ev->last_active = time(NULL);
}
// add/mod an event to epoll
void EventAdd(int epollFd, int events, struct myevent_s *ev) {
	struct epoll_event epv = { 0, { 0 } };
	int op;
	epv.data.ptr = ev;
	epv.events = ev->events = events;
	if (ev->status == 1) {
		op = EPOLL_CTL_MOD;
	} else {
		op = EPOLL_CTL_ADD;
		ev->status = 1;
	}
	if (epoll_ctl(epollFd, op, ev->fd, &epv) < 0)
		printf("Event Add failed[fd=%d], evnets[%d]\n", ev->fd, events);
	else
		printf("Event Add OK[fd=%d], op=%d, evnets[%0X]\n", ev->fd, op, events);
}
// delete an event from epoll
int EventDel(int epollFd, struct myevent_s *ev) {
	struct epoll_event epv = { 0, { 0 } };
	//if (ev->status != 1)
	//	return 2;
	epv.data.ptr = ev;
	ev->status = 0;
	int t = epoll_ctl(epollFd, EPOLL_CTL_DEL, ev->fd, &epv);
	return t;
}
// accept new connections from clients
void AcceptConn(struct context *ctx, int fd, int events, void *arg) {
	struct sockaddr_in sin;
	socklen_t len = sizeof(struct sockaddr_in);
	int nfd, i;
	// accept
	if ((nfd = accept(fd, (struct sockaddr*) &sin, &len)) == -1) {
		if (errno != EAGAIN && errno != EINTR) {
		}
		printf("%s: accept, %d", __func__, errno);
		return;
	}
	do {
		for (i = 0; i < MAX_EVENTS; i++) {
			if (ctx->g_Events[i].status == 0) {
				break;
			}
		}
		if (i == MAX_EVENTS) {
			printf("%s:max connection limit[%d].", __func__, MAX_EVENTS);
			break;
		}
		// set nonblocking
		int iret = 0;
		if ((iret = fcntl(nfd, F_SETFL, O_NONBLOCK)) < 0) {
			printf("%s: fcntl nonblocking failed:%d", __func__, iret);
			break;
		}

		int threadId = nfd % THREADS + 1;
		put(&buffer[threadId], nfd);
		printf("put %d, to threadId : %d %p\n", nfd, threadId, &buffer[threadId]);

	} while (0);
	printf("new conn[%s:%d][time:%ld], pos[%d]\n", inet_ntoa(sin.sin_addr),
			ntohs(sin.sin_port), ctx->g_Events[i].last_active, i);
}
// receive data
void RecvData(struct context *ctx, int fd, int events, void *arg) {
	struct myevent_s *ev = (struct myevent_s*) arg;
	int len;
	// receive data
	len = recv(fd, ev->buff + ev->len, sizeof(ev->buff) - 1 - ev->len, 0);
	if (len > 0) {
		ev->len += len;
		ev->buff[len] = '\0';
		printf("Recv [%d]:%s\n", fd, ev->buff);

		lua_getglobal(ev->coroutine, ctx->on_message);
		lua_pushinteger(ev->coroutine, fd);
		lua_pushlstring(ev->coroutine, ev->buff, len);
		lua_call(ev->coroutine, 2, 0);

		// change to send event
//		EventSet(ev, fd, SendData, ev);
//		EventAdd(ctx->epollFd, EPOLLOUT  , ev);
	} else if (len == 0) {
		lua_getglobal(ev->coroutine, ctx->on_close);
		lua_pushinteger(ev->coroutine, ev->fd);
		lua_call(ev->coroutine, 1, 0);
		close(ev->fd);
		printf("[fd=%d] pos[%ld], closed gracefully %d.\n", fd, ev - ctx->g_Events, ctx->threadId);
		rmEvent(ctx, ev->index);
	} else if (errno != EAGAIN) {
		lua_getglobal(ev->coroutine, ctx->on_close);
		lua_pushinteger(ev->coroutine, ev->fd);
		lua_call(ev->coroutine, 1, 0);
		close(ev->fd);
		printf("[%d] recv[fd=%d] error[%d]:%s\n", ctx->threadId, fd, errno, strerror(errno));
		rmEvent(ctx, ev->index);
	}
}
// send data
void SendData(struct context *ctx, int fd, int events, void *arg) {
	struct myevent_s *ev = (struct myevent_s*) arg;
	int len;
	char t[100];
	sprintf(t, "%d = %s", ctx->threadId, ev->buff + ev->s_offset);
	// send data
	len = send(fd, t, ev->len - ev->s_offset + 4, 0);
	if (len >= 0) {
		printf("send[fd=%d], [%d<->%d]%s\n", fd, len, ev->len, ev->buff);
		ev->s_offset += len;
		if (ev->s_offset >= ev->len) {
			// change to receive event
			bzero(ev->buff, sizeof(ev->buff));
			ev->s_offset = 0;
			ev->len = 0;
			EventDel(ctx->epollFd, ev);
			EventSet(ev, fd, RecvData, ev);
			EventAdd(ctx->epollFd, EPOLLIN  , ev);
		}
	} else {
		printf("send[fd=%d,%d] error[%d]:%s\n", fd, ev->fd, errno, strerror(errno));
		close(ev->fd);
		EventDel(ctx->epollFd, ev);
		rmEvent(ctx, ev->index);
	}
}
void rmEvent(struct context *ctx, int index){
	int i = 0;
	for (i = index; i < ctx->event_num; i ++){
		if (index + 1 < MAX_EVENTS){
			ctx->g_Events[index] = ctx->g_Events[index + 1];
			ctx->g_Events[index].index = index;
		}
	}
	ctx->event_num -= 1;
}
void InitListenSocket(struct context *ctx, int fd) {
	printf("InitListenSocket %p %d\n", ctx, fd);
	EventSet(&ctx->g_Events[MAX_EVENTS], fd, AcceptConn,
			&ctx->g_Events[MAX_EVENTS]);
	// add listen socket
	EventAdd(ctx->epollFd, EPOLLIN, &ctx->g_Events[MAX_EVENTS]);
}
int epoll_new(struct context *ctx, int listen) {
	// event loop
	struct epoll_event events[MAX_EVENTS];
	int checkPos = 0;
	int epollFd;
	if (!listen){
		epollFd = epoll_create(MAX_EVENTS);
		if (epollFd <= 0)
			printf("create epoll failed.%d\n", epollFd);
		ctx->epollFd = epollFd;
	}
	else
	{
		epollFd = ctx->epollFd;
	}
	int i = 0;
	while (1) {
		// a simple timeout check here, every time 100, better to use a mini-heap, and add timer event
		long now = time(NULL);
		if (!listen){
			for (i = 0; i < 100; i++, checkPos++) // doesn't check listen fd
			{
				if (checkPos == MAX_EVENTS)
					checkPos = 0; // recycle
				if (ctx->g_Events[checkPos].status != 1)
					continue;
				long duration = now - ctx->g_Events[checkPos].last_active;
				if (duration >= 60) // 60s timeout
				{
					ctx->g_Events[checkPos].last_active = now;
	//				close(ctx->g_Events[checkPos].fd);
					printf("[threadId = %d %d][fd=%d] timeout[%ld--%ld] pos = %d\n", ctx->threadId, listen, ctx->g_Events[checkPos].fd,
							ctx->g_Events[checkPos].last_active, now, checkPos);
	//				EventDel(epollFd, &ctx->g_Events[checkPos]);
				}
			}
			int nfd = get(ctx->buffer);
			if (nfd > 0)
			{
				ctx->event_num += 1;
				ctx->g_Events[ctx->event_num].index = ctx->event_num;
				ctx->g_Events[ctx->event_num].coroutine = lua_coroutine(ctx);
				lua_getglobal(ctx->g_Events[ctx->event_num].coroutine,  ctx->on_connect);
				lua_pushinteger(ctx->g_Events[ctx->event_num].coroutine, nfd);
				lua_call(ctx->g_Events[ctx->event_num].coroutine, 1, 0);
				// add a read event for receive data
				EventSet(&ctx->g_Events[ctx->event_num], nfd, RecvData, &ctx->g_Events[ctx->event_num]);
				EventAdd(ctx->epollFd, EPOLLIN , &ctx->g_Events[ctx->event_num]);
			}
		}
		// wait for events to happen
		int fds = epoll_wait(epollFd, events, MAX_EVENTS, 1000);
		if (fds < 0) {
			printf("epoll_wait error, exit\n");
			break;
		}
		for (i = 0; i < fds; i++) {
			struct myevent_s *ev = (struct myevent_s*) events[i].data.ptr;
			if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) // read event
			{
				printf("%s = %d %d\n", __func__, ev->fd, ctx->threadId);
				ev->call_back(ctx, ev->fd, events[i].events, ev->arg);
			}
			if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) // write event
			{
				ev->call_back(ctx, ev->fd, events[i].events, ev->arg);
			}
		}
	}
	// free resource
	return 0;
}

