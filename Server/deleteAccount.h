#ifndef DELETE_ACCOUNT_H
#define DELETE_ACCOUNT_H
#include "userInput.h"

// Return 0 if not deleted, Return 1 if deleted
void deleteAccount(int sockfd){
	int option;
	int deleted = 0;
	
	while(!deleted){
		printf("Are your sure you want to delete?\n");
		printf("Press Y/n or N/n\n")
		option = getSingleChar();
		if(option == 'Y' || option == 'y'){
			deleted = 1;
			
		}
		else if(option == 'n' || option == 'N'){
			deleted = 0;
		}
	}
	return deleted;	
}



#endif
