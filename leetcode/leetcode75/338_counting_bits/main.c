#include <stdlib.h>
#define MAX_NUMBER (100000)
/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int bits(int a) {
  int i;
  unsigned int temp = a;
  int count = 0;
  for (i = 0; i < sizeof(unsigned int) * 8; i++) {
    if (temp & 0x1) {
      count++;
    }
    temp >>= 1;
  }
  return count;
}

int *countBits(int n, int *returnSize) {
  if (n < 0 || n > MAX_NUMBER || !returnSize) {
    *returnSize = 0;
    return NULL;
  }

  int *answer = (int *)malloc((n + 1) * sizeof(int));
  int i = 0;
  for (i = 0; i <= n; i++) {
    *(answer + i) = bits(i);
  }
  *returnSize = n + 1;
  return answer;
}
