#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
using namespace std;

void talk_to_server (int sockfd){
	char buf [1024];
	while (true){
		/*if (recv (sockfd, buf, sizeof (buf), 0) < 0){
            perror ("client: Receive failure");    
            exit (0);
        }
        cout << buf; */
        cout << "Type a number for the server: "; 
        int num; 
        cin>> num;
		if (send (sockfd, &num, sizeof (int), 0) == -1){
            perror("client: Send failure");
            break;
        }
        
        if (recv (sockfd, buf, sizeof (buf), 0) < 0){
            perror ("client: Receive failure");    
            exit (0);
        }
        cout << "The server sent back " << *(int *)buf << endl; 

	}
		
}



int client (char * server_name, char* port)
{
	struct addrinfo hints, *res;
	int sockfd;

	// first, load up address structs with getaddrinfo():

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	int status;
	//getaddrinfo("www.example.com", "3490", &hints, &res);
	if ((status = getaddrinfo(server_name, port, &hints, &res)) != 0) {
        cerr << "getaddrinfo: " << gai_strerror(status) << endl;
        return -1;
    }

	// make a socket:
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0)
	{
		perror ("Cannot create scoket");	
		return -1;
	}

	// connect!
	if (connect(sockfd, res->ai_addr, res->ai_addrlen)<0)
	{
		perror ("Cannot Connect");
		return -1;
	}
	/*cout << "Successfully connected to " << server_name << endl;
	cout <<"Now Attempting to send a message "<< server_name << endl;
	char buf [1024];
	sprintf (buf, "hello");
	send (sockfd, buf, strlen (buf)+1, 0);
	recv (sockfd, buf, 1024, 0);
	cout << "Received " << buf << " from the server" << endl;*/
	talk_to_server(sockfd);
	return 0;
}


int main (int ac, char** av)
{
	if (ac < 3){
        cout << "Usage: ./client <server name/IP> <port no>" << endl;
        exit (-1);
    }
	client (av [1], av [2]);
}