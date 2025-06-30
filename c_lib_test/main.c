#include <stdio.h>
#include <stdlib.h>
#include "ipc_tool.h"


int main(){
	hwusb_init(0x1234,0x5678);
	printf("it`s ok\n");
	hwusb_exit();	

	return 0;
}

