#ifndef LOGINMENU_H
#define LOGINMENU_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../Auxiliary/err_handle.h"
#include "Client_Auxiliary/userInput.h"

// Gets the login option from the client and send the option chosen to the
// server.
char getLoginMenuOption(int sockfd){
	int err;
	char option;
	printf("1 - New User\n");
	printf("2 - Current User\n");
	printf("Option: ");
	option = getSingleChar();
	err = write(sockfd,&option,sizeof(option));
	if(err < 0) error("ERROR: Writing to Socket");
	return option;
}

// Return 0 if ESC is pressed. Return 1 if successfull GET
int getUserProfile(char username[],char password[]){
	printf("Username: ");
	if(getString(username,11,0) == 0)	
		return 0;
	printf("\nPassword: ");
	if(getString(password,11,1) == 0)
		return 0;
	printf("\n");
	return 1;
}

// Write the user profile to the server so that the server can search 
// for its existance in the database.
void writeLogin(int sockfd,char username[],char password[]){
	int err;	
	err = write(sockfd,username,11);
	if(err < 0) error("ERROR writing to socket");
	err = write(sockfd,password,11);
	if(err < 0) error("ERROR writing to socket");
}

// Reads the number of characters to read for the error message
int readNumMssgChars(int sockfd){
	int err;
	int numChars;
	err = read(sockfd,&numChars,sizeof(int));
	if(err == 0) error("ERROR reading from socket");
	return numChars;
}

// Read the error message from server
void readMssg(int sockfd,char mssg[],int numChar){
	int err;
	err = read(sockfd,mssg,numChar);
	if(err == 0) error("ERROR reading from socket");
}

// Read the status code from the server for user creation or login
int readStatusCode(int sockfd){
	int err;
	int code;
	err = read(sockfd,&code,sizeof(code));
	if(err == 0) error("ERROR: Reading from Socket\n");
	return code;
}

// Function to create a new user account
void createNewUser(int sockfd){
	int numMssgChars = 0;
	int newUserCreated = 0;
	char username[11];
	char password[11];
	char mssg[256];

	system("clear");

	while(!newUserCreated){

		bzero(username,sizeof(username));
		bzero(password,sizeof(password));
		bzero(mssg,sizeof(mssg));

		newUserCreated = 0;
		printf("***CREATE NEW USER***\n");
		int g_result = getUserProfile(username,password);

		// Write to server the status of the getUserProfile
		int err = write(sockfd,&g_result,sizeof(int));
		if(err < 0) error("ERROR: Writing to Socket\n");
		if(g_result == 0){
			break;
		}

		// Write new user profile information
		writeLogin(sockfd,username,password);
		
		// Read message and status code 	
		numMssgChars = readNumMssgChars(sockfd);
		readMssg(sockfd,mssg,numMssgChars);	
		newUserCreated = readStatusCode(sockfd);
		system("clear");
		printf("%s\n",mssg);
	}	
}

// Return 0 if pressed ESC. Return 1 if login successful
int loginCurrentUser(int sockfd){
	int loginCode = 0;
	int numMssgChars;
	char username[11];
	char password[11];
	char mssg[256];

	system("clear");
	while(loginCode < 2){

		bzero(username,sizeof(username));
		bzero(password,sizeof(password));
		bzero(mssg,sizeof(mssg));

		printf("***RETURNING USER LOGIN***\n");
		int g_result = getUserProfile(username,password);

		// Write to server the status of the getUserProfile
		int err = write(sockfd,&g_result,sizeof(int));
		if(err < 0) error("ERROR: Writing to Socket\n");	
		if(g_result == 0){
			loginCode = 0;	
			break;
		}

		// Write login info to server
		writeLogin(sockfd,username,password);

		// Read message and status code
		numMssgChars = readNumMssgChars(sockfd);
		readMssg(sockfd,mssg,numMssgChars);
		loginCode = readStatusCode(sockfd);
		system("clear");
		printf("%s\n",mssg);
	}		
	return loginCode;
}

// Login Menu Driver
void clientLoginMenu(int sockfd){
	char option;
	int err;
	int loginStatus;

	system("clear");
	while(1){
		option = getLoginMenuOption(sockfd);
		system("clear");
		if(option == '1'){
			createNewUser(sockfd);	
			system("clear");
		}
		else if(option == '2'){
			if(loginCurrentUser(sockfd) > 1){
				system("clear");
				break;
			}
			system("clear");
		}
		else{
			system("clear");
			printf("INVALID OPTION\n");
		}
	}
	system("clear");
	printf("SUCCESSFUL LOGIN\n");
}

#endif
