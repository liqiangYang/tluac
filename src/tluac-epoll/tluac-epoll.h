#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "../util/produce-consume.h"
#include "../tluac-thread/tluac-thread.h"

#ifndef MAX_EVENTS
#define MAX_EVENTS 500

struct myevent_s;
struct context;

struct myevent_s {
	int fd;
	void (*call_back)(struct context *ctx, int fd, int events, void *arg);
	int events;
	void *arg;
	int status; // 1: in epoll wait list, 0 not in
	char buff[128]; // recv data buffer
	int len, s_offset;
	long last_active; // last active time
	int index;
	lua_State *corutine;
};

struct context {
	int threadId;
	int epollFd;
	struct myevent_s g_Events[MAX_EVENTS + 1];
	int event_num = -1;
	struct prodcons *buffer;
	lua_State *lua;
	char *on_connect;
	char *on_message;
	char *on_close;
};


typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

void RecvData(struct context *ctx, int fd, int events, void *arg);
void SendData(struct context *ctx, int fd, int events, void *arg);
int epoll_new(struct context *ctx, int listen);
void InitListenSocket(struct context *ctx, int epollFd);
void rmEvent(struct context *ctx, int index);
#endif
