#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define MISC_DEV "/dev/my_char_dev0"
#define MISC_DEV_BUF (128)
int main() {

  char buffer[MISC_DEV_BUF];
  char message[] =
      "ahfiulewhfuiawefunawekulfnkwuarenfiuwnfiuawnfiunfuiafhiuwanrffff";
  int fd;
  int ret;
  fd = open(MISC_DEV, O_RDWR);
  if (fd < 0) {
    printf("open file fail\n");
    return -1;
  }

  ret = write(fd, message, sizeof(message));
  if (ret < 0) {
    printf("write nr fail.\n");
    goto err;
  }
  sleep(1);

  ret = read(fd, buffer, MISC_DEV_BUF);
  if (ret) {
    if (ret < MISC_DEV_BUF) {
      buffer[ret] = '\0'; // Null-terminate the string
      printf("read %d bytes: %s\n", ret, buffer);
    } else {
      printf("read full buffer: %s\n", buffer);
    }
  } else {
    printf("read fail\n");
  }

err:

  close(fd);

  return 0;
}
