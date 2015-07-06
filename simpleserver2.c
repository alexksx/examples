#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXBYTES 1024
#define MAXCLIENTS 100
#define MAXPATH 10000
#define PORT "8080"

char root[]="/tmp/";

void respond(int sock_fd) {
	char path[MAXPATH] = "";
	char path_buf[MAXPATH], data_buf[MAXBYTES];
	int fd, bytes_read;
	read(sock_fd, path_buf, MAXPATH);
	strcat(strcat(path, root), path_buf);
	if((fd = open(path, O_RDONLY)) != -1) {
		while(bytes_read = read(fd, data_buf, MAXBYTES))
			write(sock_fd, data_buf, bytes_read);
		close(fd);
	}
	else
		write(sock_fd, "error 404: not found!\n", 22);
}

int main()
{
	int server_sockfd, client_sockfd;
	int client_len;
	int i = 1;
	struct addrinfo hint, *server_address_info, *v;
	struct sockaddr_storage client_address_info;
	memset(&hint, 0, sizeof hint);
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;
	if(getaddrinfo(NULL, PORT, &hint, &server_address_info))
	{
		perror("getaddrinfo");
		exit(1);
	}
	for(v = server_address_info; v; v = v->ai_next)
	{
		if((server_sockfd = socket(v->ai_family, v->ai_socktype, v->ai_protocol)) == -1)
		{
			perror("socket");
			continue;
		}
		if(setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}
		if(bind(server_sockfd, v->ai_addr, v->ai_addrlen) == -1)
		{
			close(server_sockfd);
			perror("bind");
			continue;
		}
		break;
	}
	if(!v)
	{
		printf("bind failed");
		exit(1);
	}
	if(listen(server_sockfd, MAXCLIENTS) == -1)
	{
		perror("listen");
		exit(1);
	}
	while(1)
	{
		client_len = sizeof client_address_info;
		if((client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_address_info, &client_len)) == -1)
		{
			perror("accept");
			continue;
		}
		if(!fork())
		{
			close(server_sockfd);
			respond(client_sockfd);
			exit(0);
		}
		else
			close(client_sockfd);
	}
}
