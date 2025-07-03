#include <stdlib.h>
#include <string.h>

char * mergeAlternately(char * word1, char * word2) {
    if (!word1) word1 = "";
    if (!word2) word2 = "";

    int len1 = strlen(word1), len2 = strlen(word2);
    int maxLen = len1 + len2;
    char * result = (char *)malloc(maxLen + 1); // +1 for '\0'

    int i = 0, j = 0, k = 0;
    while (i < len1 || j < len2) {
        if (i < len1) result[k++] = word1[i++];
        if (j < len2) result[k++] = word2[j++];
    }
    result[k] = '\0';
    return result;
}
