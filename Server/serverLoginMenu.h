#ifndef LOGIN_MENU_SERVER_H
#define LOGIN_MENU_SERVER_H

#define EXIT_CLEAN 0
#define EXIT_ESC 1
#define EXIT_CTRL_C 2

#include <stdio.h>
#include <stdlib.h> // exit()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_addr()
#include <unistd.h> // read(), write()
#include <strings.h> // bzero()
#include <string.h>
#include <pthread.h>
#include "../Auxiliary/err_handle.h"
#include "Server_Auxiliary/server_aux.h"

// Append new new to txt file
void appendNewUser(char username[],char password[]){
	FILE *ptr = fopen("../Database/Users_Info/users_cred.txt","a");
	if(ptr == NULL){
		error("File could not be opened\n");
	}
	fprintf(ptr,"%s %s\n",username,password);
	fclose(ptr);
}

// Read the login credentials from the client
// Returns 1 is client CTRL-C
// Else returns 0
int readLogin(int sockfd,char username[],char password[]){
	int n;
	n = read(sockfd,username,11);		
	if(n == 0){
		perror("ERROR: Reading from Socket\n");
		return EXIT_CTRL_C;
	}
	n = read(sockfd,password,11);
	if(n == 0){
		perror("ERROR: Reading from Socket\n");
		return EXIT_CTRL_C;
	}	
	printf("READ LOGIN INFO: %s %s\n",username,password);
	return EXIT_CLEAN;
}

// Search full profile with username and password, and returns search code
// Return 2 if found,1 if password incorrect, and 0 for not found
int searchFullProfile(char username[],char password[]){
	FILE *ptr = fopen("../Database/Users_Info/users_cred.txt","r");
	if(ptr == NULL){
		error("Cannot open file\n");
		exit(1);
	}
	char dbu[11];
	char dbp[11];
	while(fscanf(ptr,"%s %s",dbu,dbp) == 2){
		if(strcmp(username,dbu) == 0){
			if(strcmp(password,dbp) == 0){
				fclose(ptr);
				return 2;
			}
			else{
				fclose(ptr);
				return 1;
			}
		}
	}
	fclose(ptr);
	return 0;
}

// Return -1 if username not found
// Return line index where the username was found
int searchUsername(char username[]){
	FILE *ptr = fopen("../Database/Users_Info/users_cred.txt","r");
	char u_n[256];
	bzero(u_n,sizeof(u_n));
	if(ptr == NULL){
		error("FILE NOT OPENED\n");
		exit(1);
	}
	int i = 0;
	while(fscanf(ptr,"%s%*[^\n]",u_n) == 1){
		if(strcmp(username,u_n) == 0)
			return i;
		i++;
	}
	return -1;
}

// Error table for new users
void errMssgTableNewUser(int line,char mssg[]){
	bzero(mssg,30);
	if(line == -1) strcpy(mssg,"PROFILE CREATED");
	else strcpy(mssg,"PROFILE ALREADY EXISTS");
	printf("%s\n",mssg);
}

// Error table for existing users
void errMssgTableExistingUser(int code,char mssg[]){
	if(code == 0) strcpy(mssg,"PROFILE NOT EXISTS");
	else if(code == 1) strcpy(mssg,"INCORRECT PASSWORD");
	else strcpy(mssg,"SUCCESSFUL LOGIN");
}

// Write the number of chars in a message
void writeNumMssgChars(int sockfd,int num){
	int err = write(sockfd,&num,sizeof(int));
	if(err < 0) error("ERROR: Writing to Socket\n");
}

// Write the status code of either user creation or login
void writeStatusCode(int sockfd,int code){
	int err = write(sockfd,&code,sizeof(int));
	if(err < 0) error("ERROR: Writing to Socket\n");
}

// Write message to the client
void writeMssg(int sockfd,char mssg[],int size){
	int err = write(sockfd,mssg,size);
	if(err < 0) error("ERROR: Writing to Socket\n");
}

// bzero() the username, password, and mssg char arrays
void bzeroUPM(char username[],char password[],char mssg[]){
	bzero(username,11);
	bzero(password,11);
	bzero(mssg,30);
}

// Creates a new user profile
// Returns EXIT_CTRL_C if client pressed Ctrl-C
// Returns EXIT_ESC if client pressed Esc
// Returns EXIT_CLEAN if client entered credentials successfully
int serverCreateNewUser(int sockfd){
	int err;
	int line = 0;
	int statusCode = 0;
	int termCode = 0;;
	int breakCode;
	char mssg[30];
	char username[11];
	char password[11];
	while(!statusCode){

		// bzero() the char arrays
		bzeroUPM(username,password,mssg);
	
		err = breakCurrentAction(sockfd);
		if(err == EXIT_ESC) return EXIT_ESC;
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;

		// Read login information
		err = readLogin(sockfd,username,password);
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;

		// Search line of user in database
		line = searchUsername(username);	
		printf("after searchUsername()\n");
		// Append new profile to database
		if(line == -1){
			statusCode = 1;
			appendNewUser(username,password);
		}
	
		// Get err message from table
		errMssgTableNewUser(line,mssg);	

		printf("statusCode: %d\n",statusCode);
		printf("numMssgChars: %ld\n",strlen(mssg));
		// Write message and status code
		writeNumMssgChars(sockfd,strlen(mssg));
		writeMssg(sockfd,mssg,strlen(mssg));
		writeStatusCode(sockfd,statusCode);
	}
	return EXIT_CLEAN;
}

// Returns EXIT_CTRL_C if user presses Ctrl-C
// Returns EXIT_ESC if uses pressed Esc
// Returns EXIT_CLEAN if users logged in successfully
int serverCurrentUser(int sockfd,char u_hold[]){
	int err;
	int statusCode = 0;
	int breakCode;
	char mssg[30];
	char username[11];
	char password[11];
	while(statusCode < 2){		

		// bzero() the char arrays
		bzeroUPM(username,password,mssg);
		
		err = breakCurrentAction(sockfd);
		if(err == EXIT_ESC) return EXIT_ESC;
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		
		// Read login information
		readLogin(sockfd,username,password);
		
		// Search profile in database
		statusCode = searchFullProfile(username,password);

		// Get error message
		errMssgTableExistingUser(statusCode,mssg);
		
		// Write message and status code
		writeNumMssgChars(sockfd,strlen(mssg));
		writeMssg(sockfd,mssg,strlen(mssg));	
		writeStatusCode(sockfd,statusCode);
	}
	strcpy(u_hold,username);
	return EXIT_CLEAN;
}

// Returns EXIT_CTRL_C if client pressed Ctrl-C
// Returns EXIT_CLEAN if client logged successfully
int serverLoginMenu(int sockfd,char username[]){
	int err;
	char option;
	while(1){
		err = readUserOption(sockfd,&option);
		if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		if(option == '1'){
			err = serverCreateNewUser(sockfd);	
			if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		}
		else if(option == '2'){
			err = serverCurrentUser(sockfd,username);
			if(err == EXIT_CLEAN) return EXIT_CLEAN;
			if(err == EXIT_CTRL_C) return EXIT_CTRL_C;
		}
	}
	return EXIT_CLEAN;
}

#endif


