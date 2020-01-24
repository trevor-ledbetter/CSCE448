
struct Reply process_command(const int sockfd, char* command)
{
	//Parse the command into two strings: action and roomName
	cout << "process_command command: " << command << endl;

	char* action;
	char* roomName;
	int lengthOfCommand = strlen(command);
	char commandArray[lengthOfCommand];
	strcpy(commandArray, command);

	cout << "here1" << endl;


	char* token = strtok(commandArray, " ");
	action = token;
	cout << "here2\n";
	int count = 0;
	while(token != NULL)
	{
		if(count == 1) //second argument
		{
							cout << "here3\n";
			roomName = token;
				cout << "here4\n";

		}

        token = strtok(NULL, " "); 
		count++;
	}

	cout << "after while\n";
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

	
	//copy the firstCharacter then roomName to message
	int roomNameLength = 0;
	if(count==2){ //for LIST
		roomNameLength += strlen(roomName);
	}
	char message[1+roomNameLength];

	if(count==1){  //for LIST
		//strcpy(message, firstCharacter);
		memcpy(message, firstCharacter, sizeof(firstCharacter));
	}else{  //for all other
		//strcpy(message, firstCharacter);
		memcpy(message, firstCharacter, sizeof(firstCharacter));
		strncat(message, roomName, sizeof(roomName));
	}

	cout << "ROOMNAME is: " << roomName << endl;

	//send message to the server using send()
	int lengthOfMessage = strlen(message);
	char* messageP = message;
	if (send(sockfd, messageP, lengthOfMessage, 0) < 0){
		printf("error with send in client\n");
		struct Reply reply_error;
		reply_error.status = FAILURE_INVALID;
		return reply_error;
	}

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
				cout << "portBuf is: " << portBuf << endl;
				reply.port = atoi(portBuf);

				char* num_memberBuf = new char;
				recv(sockfd, num_memberBuf, MAX_DATA, 0);
				cout << "num_memberBuf is: " << num_memberBuf << endl;
				reply.num_member = atoi(num_memberBuf);


				cout << "reply.status: " << reply.status << endl;
				cout << "reply.port: " << reply.port << endl;
				cout << "reply.num_member: " << reply.num_member << endl;
				

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
				cout << "reply list room: " << reply.list_room << endl;
				cout << "listBuffer: " << listBuffer << endl;
				printf("printf %s\n", listBuffer);
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

struct Reply process_command(const int sockfd, char* command)
{
    string strCommand = str(command);
    cout << "strCommand: " << strCommand << endl;
    char* action = new char[0];
    char* roomName = new char[sizeof(command-1)];

    int count = 0;
    for(int i=0; i<strCommand.length; i++){
        if(strCommand[i] == ' '){
            count++;
        }else if(count==0){
            char* c = strCommand[i];
            strncat(action, c, 1);
        }else if(count ==1){
            char* c = strCommand[i];
            strncat(roomName, c, 1);
        }else{
            break;
        }
    }
    cout << "action: " << action << endl;
    cout << "roomName: " << roomName << endl;

    struct Reply reply;
    reply.status = SUCCESS;


}












struct Reply process_command(const int sockfd, char* command)
{
	//char* idk;
	//strncat(idk, command, sizeof(command));
    string strCommand = string(command);
    cout << "strCommand: " << strCommand << endl;
    char* action = new char[0];
    char* roomName = new char[sizeof(command-1)];
	//char* action;
	//char* roomName;

    int count = 0;
    for(int i=0; i<strCommand.length(); i++){
        if(strCommand[i] == ' '){
            count++;
        }else if(count==0){
            char* c = &strCommand[i];
            //strncat(action, c, 1);
			memcpy(action+i, command+i, 1);
        }else if(count ==1){
            char* c = &strCommand[i];
            //strncat(roomName, c, 1);
			memcpy(roomName+i, command+i, 1);
        }else{
            break;
        }
    }
	count++;
    cout << "action: " << action << endl;
    cout << "roomName: " << roomName << endl;

	cout << "count is: " << count << endl;
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

	//copy the firstCharacter then roomName to message
	int roomNameLength = 0;
	if(count==2){ //for LIST
		roomNameLength += strlen(roomName);
	}
	char message[1+roomNameLength];

	if(count==1){  //for LIST
		//strcpy(message, firstCharacter);
		memcpy(message, firstCharacter, sizeof(firstCharacter));
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


	char* replyBuffer = new char[MAX_DATA];
	struct Reply reply;
	reply.status = FAILURE_INVALID;
	switch(*firstCharacter){
		case '0':
			{
				//CREATE
				cout << "create\n";
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
				cout << "portBuf is: " << portBuf << endl;
				reply.port = atoi(portBuf);

				char* num_memberBuf = new char;
				recv(sockfd, num_memberBuf, MAX_DATA, 0);
				cout << "num_memberBuf is: " << num_memberBuf << endl;
				reply.num_member = atoi(num_memberBuf);


				cout << "reply.status: " << reply.status << endl;
				cout << "reply.port: " << reply.port << endl;
				cout << "reply.num_member: " << reply.num_member << endl;
				

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
				cout << "reply list room: " << reply.list_room << endl;
				cout << "listBuffer: " << listBuffer << endl;
				printf("printf %s\n", listBuffer);
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

	memset(replyBuffer, 0, sizeof(replyBuffer));
	memset(action, 0, sizeof(action));
	memset(roomName, 0, sizeof(roomName));
	delete[] replyBuffer;
	delete firstCharacter;
	delete[] action;
	delete[] roomName;
	return reply; 
}