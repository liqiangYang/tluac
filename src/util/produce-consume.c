#include "produce-consume.h"
/* 初始化缓冲区结构 */
void init(struct prodcons *b) {
	pthread_mutex_init(&b->lock, NULL);
	pthread_cond_init(&b->notempty, NULL);
	pthread_cond_init(&b->notfull, NULL);
	b->readpos = 0;
	b->writepos = 0;
}
/* 将产品放入缓冲区,这里是存入一个整数*/
void put(struct prodcons *b, int data) {
	pthread_mutex_lock(&b->lock);
	/* 等待缓冲区未满*/
	if ((b->writepos + 1) % BUFFER_SIZE == b->readpos) {
		pthread_cond_wait(&b->notfull, &b->lock);
	}
	/* 写数据,并移动指针 */
	b->buffer[b->writepos] = data;
	b->writepos++;
	if (b->writepos >= BUFFER_SIZE)
		b->writepos = 0;
	/* 设置缓冲区非空的条件变量*/
	pthread_cond_signal(&b->notempty);
	pthread_mutex_unlock(&b->lock);
}
/* 从缓冲区中取出整数*/
int get(struct prodcons *b) {
	int data;
	pthread_mutex_lock(&b->lock);
	/* 等待缓冲区非空*/
	if (b->writepos == b->readpos) {
		pthread_mutex_unlock(&b->lock);
		return 0;
		pthread_cond_wait(&b->notempty, &b->lock);
	}
	/* 读数据,移动读指针*/
	data = b->buffer[b->readpos];
	b->readpos++;
	if (b->readpos >= BUFFER_SIZE)
		b->readpos = 0;
	/* 设置缓冲区未满的条件变量*/
	pthread_cond_signal(&b->notfull);
	pthread_mutex_unlock(&b->lock);
	return data;
}
