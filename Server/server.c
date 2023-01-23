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

void error(char *msg){
	perror(msg);
	exit(1);
}

// Append new new to txt file
void appendNewUser(char username[],char password[]){
	FILE *ptr = fopen("DATABASE.txt","a");
	fprintf(ptr,"%s %s\n",username,password);
	fclose(ptr);
}

// Return 0 if ENTER pressed, Return 1 if ESC pressed
int breakToLoginMenu(int sockfd){
	int err;
	int esc;
	// Read if user pressed ESC
	err = read(sockfd,&esc,sizeof(esc));
	if(err == 1) error("ERROR: Reading to "
	"Socket\n");
	if(esc == 0){
		printf("RETURN TO LOGIN MENU\n");
		return 1;
	}
	return 0;
}

// Delete user from txt file givin the numerical line in the file
void deleteUser(int x){
	FILE *ptr = fopen("DATABASE.txt","r");
	FILE *new_ptr = fopen("HOLD_DB.txt","a");
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
	remove("DATABASE.txt");
	rename("HOLD_DB.txt","DATABASE.txt");
}

// Read the login credentials from the client
void readLogin(int sockfd,char username[],char password[]){
	int n;
	n = read(sockfd,username,11);		
	if(n == 0) error("ERROR reading from socket");
	n = read(sockfd,password,11);
	if(n == 0) error("ERROR reading from socket");
	
	printf("READ LOGIN INFO: %s %s\n",username,password);
}

// Search full profile with username and password, and returns search code
// Return 2 if found,1 if password incorrect, and 0 for not found
int searchFullProfile(char username[],char password[]){
	FILE *ptr = fopen("DATABASE.txt","r");
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
	FILE *ptr = fopen("DATABASE.txt","r");
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

// Read the option from the client
char readUserOption(int sockfd){
	int n;
	char option;
	n = read(sockfd,&option,sizeof(option));
	if(n == 0) error("ERROR: Reading to Socket\n");
	return option;
}

void serverDriver(int sockfd){
	int esc;
	int line;
	int searchResult;
	int statusCode;
	char option;
	char username[11];
	char password[11];
	char mssg[30];
	while(1){

		// Zero out the variables
		bzero(username,sizeof(username));
		bzero(password,sizeof(password));
		bzero(mssg,sizeof(mssg));
	
		// Read user option
		option = readUserOption(sockfd);	
		printf("Option: %c\n",option);

		// Create a new user
		if(option == '1'){
			statusCode = 0;
			while(!statusCode){

				// Break if client pressed ESC
				if(breakToLoginMenu(sockfd) == 1)
					break;

				// Read login information
				readLogin(sockfd,username,password);
	
				// Search line of user in database
				line = searchUsername(username);	

				// Append new profile to database
				if(line == -1){
					statusCode = 1;
					appendNewUser(username,password);
				}
			
				// Get err message from table
				errMssgTableNewUser(line,mssg);	
				printf("STRLEN: %lu\n",strlen(mssg));
	
				// Write message and status code
				writeNumMssgChars(sockfd,strlen(mssg));
				writeMssg(sockfd,mssg,strlen(mssg));
				writeStatusCode(sockfd,statusCode);
			}
		}

		// Login to existing profile
		else if(option == '2'){	
			statusCode = 0;
			while(statusCode < 2){				

				// Break if client pressed ESC
				if(breakToLoginMenu(sockfd) == 1)
					break;

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
			}
		}
	}
}

int main(int argc, char *argv[]){
	
	int sockfd;
	int portno;
	int clilen;
	int newsockfd;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	FILE *fileptr;
	pthread_t tid;

	if(argc < 2){
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0) error("ERROR opening socket");

	bzero((char *) &serv_addr,sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if(bind(sockfd, 
		(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, 
	(struct sockaddr *) &cli_addr, &clilen);
	if(newsockfd < 0) error("ERROR on accept");
	serverDriver(newsockfd);
	close(sockfd);
	
	return 0;
}
