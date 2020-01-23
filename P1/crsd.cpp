#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include <pthread.h>
#include <signal.h>
#include <string>
#include <thread>
#include <array>
#include <iostream>
#include <vector>
using namespace std;

/**
 * 
 * The base of the code was adapted from Dr. Tanzir's 313 class
 * 
 * */

//
// ─── SHARED GLOBAL VARS ─────────────────────────────────────────────────────────
//

// Constants
const size_t MAX_MEMBER = 64;
const size_t MAX_ROOM   = 128;
const size_t MAX_NAME   = 32;
const int    PORT_START = 90000;
const char*  SHARED_MEMORY_NAME = "/chatroom_DB_mem";

// IPC Variables
char    *shared_start = nullptr;
sem_t   *shared_sem   = nullptr;
char    *shared_data  = nullptr;

//
// ─── ROOM DATABASE ──────────────────────────────────────────────────────────────
//

struct Room {
    char room_name[MAX_NAME];
    int port_num;
    int num_members;
    array<int, MAX_MEMBER> slave_socket;
    pid_t chatroom_process;
};

// Using stl array for ease
typedef array<Room, MAX_ROOM> roomDB_t;
int totalRooms = 0;

const off_t SHARED_MEMORY_SIZE = sizeof(sem_t) + sizeof(roomDB_t);

// Forward declarations
void chatroom_send_handler(int _client_socket, string _msg);


/**
 * Handles client requests to open a new room and sends appropriate response to requesting client
 * 
 * @param _client_socket    File descriptor referring to client slave socket
 * @param _room_name        Name to give new room
 * */
struct Reply room_creation_handler (int _client_socket, string _room_name) {
    struct Reply reply;
    reply.status = FAILURE_UNKNOWN;
    // Ensure room creation is valid
    cout << "before totalname" << endl;
    if (totalRooms >= MAX_ROOM) {
        perror("Too many rooms currently in use. Close some rooms to make new ones");
        reply.status = FAILURE_INVALID;
        return reply;
    }
    cout << "before reinterpret cast" << endl;
    // Get reference to data
    roomDB_t &room_db = *reinterpret_cast<roomDB_t*>(shared_data);
    cout << "before already exists" << endl;

    for (auto It = room_db.begin(); It != room_db.end(); It++) {
        if (string(It->room_name) == _room_name) {
            perror("Requested room name already exists.");
            reply.status = FAILURE_ALREADY_EXISTS;
            return reply;
        }
    }

    cout << "before roomname stuff" << endl;
    // Make room and notify client
    for (auto Idx = 0; Idx < MAX_ROOM; Idx++) {
        if (room_db[Idx].room_name == "") {
            cout << "1" << endl;
            strncpy(room_db[Idx].room_name, _room_name.c_str(), MAX_NAME);
            cout << "2" << endl;
            // Initialize db entry
            room_db[Idx].num_members = 0;
            cout << "3" << endl;
            room_db[Idx].port_num = PORT_START + Idx;
            cout << "4" << endl;
            room_db[Idx].slave_socket.fill(-1);
            cout << "5" << endl;
            room_db[Idx].chatroom_process = -1;

            // Create process
            // if (fork() == 0) { // Parent
            reply.status = SUCCESS;
            cout << "before reply" << endl;
            return reply;
        }
    }

    perror("No empty room name found");
    return reply;    
}

/**
 * Handles chatroom deletion and sends appropriate response to requesting client and all other connected clients
 * 
 * @param _client_socket    File descriptor referring to client slave socket
 * @param _room_name        Name of room to delete
 * */
void room_deletion_handler(int _client_socket, string _room_name) {
    // Get reference to data
    roomDB_t &room_db = *reinterpret_cast<roomDB_t*>(shared_data);
    
    // Ensure room can be deleted
    Status procStat = FAILURE_UNKNOWN;
    Room* roomPending = nullptr;
    for (Room currRoom : room_db) {
        if (string(currRoom.room_name) == _room_name) {
            roomPending = &currRoom;
            break;
        }
    }
    if (roomPending == nullptr) {
        perror("Could not find room with specified name");
        procStat = FAILURE_NOT_EXISTS;
        send(_client_socket, &procStat, sizeof(Status), 0);
        return;
    }

    // Send exit messages on separate threads
    vector<thread> senderThreads;
    string exitMsg = "Chat room being deleted!";
    for (auto It = roomPending->slave_socket.begin(); It != roomPending->slave_socket.end(); It++) {
        if (*It != -1) {
            thread sendThread(chatroom_send_handler, *It, exitMsg);
            senderThreads.push_back(std::move(sendThread));
        }
    }

    // Join threads then delete room from database
    for (auto &trd : senderThreads) {
        trd.join();
    }
    // End room process
    kill(roomPending->chatroom_process, SIGTERM);
    // Critical Section
    sem_wait(shared_sem);
    roomPending->chatroom_process = -1;
    roomPending->num_members = 0;
    roomPending->port_num = -1;
    strcpy(roomPending->room_name, "");
    roomPending->slave_socket.fill(-1);
    sem_post(shared_sem);
    // End critical section

}

