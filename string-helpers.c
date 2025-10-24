#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string-helpers.h"

void removeWhitespaces(char *str) {
  int len = strlen(str);
  char newArr[len+1];
  int charPos = 0;

  for (int i = 0; i < len; i++) {
    unsigned char c = (unsigned char)str[i];

    if (c != ' ')
    {
        newArr[charPos] = (char)c;
        charPos++;
    }
  }

  newArr[charPos] = '\0';
  memcpy(str, newArr, charPos + 1);
}

void str_alloc_copy(char **dest, char *src) {
  char *s = malloc(sizeof(char) * strlen(src) + 1);
  
  strcpy(s, src);

  *dest = s;
}
