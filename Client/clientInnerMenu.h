#ifndef CLIENT_INNER_MENU_H
#define CLIENT_INNER_MENU_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "../Auxiliary/err_handle.h"
#include "Client_Auxiliary/userInput.h"

extern int sockfd;

// Print the Client Inner Menu and get() option
// Returns EXIT_CLEAN if user prssed ENTER and EXIT_ESC if pressed ESC
int getInnerMenuOption(char *option){
	int err;
	printf("1 - Chat Room\n");
	printf("2 - Logout\n");
	printf("3 - Delete Account\n");
	printf("Option: ");
	err = getSingleChar(option,0);	
	return err;
}

// Reads the chat thread names from the server and prints them
int readChatThreadNames(char **buffer){
	int err = 0;
	int bufferSize = 0;

	// Read the size of the content	
	err = read(sockfd,&bufferSize,sizeof(bufferSize));
	if(err == 0) return EXIT_CTRL_C;

	// Read the content material	
	*buffer = (char *)malloc(bufferSize);	
	err = read(sockfd,*buffer,bufferSize);
	if(err == 0) return EXIT_CTRL_C;

	return EXIT_CLEAN;
}

// Reads the chat log file content
int readChatThreadLog(char **buffer){
	int err;
	int bufferSize;

	err = read(sockfd,&bufferSize,sizeof(bufferSize));
	if(err == 0) return EXIT_CTRL_C;

	/*
	This if-statement was added because if the chat log is empty, then 
	zero bytes will be read() by the client. Then that is the same as 
	if the server pressed CTRL-C and so the error handling for that 
	in the client will execute. So, to avoid that, the client and the 
	server will only read() and write() when the bytes within the 
	chat log is greater than 0.
	*/
	if(bufferSize > 0){
		*buffer = (char *)malloc(bufferSize);
		err = read(sockfd,*buffer,bufferSize);
		if(err == 0) return EXIT_CTRL_C;
	}
	return EXIT_CLEAN;
}

void readRTMssg(){
	int n;
	char mssg[256];
	int count = 0;
	int breakOut = 0;

	while(1){
		// Read from Server whether user pressed ESC to break from 
		// the infinite loop and end the thread
		n = read(sockfd,&breakOut,sizeof(breakOut));
		if(n == 0) error("ERROR: Reading from Socket\n");
		if(breakOut == 1) break;

		// Read from the message from the Server and print
		bzero(mssg,sizeof(mssg));
		n = read(sockfd,mssg,sizeof(mssg));		
		if(n == 0) error("ERROR: Reading from Socket\n");	
		printf("\r%s\n",mssg);
		printf("\r> ");
		fflush(stdout);
	}
}

void writeRTMssg(){
	int n;
	int err;
	int getResult;
	char mssg[256];

	while(1){
		printf("\r> ");
		fflush(stdout);	
		bzero(mssg,sizeof(mssg));
		getResult = getString(mssg,sizeof(mssg),0,1);
		printf("\n");

		// Write the breakStatus to the server
		err = write(sockfd,&getResult,sizeof(getResult));
		if(err < 0) error("ERROR: Writing to Socket\n");
		if(getResult == EXIT_ESC) break;

		// Write the message to the server
		err = write(sockfd,mssg,sizeof(mssg));	
		if(err < 0) error("ERROR: Writing to Socket\n");
	}
}

void realTimeChat(){
	int err;
	pthread_t read_msg_thread;
	pthread_t write_msg_thread;

	pthread_create(&read_msg_thread,NULL,(void *) readRTMssg,NULL);
	pthread_create(&write_msg_thread,NULL,(void *) writeRTMssg,NULL);
	pthread_join(read_msg_thread,NULL);
	pthread_join(write_msg_thread,NULL);
}

// Returns EXIT_ESC if user pressed ESC
int chatThreadDriver(){
	int err;
	int exitInputCode;
	int validOption;
	char *buffer;
	char option;

	while(1){
		validOption = 0;
		buffer = NULL;
		while(!validOption){

			// Read the names of the chat threads
			err = readChatThreadNames(&buffer);
			if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
			if(buffer != NULL) printf("%s\n",buffer);

			// Get and Write the chat thread option
			// to the server
			printf("Option: ");
			exitInputCode = getSingleChar(&option,1);

			// Write the exit status of the option get
			err = write(sockfd,&exitInputCode,
			sizeof(exitInputCode));
			if(err < 0) return EXIT_CTRL_C;
			if(exitInputCode == EXIT_ESC){
				free(buffer);
				buffer = NULL;
				system("clear");
				return EXIT_ESC;
			}
			
			// Write the option to the server
			err = write(sockfd,&option,sizeof(option));
			if(err < 0) return EXIT_CTRL_C;
		
			// Read if the option was a valid choice
			err = read(sockfd,&validOption,sizeof(validOption));
			if(err == 0) return EXIT_CTRL_C;

			free(buffer);
			buffer = NULL;
			system("clear");
		}
		// Read the chat thread log
		err = readChatThreadLog(&buffer);
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		if(buffer != NULL) printf("%s\n",buffer);
	
		free(buffer);
		buffer = NULL;
		
		realTimeChat();
		system("clear");
	}
	return 0;
}

int writeBooleanOption(char *prompt,int *booleanOption){
	int err;
	int breakout = 0;
	int inputExitCode;
	char option;
	
	while(!breakout){
		printf("%s\n",prompt);
		printf("Press Y/y or N/n\n");
		inputExitCode = getSingleChar(&option,1);

		// Write the input exit code to server
		err = write(sockfd,&inputExitCode,sizeof(inputExitCode));
		if(err < 0) return EXIT_CTRL_C;
		if(inputExitCode == EXIT_ESC) return EXIT_ESC;

		// Write the delete account option to the server
		if(option == 'Y' || option == 'y'){
			breakout = 1;
			*booleanOption = 1;
		}
		else if(option == 'N' || option == 'n'){ 
			breakout = 1;
			*booleanOption = 0;
		}
		else{
			*booleanOption = -1;
			system("clear");
			printf("INVALID OPTION\n");
		}
		err = write(sockfd,booleanOption,sizeof(int));
		if(err < 0) return EXIT_CTRL_C;
	}
	return EXIT_CLEAN;
}

int logoutAccount(int *booleanOption){
	int err;
	char *prompt = "Are you sure that you want to logout?";
	err = writeBooleanOption(prompt,booleanOption);
	return err;
}

int deleteAccount(int *booleanOption){
	int err;
	char *prompt = "Are you sure that you want to delete your account?";
	err = writeBooleanOption(prompt,booleanOption);
	return err;
}

int clientInnerMenu(){
	char option;
	int err;
	int invalidOption;
	int exitInputCode;
	while(1){
		invalidOption = 0;
		getInnerMenuOption(&option);
		err = write(sockfd,&option,sizeof(option));
		if(err < 0) error("ERROR: Writing to Socket\n");
		system("clear");

		if(option == '1') chatThreadDriver();
		else if(option == '2'){
			int booleanOption = 0;
			err = logoutAccount(&booleanOption);	
			if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
			if(booleanOption == 1) break;
		}
		else if(option == '3'){
			int booleanOption = 0;
			err = deleteAccount(&booleanOption);
			if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
			if(booleanOption == 1) break;
		}
		else invalidOption = 1;
		system("clear");
		if(invalidOption == 1) printf("Invalid Option\n");
	}
	return EXIT_CLEAN;
} 

#endif
