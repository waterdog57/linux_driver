#include <stdio.h>
double findMaxAverage(int *nums, int numsSize, int k) {
  if (k < 1 || k > 100000 || numsSize < k)
    return 0;

  int count = 0;
  double result = 0, temp = 0;
  for (count = 0; count < k; count++) {
    temp += *(nums + count);
  }
  result = temp;

  for (count = k; count < numsSize; count++) {
    temp -= *(nums + count - k);
    temp += *(nums + count);
    if (temp > result)
      result = temp;
  }
  return result / k;
}
