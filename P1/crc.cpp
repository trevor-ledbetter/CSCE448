#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include <iostream>
#include <math.h>
using namespace std;

/*
 * TODO: IMPLEMENT BELOW THREE FUNCTIONS
 */
int connect_to(const char *host, const int port);
struct Reply process_command(const int sockfd, char* command);
void process_chatmode(const char* host, const int port);

int mainsocket = -1;

void handle_termination(int _sig) {
    if (_sig == SIGINT) {
        printf("Client shutting down (Keyboard interrupt)\n");
    } else if (_sig == SIGTERM) {
        printf("Client shutting down (Termination signal)\n");
    }
		char buf[MAX_DATA];
		buf[0] = 3;
		if (send(mainsocket, buf, MAX_DATA, 0) < 0){
				printf("error with send in client\n");
		}
		close(mainsocket);
    printf("Handling signal\n");
    exit(0);
}

int main(int argc, char** argv) 
{
	if (argc != 3) {
		fprintf(stderr,
				"usage: enter host address and port number\n");
		exit(1);
	}

	signal(SIGINT,  handle_termination);
	signal(SIGTERM, handle_termination);
	signal(SIGABRT, handle_termination);

    display_title();
    
	while (1) {
		int sockfd = connect_to(argv[1], atoi(argv[2]));
    
		//prompts user for input and copies to command
		char command[MAX_DATA];
		memset(command, '\0', 80);
		//char* command = new char[MAX_DATA];
        get_command(command, MAX_DATA);

		struct Reply reply = process_command(sockfd, command);
		display_reply(command, reply);
		
		touppercase(command, strlen(command) - 1);
		if (strncmp(command, "JOIN", 4) == 0 && reply.status == SUCCESS) {
		//if (strncmp(command, "JOIN", 4) == 0) {
			printf("Now you are in the chatmode\n");
			process_chatmode(argv[1], reply.port);

		}
		close(sockfd);
    }

    return 0;
}


int connect_to(const char *host, const int port)
{	
	cout << "connect_to port: " << port << endl;
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

	//printf("Sucessfully connected to, %s\n", host);
	return sockfd;
}


struct Reply process_command(const int sockfd, char* command)
{
	//Parse the command into two strings: action and roomName

	char* action = new char;
	char* roomName = new char;
	memset(action, 0, sizeof(action));
	memset(roomName, 0, sizeof(roomName));

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
	if(strcmp(action,"CREATE") == 0 || strcmp(action,"create") == 0){
		*firstCharacter = '0';
	}else if(strcmp(action,"DELETE") == 0 || strcmp(action,"delete") == 0){
		*firstCharacter = '1';
	}else if(strcmp(action,"JOIN") == 0 || strcmp(action,"join") == 0){
		*firstCharacter = '2';
	}else if(strcmp(action,"LIST") == 0 || strcmp(action,"list") == 0){
		*firstCharacter = '3';
	}else{
		*firstCharacter = '4';
	}


	string strAction = string(action);
	string strRoomName = string(roomName);
	const char* help = strRoomName.c_str();

	//copy the firstCharacter then roomName to message
	int roomNameLength = 0;
	if(count==2){
		roomNameLength += strlen(roomName);
	}
	//char message[1+roomNameLength];
	char* message = new char[1+roomNameLength];

