#ifndef CLIENT_INNER_MENU_H
#define CLIENT_INNER_MENU_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../Auxiliary/err_handle.h"
#include "Client_Auxiliary/userInput.h"

// Print the Client Inner Menu and get() option
char getInnerMenuOption(){
	int err;
	char option;
	printf("1 - Logout\n");
	printf("2 - Delete Account\n");
	option = getSingleChar();	
	return option;
}

// Logouts the user from their account
int logoutAccount(int sockfd,int option){
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
			if(logoutAccount(sockfd,option) == 1) break;
			system("clear");
		}
		else if(option == '2'){
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
