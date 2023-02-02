#ifndef TERMIOSTRING_H
#define TERMIOSTRING_H
#include <termios.h>

// Only allows a single character to be inputed into the buffer
char getSingleChar(){
	char option;
	char buffer;
	struct termios old;
	struct termios new;

	tcgetattr(STDIN_FILENO,&old);
	new = old;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO,TCSANOW,&new);
	
	int i = 0;
	while(1){
		buffer = getchar();	
		if(i > 0 && buffer == '\n')
			break;
		if(i > 0 && (buffer == 127)){
			printf("\b \b");
			option = '\0';
			i--;
		}
		else if(i < 1 && ((47 < buffer && buffer < 58) || 
		(64 < buffer && buffer< 91) || (96 < buffer && 
		buffer < 123))){
			option = buffer;	
			i++;
			printf("%c",option);
		}
	}

	tcsetattr(STDIN_FILENO,TCSANOW,&old);

	return option;
}

// Returns 0 if exit with escape and returns 1 if exit with newline
int getString(char string[],int size,int noEcho,int extra){
	char c;
	struct termios old;
	struct termios new;

	bzero(string,size);
	tcgetattr(STDIN_FILENO,&old);
	new = old;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO,TCSANOW,&new);

	int i = 0;
	int exitInput = 0;
	while(1){
		c = getchar();		
		if(c == '\n' && i > 0){
			exitInput = 1;
			break;
		}
		if(c == 27){
			exitInput = 0;
			break;
		}
		if(i > 0 && (c == 127)){
			if(noEcho == 0) printf("\b \b");
			string[--i] = '\0';
		}
		else if(i < size - 1){
			if((extra == 0) && ((47 < c && c < 58) ||  
			(64 < c && c < 91) || (96 < c && c < 123))){
				string[i++] = c;
				if(noEcho == 0) printf("%c",c);
			}
			else if((extra == 1) && (31 < c && c < 127)){
				string[i++] = c;
				if(noEcho == 0) printf("%c",c);
			}
		}
	}
	string[i + 1] = '\0';
	tcsetattr(STDIN_FILENO,TCSANOW,&old);

	return exitInput;
}

#endif