/**
 * Main connection handling function that delegates operations to other functions based on command from message received
 * 
 * @param _client_socket Client slave socket file descriptor
 * */
void lobby_connection_handler (int _client_socket){
    printf("Connected to client socket %d\n", _client_socket);
    
    char buf [MAX_DATA];
    while (1){
        //Recieve command from client
        if (recv (_client_socket, buf, sizeof(buf), 0) < 0){
            perror ("server: Receive failure");    
            exit (0);
        }
        
        //Parse command
        string command_str(buf); //convert to std::string
        cout << "Message recieved is: " << command_str << endl;
        char firstChar = command_str[0];
        struct Reply reply;
        switch(firstChar){
            case '0':
                {
                    //create a room
                    string room_name(command_str.begin()+1, command_str.end());
                    cout << "before creation" << endl;
                    reply = room_creation_handler(_client_socket, room_name);
                    cout << "after creation" << endl;
                    break;
                }
            case '1':
                {
                    //delete a room
                    break;
                }
            case '2':
                {
                    //join a room
                    //1. check if room exists
                    //2. check if there is an open spot
                    //3. return a REPLY with the room
                    break;
                }
            case '3':
                {
                    //list all the chatrooms
                    break;
                }
            default:
                break;
        }

        int size = sizeof(reply) + 1; //take away +1???
        char* msgBuf = new char[size];
        cout << "before memcpy" << endl;

        memcpy(msgBuf, &reply, sizeof(reply));
        cout << "after memcpy" << endl;

        if (send(_client_socket, msgBuf, sizeof(msgBuf), 0) == -1){
            perror("send");
            break;
        }
        delete msgBuf;
        break;
    }
    printf("Closing client socket\n");
	close(_client_socket);
}

/**
 * Chatroom connection sender that delegates outgoing on a socket
 * 
 * @param _client_socket    Client slave socket file descriptor
 * */
void chatroom_send_handler(int _client_socket, string _msg) {
    printf("Connected to Chatroom:Send on slave socket: %d", _client_socket);

    // Add message to buffer
    char buf [MAX_DATA];
    strncpy(buf, _msg.c_str(), MAX_DATA);

    // Send to client
    if (send(_client_socket, buf, sizeof(buf), 0) < 0) {
        fprintf(stderr, "server: Send Failure to Socket %d", _client_socket);
    }
}

/**
 * Chatroom connection listener that delegates incoming messages on a socket
 * 
 * @param _client_socket    Client slave socket file descriptor
 * @param _room_ptr         Pointer to Room object in shared memory
 * */
void chatroom_listen_handler(int _client_socket, Room* _room_ptr) {
    printf("Connected to Chatroom:Listen on slave socket: %d", _client_socket);

    char buf [MAX_DATA];
    // Keep listening for incoming message
    while(1) {
        if (recv (_client_socket, buf, sizeof (buf), 0) < 0){
            perror ("server: Receive failure");    
            exit (0);
        }

        // Send message to other clients
        for (int currSocket : _room_ptr->slave_socket) {
            if (currSocket != -1) {
                thread sendThread(chatroom_send_handler, currSocket, string(buf));
                sendThread.detach();
            }
        }
    }
    printf("No longer connected to client on socket: %d. Closing...", _client_socket);
    close(_client_socket);
    _room_ptr->num_members -= 1;
    for (int &currSock : _room_ptr->slave_socket) {
        if (currSock == _client_socket) {
            currSock = -1;
            break;
        }
    }
}

/**
 * Main server function that listens for connections and has the main accept loop
 * Creates threads for each incoming connection
 * 
 * @param port Port number as a char which determines which port to listen for incoming connections
 * */
int server (char* port)
{
	int sockfd;  // listen on sock_fd
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
	while(1) 
	{  // main accept() loop
        sin_size = sizeof their_addr;
        int slave_socket = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (slave_socket == -1) {
            fprintf(stderr, "server: accept\n");

            continue;
        }
        thread slave_thread(lobby_connection_handler, slave_socket);
        slave_thread.detach();
    }

    return 0;
}

/**
 * Slave chatroom server function that listens for connections and has the main accept loop
 * Creates threads for each incoming connection
 * 
 * @param port          Port number as a char which determines which port to listen for incoming connections
 * @param _room_ptr     Pointer to room in shared memory
 * */
