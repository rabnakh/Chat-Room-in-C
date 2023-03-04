#ifndef LOGINMENU_H
#define LOGINMENU_H

#define EXIT_CLEAN 0
#define EXIT_ESC 1
#define EXIT_CTRL_C 2

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../Auxiliary/err_handle.h"
#include "Client_Auxiliary/userInput.h"

extern int sockfd;

// Gets the login option from the client and send the option chosen to the
// server.
char getLoginMenuOption(){
	int err;
	int exitInputCode;
	char option;
	printf("1 - New User\n");
	printf("2 - Current User\n");
	printf("Option: ");
	exitInputCode = getSingleChar(&option,0);
	err = write(sockfd,&option,sizeof(option));
	if(err < 0) error("ERROR: Writing to Socket");
	return option;
}

// Return EXIT_CLEAN if successfull GET. Return EXIT_ESC if ESC
int getUserProfile(char username[],char password[]){
	printf("Username: ");
	if(getString(username,11,0,0) == 1)
		return EXIT_ESC;
	printf("\nPassword: ");
	if(getString(password,11,1,0) == 1)
		return EXIT_ESC;
	printf("EXIT_CLEAN\n");
	return EXIT_CLEAN;
}

// Write the user profile to the server so that the server can search 
// for its existance in the database.
void writeLogin(char username[],char password[]){
	int err;	
	err = write(sockfd,username,11);
	if(err < 0) error("ERROR writing to socket");
	err = write(sockfd,password,11);
	if(err < 0) error("ERROR writing to socket");
}

// Reads the number of characters to read for the error message
int readNumMssgChars(int *numChars){
	int err;
	err = read(sockfd,numChars,sizeof(int));
	if(err == 0) return EXIT_CTRL_C;
	return EXIT_CLEAN;
}

// Read the error message from server
int readMssg(char mssg[],int numChar){
	int err;
	err = read(sockfd,mssg,numChar);
	if(err == 0) return EXIT_CTRL_C;
	return EXIT_CLEAN;
}

// Read the status code from the server for user creation or login
int readStatusCode(int *code){
	int err;
	err = read(sockfd,code,sizeof(int));
	if(err == 0) return EXIT_CTRL_C;
	return EXIT_CLEAN;
}

// Function to create a new user account
int createNewUser(){
	int err;
	int get_err;
	int numMssgChars = 0;
	int newUserCreated = 0;
	char username[11];
	char password[11];
	char mssg[256];

	while(!newUserCreated){
		bzero(username,sizeof(username));
		bzero(password,sizeof(password));
		bzero(mssg,sizeof(mssg));

		newUserCreated = 0;
		printf("***CREATE NEW USER***\n");
		get_err = getUserProfile(username,password);

		// Write to server the status of the getUserProfile
		err = write(sockfd,&get_err,sizeof(int));
		if(err < 0) error("ERROR: Writing to Socket\n");
		if(get_err == EXIT_ESC) return EXIT_ESC;

		// Write new user profile information
		writeLogin(username,password);

		// Read message and status code 	
		err = readNumMssgChars(&numMssgChars);
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;

		readMssg(mssg,numMssgChars);	
		err = readStatusCode(&newUserCreated);
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		system("clear");
		printf("%s\n",mssg);
	}
	return EXIT_CLEAN;
}

// Return 0 if pressed ESC. Return 1 if login successful
int loginCurrentUser(){
	int err;
	int get_err;
	int loginCode = 0;
	int numMssgChars;
	char username[11];
	char password[11];
	char mssg[256];

	while(1){
		bzero(username,sizeof(username));
		bzero(password,sizeof(password));
		bzero(mssg,sizeof(mssg));

		printf("***RETURNING USER LOGIN***\n");
		get_err = getUserProfile(username,password);

		// Write to server the status of the getUserProfile
		int err = write(sockfd,&get_err,sizeof(int));
		if(err < 0) error("ERROR: Writing to Socket\n");	
		if(get_err == EXIT_ESC) return EXIT_ESC;

		// Write login info to server
		writeLogin(username,password);

		// Read message and status code
		err = readNumMssgChars(&numMssgChars);
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		err = readMssg(mssg,numMssgChars);
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		err = readStatusCode(&loginCode);
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		if(loginCode == 2) return EXIT_CLEAN;

		system("clear");
		printf("%s\n",mssg);
	}		
	return 0;
}

// Login Menu Driver
void clientLoginMenu(){
	int err;
	int invalidOption;
	int loginStatus = 0;
	char option;

	system("clear");
	while(!loginStatus){
		invalidOption = 0;
		option = getLoginMenuOption();
		system("clear");
		if(option == '1') createNewUser();
		else if(option == '2'){
			err = loginCurrentUser();
			if(err == EXIT_CLEAN) loginStatus = 1;
		}
		else invalidOption = 1;
		system("clear");
		if(invalidOption) printf("INVALID OPTION\n");
			
	}
}

#endif
