#include <stdio.h>
#include <pthread.h>
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 16 // 缓冲区数量
struct prodcons {
// 缓冲区相关数据结构
	int buffer[BUFFER_SIZE]; /* 实际数据存放的数组*/
	pthread_mutex_t lock; /* 互斥体lock 用于对缓冲区的互斥操作 */
	int readpos, writepos; /* 读写指针*/
	pthread_cond_t notempty; /* 缓冲区非空的条件变量 */
	pthread_cond_t notfull; /* 缓冲区未满的条件变量 */
};

void init(struct prodcons *b);
void put(struct prodcons *b, int data);
int get(struct prodcons *b);
#endif