	if(count==1){  //for LIST
		//strcpy(message, firstCharacter);
		memcpy(message, firstCharacter, 1);
	}else{  //for all other
		//strcpy(message, firstCharacter);
		memcpy(message, firstCharacter, sizeof(firstCharacter));
		strncat(message, roomName, sizeof(roomName));
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

	//for each command the client will recieve different info back
	char* replyBuffer = new char[MAX_DATA];
	struct Reply reply;
	reply.status = FAILURE_INVALID;
	switch(*firstCharacter){
		case '0':
			{
				//CREATE
				recv(sockfd, replyBuffer, MAX_DATA, 0);
				enum Status stat = *(enum Status*)replyBuffer;
				reply.status = stat;
				break;
			}
		case '1':
			{
				//DELETE
				recv(sockfd, replyBuffer, MAX_DATA, 0);
				enum Status stat = *(enum Status*)replyBuffer;
				reply.status = stat;
				break;
			}
		case '2':
			{
				//JOIN

				recv(sockfd, replyBuffer, MAX_DATA, 0);
				char* statusBuffer = new char[sizeof(reply.status)];
				memcpy(statusBuffer, replyBuffer, sizeof(reply.status));
				reply.status = *(enum Status*)statusBuffer;

				char* portBuf = new char;
				recv(sockfd, portBuf, MAX_DATA, 0);
				reply.port = atoi(portBuf);

				char* num_memberBuf = new char;
				recv(sockfd, num_memberBuf, MAX_DATA, 0);
				reply.num_member = atoi(num_memberBuf);
				

				memset(portBuf, 0, sizeof(portBuf));
				memset(num_memberBuf, 0, sizeof(num_memberBuf));

				delete[] portBuf;
				delete[] num_memberBuf;
				break;
			}
		case '3':
			{
				//LIST
				enum Status s = FAILURE_INVALID;
				recv(sockfd, replyBuffer, sizeof(s), 0);
				enum Status stat = *(enum Status*)replyBuffer;
				reply.status = stat;

				char* listBuffer = new char[MAX_DATA];
				recv(sockfd, listBuffer, MAX_DATA, 0);
				char* list = listBuffer;
				//reply.list_room = listBuffer;
				memcpy(reply.list_room, listBuffer, 256);
				memset(listBuffer, 0, sizeof(listBuffer));
				delete[] listBuffer;
				break;
			}
		case '4':
			{
				//error
				recv(sockfd, replyBuffer, MAX_DATA, 0);
				enum Status stat = *(enum Status*)replyBuffer;
				reply.status = stat;
				break;
			}
		default:
			break;
	}

	//struct Reply reply = *(Reply*)replyBuffer;
	memset(replyBuffer, 0, sizeof(replyBuffer));
	//memset(action, 0, sizeof(action));
	//memset(roomName, 0, sizeof(roomName));

	delete[] replyBuffer;

	delete firstCharacter;
	return reply; 
}

void process_chatmode(const char* host, const int port)
{
	//port comes from a response from the server!
	int sockfd = connect_to(host, port);
	mainsocket = sockfd;
	if(sockfd < 0) exit(0);

	while(1) {
		// Setup select() params
		fd_set 	readFDSet;
		fd_set  writeFDSet;
		timeval readTime;
		int			selectOut;

		FD_ZERO(&readFDSet);
		FD_ZERO(&writeFDSet);
		FD_SET(sockfd, &readFDSet);
		FD_SET(sockfd, &writeFDSet);
		FD_SET(0, &readFDSet);

		readTime.tv_sec = 1;
		readTime.tv_usec = 0;

		selectOut = select(sockfd+1, &readFDSet, &writeFDSet, NULL, &readTime);

		if (selectOut < 0) {
			perror("Error with select");
			exit(1);
		} else if (selectOut > 0) {
			char buf[MAX_DATA];
			memset(buf, 0, MAX_DATA);
			// Handle STDIN
			if (FD_ISSET(0, &readFDSet)) {
				get_message(buf, MAX_DATA);
				if (send(sockfd, buf, MAX_DATA, 0) < 0){
					printf("error with send in client\n");
				}
			} else if (FD_ISSET(sockfd, &readFDSet)) {
				int length = recv(sockfd, buf, MAX_DATA, 0);
				if (buf[0] == 3 || length == 0) {
					string msg_s = "Chatroom closing. Closing client...\n";
					strncpy(buf, msg_s.c_str(), sizeof(buf));
					display_message(buf);
					exit(0);
				}

				display_message(buf);
				printf("\n");
			}
		}
	}

	#if 0
	__pid_t childPID = fork();
	if(!childPID){
		//child takes care of recieving messages from server
		while(1){
			char buf[MAX_DATA];
			memset(buf, 0, sizeof(buf));

			int length = recv(sockfd, buf, MAX_DATA, 0);

			if (buf[0] == 3) {
				string msg_s = "Chatroom closing. Press enter to continue...\n";
				strncpy(buf, msg_s.c_str(), sizeof(buf));
				display_message(buf);
				exit(0);
			}

			display_message(buf);
		}
	}else{
		//parent takes message input from client, and sends to server
		while(1){
			char message[MAX_DATA];
			int size = MAX_DATA;
			get_message(message, size);
			if (send(sockfd, message, size, 0) < 0){
				printf("error with send in client\n");
			}
			if (waitpid(childPID, 0, WNOHANG) > 0) {
				//printf("Reaped child, closing parent client process\n");
				exit(0);
			}
		}
	}
	#endif
}

