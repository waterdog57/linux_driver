#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define MISC_DEV "/dev/my_misc_driver"
#define MISC_DEV_BUF (60)
int main()
{
	
	char buffer[MISC_DEV_BUF];
	char message[] = "my message";
	int fd;
	int ret;
	fd = open(MISC_DEV, O_RDWR);
	if( fd < 0 ){
		printf("open file fail\n");
		return -1;
	}

	ret = write(fd, message, sizeof(message));
	printf("%ld", sizeof(message));
	if(ret != sizeof(message)){
		printf("write nr fail.\n");
		return -1;
	}

	ret = read(fd, buffer, MISC_DEV_BUF);
	printf("read %d\n", ret);


	printf("%s\n", buffer);


	close(fd);

	return 0;
}
