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

extern int thread_chat_count[8];
extern int thread_chat_list[8][10];

struct userChatThreadInfo{
	int sockfd;
	int chatThreadID;
	int termID;
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
	int count = thread_chat_count[threadID];
	int breakClientRead = 0;
	for(int i = 0;i < count;i++){
		chatThreadSockfd = thread_chat_list[threadID][i];
		if(chatThreadSockfd != sockfd){
			// Write that the client read() wont break loop
			n = write(chatThreadSockfd,&breakClientRead,
			sizeof(int));
			if(n < 0) error("ERROR: Writing to Socket\n");

			// Write the message to the client
			n = write(chatThreadSockfd,mssg,size);
			if(n < 0) error("ERROR: Writing to Socket\n");
		}
	}
}

void writeMssgToChatLog(char filePath[],char mssg[]){
	FILE *ptr = fopen(filePath,"a");
	if(ptr == NULL) error("File count not be opened\n");	
	fprintf(ptr,"%s\n",mssg);
	fclose(ptr);
}

// Writes() the names of the threads chat to the client
int writeChatThreadNames(int sockfd){
	FILE *ptr = fopen("../Database/Chat_Thread_Names/"
	"chat_thread_names.txt","r");	
	char *buffer;
	int err;
	int bufferSize;
	int fileCharLen;
	int countNames = 0;

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
			countNames++;
	}
	free(buffer);
	return countNames;
}

// Read() the chat thread option
// Return EXIT_CTRL_C if user pressed Ctrl-C
int readChatThreadOption(int sockfd, char *option){
	int err;

	// Reads the chat thread option from client
	err = readUserOption(sockfd,option);
	if(err == EXIT_CTRL_C){
		return EXIT_CTRL_C;
	}

	return EXIT_CLEAN;
}

// Converts a given char to its integer equivalant
int convertCharToInt(char option){
	char optionArr[2];
	int threadIndex;
	
	optionArr[0] = option;
	optionArr[1] = '\0';
	threadIndex = atoi(optionArr);

	return threadIndex;
}

// Creates the file path name for the chat log and stores in a sturct
void generateChatLogFilePath(char option,char filePath[]){
	strcat(filePath,"../Database/Thread_Chat_Logs/thread_");
	strcat(filePath,&option);
	strcat(filePath,".txt");
}

// Appends user to the chat thread array for the chosen chat thread
void addUserToChatThread(int sockfd,int chatThreadIndex){	
	int count = thread_chat_count[chatThreadIndex];
	thread_chat_list[chatThreadIndex][count] = sockfd;
	thread_chat_count[chatThreadIndex]++;
}


// Writes the chat log file contents to the client
void writeChatThreadLog(int sockfd,char filePath[]){
	FILE *ptr;
	char *buffer;
	int err;
	int bufferSize;
	int fileCharLen;

	// Open the chat log file	
	ptr = fopen(filePath,"r");
	if(ptr == NULL){
		error("Opening File\n");
	}

	// Open the chat log file and write the contents to the client
	fseek(ptr,0,SEEK_END);
	fileCharLen = ftell(ptr);
	rewind(ptr);
	bufferSize = fileCharLen * sizeof(char);
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

	/*
	This if-statement was added because if the chat log is empty, then 
	zero bytes will be read() by the client. Then that is the same as 
	if the server pressed CTRL-C and so the error handling for that 
	in the client will execute. So, to avoid that, the client and the 
	server will only read() and write() when the bytes within the 
	chat log is greater than 0.
	*/
	if(bufferSize > 0){
		// Write the chat log file contents to the file
		err = write(sockfd,buffer,bufferSize);
		if(err < 0){
			error("Writing to Socket\n");
		}
		free(buffer);
		buffer = NULL;
	}
}

