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
#include "serverCRDriver.h"

int users_count = 0;
int users[100] = {};

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

	while(1){
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,
		&clilen);
		if(newsockfd < 0) error("ERROR: On accept\n");
		users[users_count] = newsockfd;
		users_count++;
		pthread_create(&tid,NULL,serverDriver, 
		(void *)(long) newsockfd);
	}
	close(sockfd);
	
	return 0;
}
