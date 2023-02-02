#ifndef LOGIN_MENU_SERVER_H
#define LOGIN_MENU_SERVER_H

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
	FILE *ptr = fopen("../Database/DATABASE.txt","a");
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
		return 1;		
	}
	n = read(sockfd,password,11);
	if(n == 0){
		perror("ERROR: Reading from Socket\n");
		return 1;
	}	
	printf("READ LOGIN INFO: %s %s\n",username,password);
	return 0;
}

// Search full profile with username and password, and returns search code
// Return 2 if found,1 if password incorrect, and 0 for not found
int searchFullProfile(char username[],char password[]){
	FILE *ptr = fopen("../Database/DATABASE.txt","r");
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
	FILE *ptr = fopen("../Database/DATABASE.txt","r");
	char u_n[256];
	bzero(u_n,sizeof(u_n));

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
	printf("%s\n",mssg);
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
// Returns 1 is user pressed ctrl-c
// Returns 0 if user exited normally
int serverCreateNewUser(int sockfd){
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
	
		breakCode = breakToLoginMenu(sockfd);
		if(breakCode == 1) break;
		if(breakCode == -1){
			termCode = 1;
			break;
		}

		// Read login information
		breakCode = readLogin(sockfd,username,password);
		if(breakCode == 1){
			termCode = 1;
			break;
		}

		// Search line of user in database
		line = searchUsername(username);	

		// Append new profile to database
		if(line == -1){
			statusCode = 1;
			appendNewUser(username,password);
		}
	
		// Get err message from table
		errMssgTableNewUser(line,mssg);	

		// Write message and status code
		writeNumMssgChars(sockfd,strlen(mssg));
		writeMssg(sockfd,mssg,strlen(mssg));
		writeStatusCode(sockfd,statusCode);
	}
	return termCode;
}

// Logins the client after reading in the username and password
// Returns the line of the user within the file database
// If returns -1 then the user ESC
// If returns -2 then the user CTRL-C
int serverCurrentUser(int sockfd){
	int line = -1;
	int statusCode = 0;
	int breakCode;
	char mssg[30];
	char username[11];
	char password[11];
	while(statusCode < 2){		

		// bzero() the char arrays
		bzeroUPM(username,password,mssg);
		
		breakCode = breakToLoginMenu(sockfd);
		if(breakCode == 1) break;
		if(breakCode == -1){
			line = -2;
			break;
		}
		
		// Read login information
		readLogin(sockfd,username,password);
		
		// Search profile in database
		statusCode = searchFullProfile(username,
		password);
		printf("LOGIN CODE: %d\n",statusCode);	

		// Get error message
		errMssgTableExistingUser(statusCode,mssg);
		printf("MSSG: %s\n",mssg);
		
		// Write message and status code
		writeNumMssgChars(sockfd,strlen(mssg));
		writeMssg(sockfd,mssg,strlen(mssg));	
		writeStatusCode(sockfd,statusCode);
		if(statusCode == 2){
			line = searchUsername(username);
		}
	}
	return line;
}

// Server Driver function for the initial login menu
// Returns the line of the user within the file of the server
// Returns -1 if user CTRL-C
// Otherwise it returns the line of the logined user
int serverLoginMenu(int sockfd){
	int line;
	int newUserCode;
	char option;
	while(1){
		option = readUserOption(sockfd);
		printf("Option: %c\n %d\n",option,option);
		if(option == '0'){
			line = -1;
			break;
		}
		
		if(option == '1'){
			line = serverCreateNewUser(sockfd);	
			if(line == 1){
				line = -1;
			}
		}
		else if(option == '2'){
			line = serverCurrentUser(sockfd);
			if(line > -1) break;
			if(line == -2){
				line = -1;
				break;
			}
		}
	}
	return line;
}

#endif


