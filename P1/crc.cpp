#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include <iostream>
using namespace std;

/*
 * TODO: IMPLEMENT BELOW THREE FUNCTIONS
 */
int connect_to(const char *host, const int port);
struct Reply process_command(const int sockfd, char* command);
void process_chatmode(const char* host, const int port);


int main(int argc, char** argv) 
{
	if (argc != 3) {
		fprintf(stderr,
				"usage: enter host address and port number\n");
		exit(1);
	}

    display_title();
    
	while (1) {
		int sockfd = connect_to(argv[1], atoi(argv[2]));
    
		//prompts user for input and copies to command
		char command[MAX_DATA];
        get_command(command, MAX_DATA);

		struct Reply reply = process_command(sockfd, command);
		display_reply(command, reply);
		
		touppercase(command, strlen(command) - 1);
		if (strncmp(command, "JOIN", 4) == 0) {
			printf("Now you are in the chatmode\n");
			process_chatmode(argv[1], reply.port);
		}
	
		close(sockfd);
    }

    return 0;
}


int connect_to(const char *host, const int port)
{	
	/*
	The following connection code was adapted from one of Dr. Tanzir's 313 projects.
	*/
	struct addrinfo hints, *res;
	int sockfd = -1;

	// first, load up address structs with getaddrinfo():
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	//Convert int port to a char[]
	int digits = 0;
	int n = port;
	while(n!=0){
		n/=10;
		digits++;
	}
	char portChar[digits];
	sprintf(portChar, "%d", port);

	int status;
	if ((status = getaddrinfo(host, portChar, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error!\n");
        return -1;
    }

	// make a socket:
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0)
	{
		fprintf(stderr, "Making a socket error!\n");
		return -1;
	}

	// connect!
	if (connect(sockfd, res->ai_addr, res->ai_addrlen)<0)
	{
		fprintf(stderr, "connect error!\n");
		return -1;
	}

	printf("Sucessfully connected to, %s\n", host);
	return sockfd;
}


struct Reply process_command(const int sockfd, char* command)
{
	//Parse the command into two strings: action and roomName
	char* action;
	char* roomName;
	int lengthOfCommand = strlen(command);
	char commandArray[lengthOfCommand];
	strcpy(commandArray, command);

	char* token = strtok(commandArray, " ");
	action = token;

	int count = 0;
	while(token != NULL)
	{
		if(count == 1) //second argument
		{
			roomName = token;
		}

        token = strtok(NULL, " "); 
		count++;
	}

	//Test the input for errors and if none, convert the command to an 
	//int that will be sent as the first char to the server
	if(count < 1 || count > 2)
	{
		memset(action, 0, sizeof(action)); //reset action so firstCharacter will = 4, which is the error value
	}

	char* firstCharacter = new char[1];
	if(strcmp(action,"CREATE") == 0){
		*firstCharacter = '0';
	}else if(strcmp(action,"DELETE") == 0){
		*firstCharacter = '1';
	}else if(strcmp(action,"JOIN") == 0){
		*firstCharacter = '2';
	}else if(strcmp(action,"LIST") == 0){
		*firstCharacter = '3';
	}else{
		*firstCharacter = '4';
	}

	//copy the firstCharacter then roomName to message
	int roomNameLength = 0;
	if(count==2){ //for LIST
		roomNameLength += strlen(roomName);
	}
	char message[1+roomNameLength];

	if(count==1){  //for LIST
		strcpy(message, firstCharacter);
	}else{  //for all other
		strcpy(message, firstCharacter);
		strcat(message, roomName);
	}

	//send message to the server using send()
	int lengthOfMessage = strlen(message);
	char* messageP = message;
	if (send(sockfd, messageP, lengthOfMessage, 0) < 0){
		printf("error with send in client\n");
		struct Reply reply_error;
		reply_error.status = FAILURE_INVALID;
		return reply_error;
	}

	
	int length;
	char* replyBuffer = new char[MAX_DATA];
	length = recv(sockfd, replyBuffer, MAX_DATA, 0);
	struct Reply reply = *(Reply*)replyBuffer;

	memset(replyBuffer, 0, sizeof(replyBuffer));
	delete[] replyBuffer;
	delete firstCharacter;
	return reply; 
}


void process_chatmode(const char* host, const int port)
{
	//port comes from a response from the server!
	int sockfd = connect_to(host, port);
	if(sockfd < 0) exit(0);

	if(fork()==0){
		//child takes care of recieving messages from server
		while(1){
			char* buf;
			int length = recv(sockfd, buf, MAX_DATA, 0);
			display_message(buf);
		}
	}else{
		//parent takes message input from client, and sends to server
		while(1){
			char* message;
			int size = MAX_DATA;
			get_message(message, size);
			if (send(sockfd, message, size, 0) < 0){
				printf("error with send in client\n");
			}
		}
	}
}

