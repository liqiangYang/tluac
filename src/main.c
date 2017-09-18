#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "tluac-thread/tluac-thread.h"
#include "tluac-epoll/tluac-epoll.h"
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

int listenFd;

int main() {
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

	thread_new();
	return 0;
}

