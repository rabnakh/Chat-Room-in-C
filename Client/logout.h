#ifndef LOGOUT_H
#define LOGOUT_H

int logout(int sockfd){
	int err;
	int logout = 1;
	err = write(sockfd,&logout,sizeof(logout));
	if(err < 0) error("ERROR: Writing to Socket\n");
	return logout;
}

#endif
