# CSCE448
Distributed Systems

## Design

The overall design will involve the following:
1. Server is started and the main process thread listens to the main socket where all initial interaction is started.

2. A client will connect to the server via this main socket, and a slave socket will be created to handle the rest of the interaction.

3. If a `create` request is received, the server references a database of created chatrooms and if it does not exist, creates a new chatroom with the specifed name, a unique master socket, and thread through which messages will be handled with other clients.

4. If a `join` request is received, the server will check if the specifed name exists and return the necessary information for the client to connect to it.

5. If a `list` request is received, the server will iterate through the list of created chatrooms and send all chatroom names to the client.

6. If a `delete` request is received, the server will check if the specified chatroom name exists and if so delete it.

7. Responses from the server will begin with a `Status` byte followed by any additional information.

8. Commands from a client will be determined by the server with the first byte of the message, where a char will describe the requested action. The action values are defined in the *Action Values* section.

### Action Values
| Action | Value |
| ------ | ----- |
| CREATE | 1     |
| JOIN   | 2     |
| LIST   | 3     |
| DELETE | 4     |

### Server Implementation

The server will have the following:
1. The `main()` function will exist to handle parameters and hand necessary information into another `server()` function that will contain the rest of the server logic as well as the main connection accepting loop.

2. The `server()` function will exist to handle the main grunt of the server, setting up the master socket to listen for connections. A loop will exist to handle connections when they are made and will spawn a thread that runs the `connection_handler()` will then execute server-side logic based on the received message from the client.

3. The `connection_handler()` function will exist as a threaded function to handle each message/request from the clients separate from the main socket and lets the main server process handle more incoming requests. These threads will parse the received messages and will execute the correct functions associated with each message's intent.

4. The `room_creation_handler()` function will handle any requests to create a room and send the appropriate response to the sending client.


### Client Implementation

The client program will have the following:
1. 