#ifndef SERVER_INNER_MENU_H
#define SERVER_INNER_MENU_H

#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>
#include "Server_Auxiliary/server_aux.h"

extern int usersCount;
extern int users[100];

// Returns 1 if users terminated with CTRL-C
int readMessage(int sockfd,char mssg[],int size){
	int err;
	err = read(sockfd,mssg,size);
	if(err == 0){
		perror("ERROR: Reading from Socket\n");
		return 1;
	}
	return 0;
}

void writeMssgSingleClient(int recSockfd,char mssg[],int size){
	int n;
	n = write(recSockfd,mssg,size);
	if(n < 0) perror("ERROR: Writing to Socket: %d\n",recSockfd);
}

void writeMssgAllClients(int sockfd,char mssg[],int size){
	int n;
	printf("user count: %d\n",usersCount);
	for(int i = 0;i < usersCount;i++){
		if(users[i] != sockfd){
			printf("here\n");
			n = write(users[i],mssg,size);
			if(n < 0) error("ERROR: Writing to Socket\n");
		}
	}
}


// Return 0 if ESC
// Return 1 if CTRL-C 
int realTimeChat(int sockfd){
	int err;
	int breakCode;
	int termCode;
	char mssg[256];	
	while(1){

		// Break if client pressed ESC
		breakCode = breakToLoginMenu(sockfd);
		if(breakCode == 1){
			termCode = 0;	
			break;
		}
		else if(breakCode == -1){
			termCode = 1;
			break;
		}

		// read() and write() the message from the client back to 
		// the client.
		breakCode = readMessage(sockfd,mssg,sizeof(mssg));
		if(breakCode == 1){
			termCode = 1;
			break;
		}
		writeMssgAllClients(sockfd,mssg,sizeof(mssg));
		printf("%s\n",mssg);
	}
	return termCode;
}

// Return 0 if ESC
// Return 1 if CTRL-C
int directMssg(int sockfd){
	int 	
}

// Delete user from txt file givin the numerical line in the file
void deleteFileLine(int x){
	FILE *ptr = fopen("../Database/DATABASE.txt","r");
	if(ptr == NULL){
		printf("DATABASE.txt could not be opened\n");
		exit(1);
	}
	FILE *new_ptr = fopen("../Database/HOLD_DB.txt","a");
	if(new_ptr == NULL){
		printf("HOLD_DB.txt could not be opened\n");
		exit(1);
	}
	int i = 0;
	char line[256];
	bzero(line,sizeof(line));

	while(fgets(line,sizeof(line),ptr)){
		line[strcspn(line,"\n")] = '\0';
		if(i != x) fprintf(new_ptr,"%s\n",line);
		i++;
	}
	fclose(ptr);
	fclose(new_ptr);
	remove("../Database/DATABASE.txt");
	rename("../Database/HOLD_DB.txt","../Database/DATABASE.txt");
}

// Server Driver for the Inner Menu after succesful login
// Returns 0 if logouts normally
// Returns 1 if terminates with CTRL-C
int serverInnerMenu(int sockfd,int line){
	char option;	
	int logout = 0;
	int termCode;
	
	printf("ENTERED INNER MENU\n");
	while(!logout){
		option = readUserOption(sockfd);	
		printf("IM Option: %c\n",option);
		if(option == '0'){
			termCode = 1;
			break;
		}	
		if(option == '1'){
			termCode = realTimeChat(sockfd);	
			if(termCode == 1) break;
		}
		else if(option == '2'){
			
		}
		else if(option == '3'){
			logout = 1;
		}
		else if(option == '4'){
			deleteFileLine(line);
			logout = 1;
		}
	}
	return termCode;
}

#endif
