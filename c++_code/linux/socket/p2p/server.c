#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define SERVER_PORT 8001
#define SERVER_IP "127.0.0.1"

int main(void)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("Create socket error.\n");
		exit(-1);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	int optval = 1;
	if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) {
		perror("Set sockopt error.\n");
		exit(-1);
	}
	if (-1 == bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) {
		perror("Bind error.\n");
	}
	
	if (-1 == listen(sockfd, SOMAXCONN)) {
		perror("Listen error.\n");
	}
	struct sockaddr_in accepaddr;
	socklen_t size = 0;
	int connfd = accept(sockfd, (struct sockaddr *)&accepaddr, &size);
	if (-1 == connfd) {
		perror("Accept error.\n");
		exit(-1);
	}
	char recvbuffer[1024] = {0};
	while (1) {
		int size = read(connfd, recvbuffer, sizeof(recvbuffer));	
		if (0 == size) {
			printf("Client exit.\n");
			exit(0);
		}
		if (-1 == size) {
			if (errno == EINTR) { //interrupt by signal
				continue;
			}
		}
		printf("hello.\n");
		memset(recvbuffer, 0, sizeof(recvbuffer));
		write(stdout, recvbuffer, strlen(recvbuffer));
	}
	return 0;
}