// Return EXIT_ESC if user pressed ESC
// Return EXIT_CTRL_C if user pressed Ctrl-C
int realTimeThreadChat(int sockfd,int chatThreadID,char filePath[]){
	int err;
	int breakCode;
	int termCode;
	int breakClientRead = 0;
	char mssg[256];	
	while(1){

		// Break if client pressed ESC
		breakCode = breakCurrentAction(sockfd);
		if(breakCode == EXIT_ESC){
			breakClientRead = 1;
			err = write(sockfd,&breakClientRead,sizeof(int));
			if(err < 0) error("CANNOT WRITE TO CLIENT\n");
			printf("BREAK WITH EXIT_ESC FROM RTC\n");
			return EXIT_ESC;
		}
		
		// Break if client pressed CTRL-C
		if(breakCode == EXIT_CTRL_C){
			printf("BREAK WITH EXIT_CTRL_C FROM RTC\n");
			return EXIT_CTRL_C;
		}

		err = readMessage(sockfd,mssg,sizeof(mssg));
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		writeMssgAllClients(sockfd,chatThreadID,mssg,sizeof(mssg));
		writeMssgToChatLog(filePath,mssg);
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
	count = thread_chat_count[chatThreadIndex];
}

int chatThreadDriver(int sockfd){
	char optionChar;
	char countNamesChar;
	char filePath[41];
	int err;
	int countNames;
	int optionInt;
	int validOption;
	int inputExitCode;

	while(1){
		validOption = 0;
		memset(filePath,0,sizeof(filePath));
		err = 0;
		optionChar = 0;
		// Iterate while the option chosesn is not valid
		while(!validOption){

			// Writes the chat threads names to client
			countNames = writeChatThreadNames(sockfd);

			// Read the input break code
			err = read(sockfd,&inputExitCode,
			sizeof(inputExitCode));
			if(err == 0) return EXIT_CTRL_C;
			if(inputExitCode == EXIT_ESC){
				printf("BREAK WITH EXIT_ESC FROM CTD\n");
				return EXIT_ESC;
			}
			
			// Read the chat thread group option
			err = readChatThreadOption(sockfd,&optionChar);
			if(err == EXIT_CTRL_C) return EXIT_CTRL_C;

			// Validates the chosen chat thread option 
			// from client	
			countNamesChar = countNames + '0';
			if('1' <= optionChar && optionChar 
			<= countNamesChar){
				validOption = 1;	
			}
		
			// Write valid option status
			err = write(sockfd,&validOption,sizeof(int));
			if(err < 0) error("Writing to Socket\n");
			/*
			else{
				// Write error message if invalid
				err = write(sockfd,"Invalid Option",
				sizeof(14));
				if(err < 0) error("Writing to Socket\n");
			}
			*/
		}

		// Adds the user to the chat thread array
		optionInt = convertCharToInt(optionChar);
		addUserToChatThread(sockfd,optionInt);
		
		// 1 - Generate chat log file
		// 2 - writes the chat log to the client
		// 3 - Initiates the real time chat	
		// 4 - Removes the user from the chat thread array after ESC
		generateChatLogFilePath(optionChar,filePath);
		writeChatThreadLog(sockfd,filePath);
		err = realTimeThreadChat(sockfd,optionInt,filePath);
		removeUserFromChat(sockfd,optionInt);
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
	}
	return 0;
}

int readBooleanOption(int sockfd,int *booleanOption){
	int err;
	int inputExitCode = 0; 
	while(1){
		// Read to see if user pressed ESC
		err = read(sockfd,&inputExitCode,sizeof(inputExitCode));
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		if(inputExitCode == EXIT_ESC){
			printf("BREAK WITH EXIT_ESC FROM RBC\n");
			return EXIT_ESC;
		}
		// Read the user input boolean option
		err = read(sockfd,booleanOption,sizeof(int));
		if(err == 0) return EXIT_CTRL_C;
		if(*booleanOption == 0 || *booleanOption == 1)
			return EXIT_CLEAN;
	}
	return -1;
}

// Delete user from txt file givin the numerical line in the file
void deleteUserLineFromFile(char username[]){
	FILE *ptr = fopen("../Database/Users_Info/users_cred.txt","r");
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
			fprintf(new_ptr,"%s %s\n",u_read,p_read);
		}
	}	

	fclose(ptr);
	fclose(new_ptr);
	remove("../Database/Users_Info/users_cred.txt");
	rename("../Database/Users_Info/temp_users.txt",
	"../Database/Users_Info/users_cred.txt");
}

int logoutUserAccount(int sockfd,int *booleanOption){
	int err;
	err = readBooleanOption(sockfd,booleanOption);
	return err;
}

int deleteUserAccount(int sockfd,int *booleanOption,char username[]){
	int err;
	err = readBooleanOption(sockfd,booleanOption);	
	if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
	if(*booleanOption == 1) deleteUserLineFromFile(username);
	return err;
}

// Return EXIT_CTRL_C if user pressed Ctrl-C
int serverInnerMenu(int sockfd,char username[]){
	char option;	
	int err;
	int logout = 0;
	int termCode;
	
	while(1){
		err = readUserOption(sockfd,&option);	
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		if(option == '1'){
			err = chatThreadDriver(sockfd);
			if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		}
		else if(option == '2'){
			int booleanOption = 0;
			err = logoutUserAccount(sockfd,&booleanOption);
			if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
			if(booleanOption == 1){
				printf("BREAKING FROM INNERMENU\n");
				break;
			}
		}
		else if(option == '3'){
			int booleanOption = 0;
			err = deleteUserAccount(sockfd,&booleanOption,
			username);
			if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
			if(booleanOption == 1){
				printf("BREAKING FROM INNERMENU\n");
				break;
			}
		}
	}
	return 0;
}

#endif
