#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#define MISC_DEV     "/dev/my_misc_driver"
#define MISC_DEV_BUF (60)
int
main(int argc, int *argv[])
{
	int	      ret;
	struct pollfd fds[2];
	char	      buffer0[64];
	char	      buffer1[64];

	fds[0].fd = open("/dev/mydemo0", O_RDWR);
	if (fds[0].fd == -1)
		goto fail;
	fds[0].events  = POLLIN;
	fds[0].revents = 0;

	fds[1].fd = open("/dev/mydemo1", O_RDWR);
	if (fds[1].fd == -1)
		goto fail;
	fds[1].events  = POLLIN;
	fds[1].revents = 0;

	while (1) {
		ret = poll(fds, 2, -1);
		if (ret == -1)
			goto fail;

		if (fds[0].revents & POLLIN) {
			ret = read(fds[0].fd, buffer0, 64);
			if (ret < 0)
				goto fail;
			printf("%s\n", buffer0);
		}
		if (fds[1].revents & POLLIN) {
			ret = read(fds[1].fd, buffer1, 64);
			if (ret < 0)
				goto fail;
			printf("%s\n", buffer1);
		}
	}

fail:
	perror("poll test");

	return 0;
}
