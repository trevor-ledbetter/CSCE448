#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"


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

/*
 * Connect to the server using given host and port information
 *
 * @parameter host    host address given by command line argument
 * @parameter port    port given by command line argument
 * 
 * @return socket fildescriptor
 */
int connect_to(const char *host, const int port)
{
	// ------------------------------------------------------------
	// GUIDE :
	// In this function, you are suppose to connect to the server.
	// After connection is established, you are ready to send or
	// receive the message to/from the server.
	// 
	// Finally, you should return the socket fildescriptor
	// so that other functions such as "process_command" can use it
	// ------------------------------------------------------------

    // below is just dummy code for compilation, remove it.
	
	/*
	The following connection code was taken from one of Dr. Tanzir's 313 projects.
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
	
	/*int sockfd = -1;
	return sockfd;*/
}

/* 
 * Send an input command to the server and return the result
 *
 * @parameter sockfd   socket file descriptor to commnunicate
 *                     with the server
 * @parameter command  command will be sent to the server
 *
 * @return    Reply    
 */
struct Reply process_command(const int sockfd, char* command)
{
	// ------------------------------------------------------------
	// GUIDE 1:
	// In this function, you are supposed to parse a given command
	// and create your own message in order to communicate with
	// the server. Surely, you can use the input command without
	// any changes if your server understand it. The given command
    // will be one of the followings:
	//
	// CREATE <name>
	// DELETE <name>
	// JOIN <name>
    // LIST
	//
	// -  "<name>" is a chatroom name that you want to create, delete,
	// or join.
	// 
	// - CREATE/DELETE/JOIN and "<name>" are separated by one space.
	// ------------------------------------------------------------


	// ------------------------------------------------------------
	// GUIDE 2:
	// After you create the message, you need to send it to the
	// server and receive a result from the server.
	// ------------------------------------------------------------


	// ------------------------------------------------------------
	// GUIDE 3:
	// Then, you should create a variable of Reply structure
	// provided by the interface and initialize it according to
	// the result.
	//
	// For example, if a given command is "JOIN room1"
	// and the server successfully created the chatroom,
	// the server will reply a message including information about
	// success/failure, the number of members and port number.
	// By using this information, you should set the Reply variable.
	// the variable will be set as following:
	//
	// Reply reply;
	// reply.status = SUCCESS;
	// reply.num_member = number;
	// reply.port = port;
	// 
	// "number" and "port" variables are just an integer variable
	// and can be initialized using the message from the server.
	//
	// For another example, if a given command is "CREATE room1"
	// and the server failed to create the chatroom becuase it
	// already exists, the Reply varible will be set as following:
	//
	// Reply reply;
	// reply.status = FAILURE_ALREADY_EXISTS;
    // 
    // For the "LIST" command,
    // You are suppose to copy the list of chatroom to the list_room
    // variable. Each room name should be seperated by comma ','.
    // For example, if given command is "LIST", the Reply variable
    // will be set as following.
    //
    // Reply reply;
    // reply.status = SUCCESS;
    // strcpy(reply.list_room, list);
    // 
    // "list" is a string that contains a list of chat rooms such 
    // as "r1,r2,r3,"
	// ------------------------------------------------------------


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
		printf("error with number of arguments in client\n");
		struct Reply reply_error;
		reply_error.status = FAILURE_INVALID;
		return reply_error;
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
		printf("error with action in client\n");
		struct Reply reply_error;
		reply_error.status = FAILURE_INVALID;
		return reply_error;
	}

	//copy the firstCharacter then roomName to message
	char message[1+strlen(roomName)];
	strcpy(message, firstCharacter);
	strcat(message, roomName);

	//send message to the server using send()
	int lengthOfMessage = strlen(message);
	char* messageP = message;
	if (send(sockfd, messageP, lengthOfMessage, 0) < 0){
		printf("error with send in client\n");
		struct Reply reply_error;
		reply_error.status = FAILURE_INVALID;
		return reply_error;
	}

	//TO DO################################################################
	//receive a reponse from the server
	//convert to a Reply type and return
	//TO DO################################################################
	int length;
	char* replyBuffer;
	//length = recv(sockfd, replyBuffer, MAX_DATA, 0);

	// REMOVE below code and write your own Reply.
	struct Reply reply;
	reply.status = SUCCESS;
	reply.num_member = 5;
	reply.port = 1024;
	return reply;

	//keep this code
	delete firstCharacter;
}

/* 
 * Get into the chat mode
 * 
 * @parameter host     host address
 * @parameter port     port
 */
void process_chatmode(const char* host, const int port)
{
	// ------------------------------------------------------------
	// GUIDE 1:
	// In order to join the chatroom, you are supposed to connect
	// to the server using host and port.
	// You may re-use the function "connect_to".
	// ------------------------------------------------------------

	// ------------------------------------------------------------
	// GUIDE 2:
	// Once the client have been connected to the server, we need
	// to get a message from the user and send it to server.
	// At the same time, the client should wait for a message from
	// the server.
	// ------------------------------------------------------------
	
    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    // 1. To get a message from a user, you should use a function
    // "void get_message(char*, int);" in the interface.h file
    // 
    // 2. To print the messages from other members, you should use
    // the function "void display_message(char*)" in the interface.h
    //
    // 3. Once a user entered to one of chatrooms, there is no way
    //    to command mode where the user  enter other commands
    //    such as CREATE,DELETE,LIST.
    //    Don't have to worry about this situation, and you can 
    //    terminate the client program by pressing CTRL-C (SIGINT)
	// ------------------------------------------------------------

	//port comes from a response from the server!
	int sockfd = connect_to(host, port);
	if(sockfd < 0) exit(0);

	if(fork()==0){
		//child
		while(1){
			char* buf;
			int length = recv(sockfd, buf, MAX_DATA, 0);
			display_message(buf);
		}
	}else{
		//parent
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

