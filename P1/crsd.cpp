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
#include <string>
#include <thread>
#include <array>

using namespace std;

/**
 * 
 * The base of the code was adapted from Dr. Tanzir's 313 class
 * 
**/

//
// ─── GLOBAL VARS ────────────────────────────────────────────────────────────────
//

const size_t MAX_MEMBER = 64;
const size_t MAX_ROOM   = 128;
const int    PORT_START = 90000;

//
// ─── ROOM DATABASE ──────────────────────────────────────────────────────────────
//

struct Room {
    string room_name;
    int port_num;
    int num_members;
    array<int, MAX_MEMBER> slave_socket;
};

// Using stl array for ease
array<Room, MAX_ROOM> room_db;
int totalRooms = 0;

void handle_create_room (int _client_socket, string _room_name) {
    // Ensure room creation is valid
    Status procStat = FAILURE_UNKNOWN;
    if (totalRooms >= MAX_ROOM) {
        perror("Too many rooms currently in use. Close some rooms to make new ones");
        procStat = FAILURE_INVALID;
        send(_client_socket, &procStat, sizeof(Status), 0);
        return;
    }

    for (auto It = room_db.begin(); It != room_db.end(); It++) {
        if (It->room_name == _room_name) {
            perror("Requested room name already exists.");
            procStat = FAILURE_ALREADY_EXISTS;
            send(_client_socket, &procStat, sizeof(Status), 0);
            return;
        }
    }

    // Make room and notify client
    for (auto Idx = 0; Idx < MAX_ROOM; Idx++) {
        if (room_db[Idx].room_name == "") {
            room_db[Idx].room_name = _room_name;
            room_db[Idx].num_members = 0;
            room_db[Idx].port_num = PORT_START + Idx;
            room_db[Idx].slave_socket.fill(-1);
            procStat = SUCCESS;
            send(_client_socket, &procStat, sizeof(Status), 0);
            return;
        }
    }

    perror("No empty room name found");
    procStat = FAILURE_UNKNOWN;
    send(_client_socket, &procStat, sizeof(Status), 0);
    
}

void* connection_handler (int arg){
    int client_socket = arg;

    printf("Connected to client socket %d\n", client_socket);
    
    char buf [MAX_DATA];
    while (1){
        if (recv (client_socket, buf, sizeof (buf), 0) < 0){
            perror ("server: Receive failure");    
            exit (0);
        }
        printf("ioejr");
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
        thread slave_thread(connection_handler, slave_socket);
        slave_thread.detach();
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