#ifndef CLIENT_INNER_MENU_H
#define CLIENT_INNER_MENU_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "../Auxiliary/err_handle.h"
#include "Client_Auxiliary/userInput.h"

// Print the Client Inner Menu and get() option
char getInnerMenuOption(){
	int err;
	char option;
	printf("1 - Chat Room\n");
	printf("2 - Logout\n");
	printf("3 - Delete Account\n");
	option = getSingleChar();	
	return option;
}

void readRTMssg(void *arg){
	int n;
	char mssg[256];
	int sockfd = (long) arg;
	while(1){
		bzero(mssg,sizeof(mssg));
		n = read(sockfd,mssg,sizeof(mssg));		
		if(n == 0) error("ERROR: Reading from Socket\n");	
		printf("\r%s\n",mssg);
		printf("\r> ");
		fflush(stdout);
	}
}

void writeRTMssg(void *arg){
	int n;
	int err;
	int getResult;
	int sockfd = (long) arg;
	char mssg[256];
	while(1){
		printf("\r> ");
		fflush(stdout);	
		bzero(mssg,sizeof(mssg));
		getResult = getString(mssg,sizeof(mssg),0,1);
		printf("\n");
		// Write the breakStatus to the server
		err = write(sockfd,&getResult,
		sizeof(getResult));
		if(err < 0) error("ERROR: Writing to Socket\n");
		if(getResult == 0) break;

		// Write the message to the server
		err = write(sockfd,mssg,sizeof(mssg));	
		if(err < 0) error("ERROR: Writing to Socket\n");
	}
}


void realTimeChat(int sockfd,char option){
	int err;
	err = write(sockfd,&option,sizeof(option));
	if(err < 0) error("ERROR: Writing to Socket\n");
	system("clear");

	printf("WELCOME TO THE CHATROOM\n");
	pthread_t read_msg_thread;
	pthread_t write_msg_thread;
	pthread_create(&read_msg_thread,NULL,(void *) readRTMssg,
	(void *)(long) sockfd);
	pthread_create(&write_msg_thread,NULL,(void *) writeRTMssg,
	(void *)(long) sockfd);
	while(1);
	close(read_msg_thread);
	close(write_msg_thread);
	printf("RTC Threads Closed\n");
}

/*
void readTimeChat(int sockfd,char option){
	int err;
	int getStringResult;
	char mssg[256];	
	
	err = write(sockfd,&option,sizeof(option));
	if(err < 0) error("ERROR: Writing to Socket\n");
	system("clear");
	
	while(1){
		printf("> ");

		// Get the message from the client and write the status to
		// the server.
		getStringResult = getString(mssg,sizeof(mssg),0);
		printf("HERE\n");
		err = write(sockfd,&getStringResult,
		sizeof(getStringResult));
		printf("%d\n",getStringResult);
		if(err < 0) error("ERROR: Writing to Socket\n");
		if(getStringResult == 0){
			printf("here\n");
			break;
		}

		// Write the message to the server
		err = write(sockfd,mssg,sizeof(mssg));	
		if(err < 0) error("ERROR: Writing to Socket\n");
		err = read(sockfd,mssg,sizeof(mssg));
		printf("Entered Mssg: %s\n",mssg);
	}
}
*/

// Logouts the user from their account
int logoutAccount(int sockfd,char option){
	int err;
	int logout_status;
	int breakout = 0;
	char logout_opt;
	while(!breakout){
		printf("Are you sure you want to logout?\n");
		printf("Y/y or N/n\n");
		logout_opt = getSingleChar();
		if(logout_opt == 'Y' || logout_opt == 'y'){
			printf("YES\n");
			breakout = 1;
			logout_status = 1;
			err = write(sockfd,&option,sizeof(option));
			if(err < 0) error("ERROR: Writing to Socket\n");
		}
		else if(logout_opt == 'N' || logout_opt == 'n'){
			printf("NO\n");
			breakout = 1;
			logout_status = 0;
		}
		else{
			system("clear");	
			printf("INVALID OPTION\n");
		}
	}
	return logout_status;
}

// Return 0 if not deleted, Return 1 if deleted
int deleteAccount(int sockfd,int option){
	int err;
	int delete_status;
	int breakout = 0;
	char delete_opt;
	
	while(!breakout){
		printf("Are your sure you want to delete?\n");
		printf("Press Y/y or N/n\n");
		delete_opt = getSingleChar();
		if(delete_opt == 'Y' || delete_opt == 'y'){
			breakout = 1;
			delete_status = 1;
			err = write(sockfd,&option,sizeof(delete_opt));
			if(err < 0) error("ERROR: Writing to Socket\n");
		}
		else if(delete_opt == 'N' || delete_opt == 'n'){
			breakout = 1;
			delete_status = 0;
		}
		else{
			system("clear");
			printf("INVALID OPTION\n");
		}
	}
	return delete_status;	
}

void clientInnerMenu(int sockfd){
	char option;
	while(1){
		option = getInnerMenuOption(sockfd);
		system("clear");

		if(option == '1'){
			realTimeChat(sockfd,option);
			system("clear");
		}
		else if(option == '2'){
			if(logoutAccount(sockfd,option) == 1) break;
			system("clear");
		}
		else if(option == '3'){
			if(deleteAccount(sockfd,option) == 1) break;
			system("clear");
		}
		else{
			system("clear");
			printf("INVALID OPTION\n");
		}
	}
} 

#endif
