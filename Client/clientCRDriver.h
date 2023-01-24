#ifndef CLIENT_DRIVER_H
#define CLIENT_DRIVER_H
#include "userInput.h"
#include "loginMenu.h"
#include "logout.h"
#include "deleteAccount.h"

char getInnerMenuOption(int sockfd){
	int err;
	char option;
	printf("1 - Logout\n");
	printf("2 - Delete Account\n");
	option = getSingleChar();	
	err = write(sockfd,&option,sizeof(option));
	if(err < 0) error("ERROR: Writing to Socket\n");
	return option;
}


void client_driver(int sockfd){

	while(1){
		loginMenu(sockfd);
		while(1){
			char option;
			option = getInnerMenuOption(sockfd);
		
			if(option == '1'){
				//logout();
				break;
			}
			else if(option == '2'){
				deleteAccount();
				break;
			}
			else{
				system("clear");
				printf("***Invalid option***\n");
			}
			system("clear");	
		}
	}
}



#endif
