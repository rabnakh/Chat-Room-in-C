#include <stdlib.h>
#include <stdio.h>

void fun1(){
	char mssg[256];
	while(1){
		fgets(mssg,sizeof(mssg),stdin);	
		printf("MSSG: %s",mssg);
	}

}
