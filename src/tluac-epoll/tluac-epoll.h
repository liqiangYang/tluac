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

int listenFd;
int g_epollFd;
#define MAX_EVENTS 500
struct myevent_s g_Events[MAX_EVENTS + 1]; // g_Events[MAX_EVENTS] is used by listen fd
void RecvData(int fd, int events, void *arg);
void SendData(int fd, int events, void *arg);

int epoll_new();
