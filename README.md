# Multiple Chat Rooms in C

This is a Chat Room application writtin as a client-server socket model.
Users create their own account and are then given the option to choose 
among different chat rooms, each with their different topics, to talk about
that particular subject.

## Getting Started

### Prerequisites
1. First the database for the server needs to be created.
Run the following bash script within the directory holding the server 
and client code:
```
bash createDatabase.sh
```
2. Write eight chat group names in the following file:
```Database/Chat_Thread_Names/chat_thread_names.txt```
(*Note: Currently the server is not dynamic and can only hold eight chat 
groups*)

## Running the Server
Execute the following command to run the server:
(*NOTE: The application currently can only function within the same 
computer*)

```
gcc server.c -o server
./server {port number}
```

## Running the Client
Execute the following command to run a client:
```
gcc client.c -o client
./client localhost {server port number}
```

## How to use the Client:
* Create an account
* Login using created username and password
* Enter a single character option to move within the menus\
**Press ESC to return to the previous menu or to exit the chat thread**

