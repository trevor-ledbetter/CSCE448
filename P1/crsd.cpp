#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include <pthread.h>


void* connection_handler (void* arg){
    int client_socket = *(int *) arg;
    
    char buf [1024];
    while (1){
        /*string prompt = "Welcome. Give me a number, I will double that for you: ";
        if (send(client_socket, prompt.c_str(), prompt.size() +1, 0) == -1){
            perror("server: Send failure");
            break;
        }*/
        if (recv (client_socket, buf, sizeof (buf), 0) < 0){
            perror ("server: Receive failure");    
            exit (0);
        }
        int num = *(int *)buf;
        num *= 2;
        if (num == 0)
            break;
        if (send(client_socket, &num, sizeof (num), 0) == -1){
            perror("send");
            break;
        }
    }
    printf("Closing client socket\n");
	close(client_socket);
}



int server (char* port)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *serv;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, port, &hints, &serv)) != 0) {
        fprintf(stderr, "Server: getaddrinfo error!\n");
        return -1;
    }
	if ((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1) {
        fprintf(stderr, "Server: socket\n");
		return -1;
    }
    if (bind(sockfd, serv->ai_addr, serv->ai_addrlen) == -1) {
		close(sockfd);
        fprintf(stderr, "server: bind\n");
		return -1;
	}
    freeaddrinfo(serv); // all done with this structure

    if (listen(sockfd, 20) == -1) {
        fprintf(stderr, "server: listen\n");
        exit(1);
    }
	
    printf("server: waiting for connections...\n");
	char buf [1024];
	while(1) 
	{  // main accept() loop
        sin_size = sizeof their_addr;
        int slave_socket = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (slave_socket == -1) {
            fprintf(stderr, "server: accept\n");

            continue;
        }
        pthread_t tid;
        pthread_create (&tid, 0, connection_handler, &slave_socket); 
    }

    return 0;
}

int main (int ac, char ** av)
{
    if (ac < 2){
        printf("Usage: ./server <port no>\n");
        exit (-1);
    }
	server (av [1]);	
}