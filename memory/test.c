#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main()
{
	
	char buffer[5];
	char *ptr;

	ptr = (char*)malloc( 5 * sizeof(char));

	while(1){
		sleep(8);
		*(ptr+1) = 12;
		
	}
	free(ptr);
		
	

	return 0;
}
