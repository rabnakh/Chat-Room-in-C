#ifndef SERVER_INNER_MENU_H
#define SERVER_INNER_MENU_H

#include <stdio.h>
#include <strings.h>
#include "Server_Auxiliary/server_aux.h"

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
void serverInnerMenu(int sockfd,int line){
	char option;	
	int logout = 0;
	
	printf("ENTERED INNER MENU\n");
	while(!logout){
		option = readUserOption(sockfd);	
		printf("IM Option: %c\n",option);
		
		if(option == '1'){
			logout = 1;
		}
		else if(option == '2'){
			deleteFileLine(line);
			logout = 1;
		}
	}
}

#endif
