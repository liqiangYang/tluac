#include "thread.h"

void thread_new()
{

	struct sockaddr_in     servaddr;
	//初始化Socket
	if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}
	//初始化
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
	servaddr.sin_port = htons(DEFAULT_PORT);//设置的端口为DEFAULT_PORT

	//将本地地址绑定到所创建的套接字上
	if( bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}
	//开始监听是否有客户端连接
	if( listen(socket_fd, 10) == -1){
		printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}

	/*用默认属性初始化互斥锁*/
	pthread_mutex_init(&mut,NULL);

	printf("我是主函数哦，我正在创建线程，呵呵\n");
	thread_create();
	printf("我是主函数哦，我正在等待线程完成任务阿，呵呵\n");
	thread_wait();
}
void thread_create(void)
{
	int temp;
	memset(&thread, 0, sizeof(thread));          //comment1
	/*创建线程*/
	if((temp = pthread_create(&thread[0], NULL, thread1, NULL)) != 0)  //comment2
		printf("线程1创建失败!\n");
	else
		printf("线程1被创建\n");

	if((temp = pthread_create(&thread[1], NULL, thread2, NULL)) != 0)  //comment3
		printf("线程2创建失败");
	else
		printf("线程2被创建\n");
}

void thread_wait(void)
{
	/*等待线程结束*/
	if(thread[0] !=0)
	{             //comment4
		pthread_join(thread[0],NULL);
		printf("线程1已经结束\n");
	}
	if(thread[1] !=0)
	{
		//comment5
		pthread_join(thread[1],NULL);
		printf("线程2已经结束\n");
	}
}

void *thread1()
{
	printf("thread1 :主函数在等我完成任务吗？\n");
	int    connect_fd;
	char    buff[4096];
	int     n;
	printf("======waiting for client's request======\n");
	while(1){
		//阻塞直到有客户端连接，不然多浪费CPU资源。
		if( (connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1){
			printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
			continue;
		}
		//接受客户端传过来的数据
		while(1){
			n = recv(connect_fd, buff, MAXLINE, 0);
			//向客户端发送回应数据
			//if(!fork()){ /*紫禁城*/
			if(send(connect_fd, "Hello,you are connected!\n", 26,0) == -1)
				perror("send error");
			//	close(connect_fd);
			//		exit(0);
			//	}
			buff[n] = '\0';
			printf("recv msg from client: %s\n", buff);
		}
		close(connect_fd);
	}
	close(socket_fd);

}

void *thread2()
{
	printf("thread2 :主函数在等我完成任务吗？\n");
	int    connect_fd;
	char    buff[4096];
	int     n;
	printf("======waiting for client's request======\n");
	while(1){
		//阻塞直到有客户端连接，不然多浪费CPU资源。
		if( (connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1){
			printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
			continue;
		}
		//接受客户端传过来的数据
		while(1){
			n = recv(connect_fd, buff, MAXLINE, 0);
			//向客户端发送回应数据
			//if(!fork()){ /*紫禁城*/
			if(send(connect_fd, "Hello 222222222222222222 ,you are connected!\n", 26,0) == -1)
				perror("send error");
			//	close(connect_fd);
			//		exit(0);
			//	}
			buff[n] = '\0';
			printf("recv msg from client: %s\n", buff);
		}
		close(connect_fd);
	}
	close(socket_fd);
}
