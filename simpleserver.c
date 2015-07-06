#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXBYTES 1024
#define MAXCLIENTS 100
#define MAXPATH 10000
#define PORT 8080

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
	int server_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	int epfd;
	struct epoll_event event, *events;
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
	listen(server_sockfd, MAXCLIENTS);
	events = malloc(sizeof (struct epoll_event) * MAXCLIENTS);
	if(!events) {
		perror("malloc");
		exit(1);
	}
	if((epfd = epoll_create(MAXCLIENTS)) < 0) {
		perror("epoll_create");
		free(events);
		exit(1);
	}
	event.data.fd = server_sockfd;
	event.events = EPOLLIN;
	if(epoll_ctl(epfd, EPOLL_CTL_ADD, server_sockfd, &event)){
		perror("epoll_ctl");
		free(events);
		exit(1);
	}
	while(1) {
		int i, nr_events;
		int nread;
		if((nr_events = epoll_wait(epfd, events, MAXCLIENTS, 0)) < 0) {
			perror("epoll_wait");
			free(events);
			exit(1);
		}
		for(i = 0; i < nr_events; i++) {
			if(events[i].data.fd == server_sockfd) {
				client_len = sizeof(client_address);
				event.data.fd = accept(server_sockfd,(struct sockaddr*)&client_address, &client_len);
				event.events = EPOLLIN;
				if(epoll_ctl(epfd, EPOLL_CTL_ADD, event.data.fd, &event)) {
					perror("epoll_ctl");
					free(events);
					exit(1);
				}
			}
			else {
				ioctl(events[i].data.fd, FIONREAD, &nread);
				if(!nread) {
					if(epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL)) {
						perror("epoll_ctl");
						free(events);
						exit(1);
					}
					close(events[i].data.fd);
				}
				else
					respond(events[i].data.fd);
			}
		}
	}
}