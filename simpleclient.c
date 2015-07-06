#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXBYTES 1024
#define PORT 8080

int main() {
	int sockfd;
	int len;
	struct sockaddr_in address;
	int result;
	char ch[] = "temp.file";
	char data[MAXBYTES];
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(PORT);
	len = sizeof(address);
	result = connect(sockfd, (struct sockaddr *)&address, len);
	if (result == -1) {
		perror("oops : client1");
		exit(1);
	}
	write(sockfd, ch, sizeof(ch));
	do {
		len = read(sockfd, data, MAXBYTES);
		printf("%s", data);	
	} while(len == MAXBYTES);
	close(sockfd);
	exit(0);
}