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

struct myevent_s {
	int fd;
	void (*call_back)(int fd, int events, void *arg);
	int events;
	void *arg;
	int status; // 1: in epoll wait list, 0 not in
	char buff[128]; // recv data buffer
	int len, s_offset;
	long last_active; // last active time
};

#define MAX_EVENTS 500

struct context {
	int epollFd;
	struct myevent_s g_Events[MAX_EVENTS + 1];
	struct prodcons buffer;
};

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

void RecvData(int fd, int events, void *arg);
void SendData(int fd, int events, void *arg);
int epoll_new();