void chatroom_server(char* port, Room* _room_ptr) {
    int sockfd;  // listen on sock_fd
    struct addrinfo hints, *serv;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if (getaddrinfo(NULL, port, &hints, &serv) != 0) {
        fprintf(stderr, "Chat Server: getaddrinfo error!\n");
        return;
    }
	if ((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1) {
        fprintf(stderr, "Chat Server: socket\n");
		return;
    }
    if (bind(sockfd, serv->ai_addr, serv->ai_addrlen) == -1) {
		close(sockfd);
        fprintf(stderr, "Chat Server: bind\n");
		return;
	}
    freeaddrinfo(serv); // all done with this structure

    if (listen(sockfd, 20) == -1) {
        fprintf(stderr, "Chat Server: listen\n");
        exit(1);
    }
	int serverID = atoi(port) - PORT_START;
    printf("Chat Server[%d]: waiting for connections...\n", serverID);
	while(1) 
	{  // main accept() loop
        sin_size = sizeof their_addr;
        int slave_socket = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (slave_socket == -1) {
            fprintf(stderr, "Chat Server: accept\n");

            continue;
        }
        thread slave_thread(chatroom_listen_handler, slave_socket, _room_ptr);
        slave_thread.detach();
    }

    return;
}

//
// ─── SIGNAL HANDLERS ────────────────────────────────────────────────────────────
//

void handle_termination_master(int _sig) {
    if (_sig == SIGINT) {
        printf("Server shutting down (Keyboard interrupt)\n");
    } else if (_sig == SIGTERM) {
        printf("Server shutting down (Termination signal)\n");
    }
    printf("Handling signal from master\n");
    munmap(shared_start, SHARED_MEMORY_SIZE);
    shm_unlink(SHARED_MEMORY_NAME);
    sem_destroy(shared_sem);    // Only destroy semaphore from master
    exit(0);
}

void handle_termination_slave(int _sig) {
    if (_sig == SIGINT) {
        printf("Server shutting down (Keyboard interrupt)\n");
    } else if (_sig == SIGTERM) {
        printf("Server shutting down (Termination signal)\n");
    }
    printf("Handling signal from slave\n");
    munmap(shared_start, SHARED_MEMORY_SIZE);
    shm_unlink(SHARED_MEMORY_NAME);
    exit(0);
}

int main (int ac, char ** av)
{
    if (ac < 2){
        printf("Usage: ./server <port no>\n");
        exit (-1);
    }

    if (0 || atoi(av[1]) >= PORT_START && atoi(av[1]) < PORT_START+MAX_ROOM) {
        printf("Starting chatroom server\n");

        signal(SIGINT,  handle_termination_slave);
        signal(SIGTERM, handle_termination_slave);
        
        // Setup shared memory
        int shmFD = shm_open(SHARED_MEMORY_NAME, O_RDWR, S_IRUSR|S_IWUSR);
        shared_start = (char*)mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shmFD, 0);
        close(shmFD);
        shared_data = shared_start + sizeof(sem_t);
        shared_sem = (sem_t*)shared_start;
        printf("TEST2: %d", reinterpret_cast<roomDB_t*>(shared_data)->at(0).num_members);

        roomDB_t* db_ptr = reinterpret_cast<roomDB_t*>(shared_data);
        Room* process_chatroom = nullptr;

        for (auto It = db_ptr->begin(); It != db_ptr->end(); It++) {
            if (It->port_num == atoi(av[1])) {
                process_chatroom = It;
            }
        }

        chatroom_server(av[1], process_chatroom);
    } else {
        printf("Starting lobby server\n");

        signal(SIGINT,  handle_termination_master);
        signal(SIGTERM, handle_termination_master);

        // Setup shared memory region
        int shmFD = shm_open(SHARED_MEMORY_NAME, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
        if (shmFD < 0) {
            perror("Error setting up shared memory");
            exit(1);
        }
        ftruncate(shmFD, SHARED_MEMORY_SIZE);
        shared_start = (char*)mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shmFD, 0);
        close(shmFD);
        shared_data = shared_start + sizeof(sem_t);
        shared_sem = (sem_t*)shared_start;
        roomDB_t initDB;
        sem_t    initSem;
        initDB.at(0).num_members = 25965;
        memcpy(shared_data, &initDB, sizeof(roomDB_t));
        memcpy(shared_start, &initSem, sizeof(sem_t));
        printf("TEST: %d", reinterpret_cast<roomDB_t*>(shared_data)->at(0).num_members);

        // Setup shared semaphore to synchronize r/w with shared memory
        if (sem_init(shared_sem, 1, 1) < 0) {
            perror("Error setting up kernel semaphore");
            exit(1);
        }

        server (av [1]);	
    }
    printf("Server shutting down (end of main)\n");
    handle_termination_master(0);
}