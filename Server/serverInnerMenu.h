#ifndef SERVER_INNER_MENU_H
#define SERVER_INNER_MENU_H

#define EXIT_CLEAN 0
#define EXIT_ESC 1
#define EXIT_CTRL_C 2

#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>
#include "Server_Auxiliary/server_aux.h"

extern int usersCount;
extern int users[100];
extern char chatThreads[6][50];

struct userChatThreadInfo{
	int sockfd;
	int chatThreadID = -1;
	int termID = -1;
};

struct chatThreadOption{
	int option;
	int numChatThreads;
	char filePath[256];
};

// Reads() the message that was sent from the user
// Returns EXIT_CTRL_C if users terminated with CTRL-C
int readMessage(int sockfd,char mssg[],int size){
	int err;
	err = read(sockfd,mssg,size);
	if(err == 0){
		perror("ERROR: Reading from Socket\n");
		return EXIT_CTRL_C;
	}
	return EXIT_CLEAN;
}

// Writes() the message to all the users within the chat thread array
void writeMssgAllClients(int sockfd,int threadID,char mssg[],int size){
	int n;
	int chatThreadSockfd;
	for(int i = 0;i < thread_chat_count[threadID];i++){
		chatThreadSockfd = thread_chat_list[threadID][i];
		if(chatThreadSockfd != sockfd){
			n = write(sockfd,mssg,size);
			if(n < 0) error("ERROR: Writing to Socket\n");
		}
	}
}


