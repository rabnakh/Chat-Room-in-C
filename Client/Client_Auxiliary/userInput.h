#ifndef TERMIOSTRING_H
#define TERMIOSTRING_H
#include <termios.h>

extern struct termios initial;

// Only allows a single character to be inputed into the buffer
// Return EXIT_CLEAN if user pressed ENTER and return EXIT_ESC if user 
// pressed ESC
char getSingleChar(char *option,int exitEsc){
	char buffer;
	struct termios new;
	int exit_code = 0;

	new = initial;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO,TCSANOW,&new);
	
	int i = 0;
	while(1){
		buffer = getchar();	
		if(buffer == 27 && exitEsc == 1){
			exit_code = EXIT_ESC;
			*option = '\0';
			break;
		}
		if(i > 0 && buffer == '\n'){
			exit_code = EXIT_CLEAN;
			break;
		}
		if(i > 0 && (buffer == 127)){
			printf("\b \b");
			*option = '\0';
			i--;
		}
		else if(i < 1 && ((47 < buffer && buffer < 58) || 
		(64 < buffer && buffer< 91) || (96 < buffer && 
		buffer < 123))){
			*option = buffer;	
			i++;
			printf("%c",*option);
		}
	}

	tcsetattr(STDIN_FILENO,TCSANOW,&initial);

	return exit_code;
}

// Returns 0 if exit with NEWLINE and returns 1 if exit with ESC
int getString(char string[],int size,int noEcho,int extra){
	char c;
	//struct termios old;
	struct termios new;

	bzero(string,size);
	//tcgetattr(STDIN_FILENO,&old);
	new = initial;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO,TCSANOW,&new);

	int i = 0;
	int exitInput = 0;
	while(1){
		c = getchar();		
		if(c == '\n' && i > 0) break;
		if(c == 27){
			exitInput = 1;
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
	tcsetattr(STDIN_FILENO,TCSANOW,&initial);

	return exitInput;
}

#endif
