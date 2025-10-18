#include "tcalc.h"
#include "timespan.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 8
#define TIME_LEN 7
#define LINE_LEN 7

#define START_MODE 1
#define END_MODE 2
#define COMPLETE_MODE 3

int validateValue(const char *str);

int main() {
  printf("bamse\n");

  char starts[MAX_ROWS][TIME_LEN];
  char ends[MAX_ROWS][TIME_LEN];

  int count = 0;

  char line[LINE_LEN];
  printf("Ange klockslag vid start (hh:mm)\n");
  while (count < MAX_ROWS) {
    int mode;
    printf("Varv %d\n", count);

    if (strlen(starts[count]) == 0) {
      mode = START_MODE;
    } else if (strlen(ends[count]) == 0) {
      mode = END_MODE;
    } else {
      mode = COMPLETE_MODE;
    }

    if (mode == START_MODE) {
      printf("Ange starttid (hh:mm)");
    } else if (mode == END_MODE) {
      printf("Ange sluttid (hh:mm)");
    } else {
      printf("Du har angivit '%s' -> '%s'", starts[count], ends[count]);
      count++;
      continue;
    }

    if (!fgets(line, sizeof line, stdin)) {
      printf("Loop avbruten\n");
      break;
    }

    size_t len = strcspn(line, "\n");
    // Convert newline to EOF
    if (line[len] == '\n') {
      line[len] = '\0';
    } else {
      int ch;
      while ((ch = getchar()) != '\n' && ch != EOF) {
      }
      fprintf(stderr, "För lång rad, försök igen.\n");
      continue;
    }

    // How can I Determine that no overflow was given?

    if (strlen(line) == 0) {
      printf("Loop avbruten\n");
      break;
    }

    if (validateValue(line) == 0) {
      printf("Ogiltig värde '%s' \n", line);
      continue;
    }

    if (mode == 1) {
      sscanf(line, "%6s", starts[count]);
    } else if (mode == 2) {
      sscanf(line, "%6s", ends[count]);
    }
  }

  return 0;
}

int validateValue(const char *str) {
  if (strlen(str) < 5) {
    return 0;
  }

  if (!isdigit((unsigned char)str[0]) || !isdigit((unsigned char)str[1]) ||
      str[2] != ':' || !isdigit((unsigned char)str[3]) ||
      !isdigit((unsigned char)str[4])) {
    return 0; // bad format
  }
  return 1;
}