// Writes() the names of the threads chat to the client
int writeChatThreadNames(int sockfd){
	FILE *ptr = fopen("../Database/Chat_Thread_Names/
	chat_thread_names.txt");	
	char *buffer;
	int bufferSize;
	int fileCharLen;
		
	if(ptr == NULL){
		error("ERROR: Writing to File\n");
	}
	
	fseek(ptr,0,SEEK_END);
	fileCharLen = ftell(ptr);
	rewind(ptr);
	bufferSize = fileCharLen * sizeof(char);
	buffer = malloc(bufferSize);	
	if(!buffer){
		error("Memory Not Allocated\n");
	}
	fread(buffer,fileCharLen,1,ptr);
	fclose(ptr);
	free(buffer);

	err = write(sockfd,&bufferSize,sizeof(bufferSize));
	if(err < 0){
		error("Writing to Socket\n");
	}
	
	err = write(sockfd,buffer,bufferSize);
	if(err < 0){
		error("Writing to Socket\n");
	}

	for(int i = 0;i < bufferSize;i++){
		if(buffer[i] == '\n')
			(*countNames)++;
	}
	
	return countNames;
}

// Read() the chat thread option
// Return EXIT_CTRL_C if user pressed Ctrl-C
int readChatThreadOption(int sockfd, char *option){

	// Reads the chat thread option from client
	option = readUserOption(sockfd,option);
	if(option == EXIT_CTRL_C){
		return EXIT_CTRL_C;
	}

	return EXIT_CLEAN;
}

// Converts a given char to its integer equivalant
int covertCharToInt(char option){
	char optionArr[2];
	int threadIndex;
	
	optionArr[0] = option;
	optionArr[1] = '\0';
	threadIndex = atoi(optionArr);

	return threadIndex;
}

// Creates the file path name for the chat log and stores in a sturct
void generateChatLogFile(char option,char filePath[]){
	strcat(filePath,"../Thread_Chat_Logs/");
	strcat(filePath,option);
	strcat(filePath,".txt");
}

// Appends user to the chat thread array for the chosen chat thread
void addUserToChatThread(int sockfd,int threadIndex){	
	thread_chat_list[threadIndex][userCount] = sockfd;
	thread_chat_count[chatThreadIndex]++;
}


// Writes the chat log file contents to the client
void writeChatThreadLog(int sockfd,char filePath[]){
	FILE *ptr;
	char *buffer;
	int bufferSize;
	int fileCharLen;

	// Open the chat log file	
	ptr = fopen(filePath);
	if(ptr == NULL){
		error("Opening File\n");
	}

	// Open the chat log file and write the contents to the client
	fseek(ptr,0,SEEK_END);
	fileCharLen = ftell(ptr);
	rewind(ptr);
	bufferSize = fileChar * sizeof(char);
	buffer = malloc(bufferSize);
	if(!buffer){
		error("Memory not Allocated\n");
	}
	fread(buffer,fileCharLen,1,ptr);
	fclose(ptr);

	// Write the buffer size of bytes to the client	
	err = write(sockfd,&bufferSize,sizeof(bufferSize));
	if(err < 0){
		error("Writing to Socket\n");
	}

	// Write the chat log file contents to the file
	err = write(sockfd,buffer,bufferSize);
	if(err < 0){
		error("Writing to Socket\n");
	}
	free(buffer);
}

// Return EXIT_ESC if user pressed ESC
// Return EXIT_CTRL_C if user pressed Ctrl-C
int realTimeThreadChat(int sockfd,int chatThreadId){
	int err;
	int breakCode;
	int termCode;
	char mssg[256];	
	while(1){

		// Break if client pressed ESC
		breakCode = breakCurrentAction(sockfd);
		if(breakCode == EXIT_ESC){
			return EXIT_ESC;
		}
		
		// Break if client pressed CTRL-C
		if(breakCode == EXIT_CTRL_C){
			return EXIT_CTRL_C;
		}

		// read() and write() the message from the client back to 
		// the client.
		readMessage(sockfd,mssg,sizeof(mssg));
		writeMssgAllClients(sockfd,chatThreadID,mssg,sizeof(mssg));
	}
	return 0;
}

// Removes the user from the chat thread array
void removeUserFromChat(int sockfd,int chatThreadIndex){
	int userIndex = 0;
	int count = thread_chat_count[chatThreadIndex];
	for(int i = 0;i < count;i++){
		if(thread_chat_list[chatThreadIndex][i] == sockfd)
			break;
		userIndex++;
	}
	int next;
	for(int i = userIndex;i < count - 1;i++){
		next = thread_chat_list[chatThreadIndex][i + 1];
		thread_chat_list[chatThreadIndex][i] = next;
	}
	thread_chat_count[chatThreadIndex]--;
}

int chatThreadDriver(int sockfd){
	char optionChar;
	char countNamesChar;
	char filePath[50];
	int err;
	int countNames;
	int optionInt;
	int validOption = 0;

	// Iterate while the option chosesn is not valid
	while(!validOption){

		// Writes the chat threads names to client
		countNames = writeChatThreadNames(sockfd);
		err = readChatThreadOption(sockfd,*optionChar);
		if(err == EXIT_ESC) return EXIT_ESC;
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;

		// Validates the chosen chat thread option from client	
		chatGroupsChar = countNames + '\0';
		if('1' <= optionChar && optionChar <= countNamesChar){
			validOption = 1;	
		}
		else{
			// Write error message if invalid
			err = write(sockfd,"Invalid Option",sizeof(14));
			if(err < 0) error("Writing to Socket\n");
		}
	}

	// Adds the user to the chat thread array
	optionInt = convertCharToInt(option);
	addUserToChatThread(sockfd,optionInt);
	
	// 1 - Generate chat log file
	// 2 - writes the chat log to the client
	// 3 - Initiates the real time chat	
	// 4 - Removes the user from the chat thread array after ESC
	generateChatLogFile(optionChar,filePath);
	writeChatThreadLog(sockfd,filePath);
	err = realTimeThreadChat(sockfd,optionInt);
	removeUserFromChat(sockfd,optionInt);
	
	if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
	if(err == EXIT_ESC) return EXIT_ESC;
	return EXIT_CLEAN;
}

// Delete user from txt file givin the numerical line in the file
void deleteUserAccount(char username[]){
	FILE *ptr = fopen("../Database/Users_Info/users_creds.txt","r");
	if(ptr == NULL){
		printf("users_cred.txt could not be opened\n");
		exit(1);
	}
	FILE *new_ptr = fopen("../Database/Users_Info/temp_users.txt","a");
	if(new_ptr == NULL){
		printf("temp_users.txt could not be opened\n");
		exit(1);
	}

	char u_read[256];	
	char p_read[256];
	while(fscanf(ptr,"%s %s",u_read,p_read) == 2){
		if(strcmp(username,u_read) != 0){
			fprintf(new_ptr,"%s %s\n",u_read,p_read)
		}
	}	

	fclose(ptr);
	fclose(new_ptr);
	remove("../Database/Users_Info/users_creds.txt");
	rename("../Database/Users_Info/temp_users.txt",
	"../Database/users_creds.txt");
}

// Return EXIT_CTRL_C if user pressed Ctrl-C
int serverInnerMenu(int sockfd,char username){
	char option;	
	int err;
	int logout = 0;
	int termCode;
	
	while(1){
		err = readUserOption(sockfd,*option);	
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		if(option == '1'){
			err = chatThreadDriver(sockfd);
			if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		}
		else if(option == '2') return EXIT_CLEAN;
		else if(option == '3'){
			deleteUserAccount(username);
			return EXIT_CLEAN;
		}
	}
	return 0;
}

#endif
