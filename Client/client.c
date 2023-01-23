#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h> // write() and read()
#include <strings.h>
#include <string.h>
#include <pthread.h>
#include "loginMenu.h"

//struct termios old;

int main(int argc,char *argv[]){

	int sockfd;
	int portno;
	int n;
	char buffer[256];
	struct hostent *server;
	struct sockaddr_in serv_addr;

	if(argc < 3){
		fprintf(stderr,"usage %s hostname port\n",argv[0]);
		exit(0);
	}

	//Create socket file descriptor
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		error("ERROR opening socket");
	}

	//Get ip address of the host
	server = gethostbyname(argv[1]);
	if(server == NULL){
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	//Create struct to hold server address and port	
	portno = atoi(argv[2]);
	bzero((char *) &serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = server->h_addrtype;
	bcopy(server->h_addr, &serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);

	if(connect(sockfd,(struct sockaddr *) &serv_addr, 
	sizeof(serv_addr)) < 0) error("ERROR connecting");

	//tcgetattr(STDIN_FILENO,&old);
	loginMenu(sockfd);

	close(sockfd);

	return 0;
}


