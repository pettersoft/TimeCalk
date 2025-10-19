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
#define WORKDAY_IN_MINUTES 480

int validateValue(const char *str);
void pasteInput(size_t *count, char starts[][TIME_LEN], char ends[][TIME_LEN]);
void iterateTimes(size_t *count, char starts[][TIME_LEN],
                  char ends[][TIME_LEN]);
void parseToWrappers(size_t count, const char starts[][TIME_LEN],
                     const char ends[][TIME_LEN], Timespan work[count]);
int parseToMinutes(const char *input);

int main() {
  char starts[MAX_ROWS][TIME_LEN] = {{0}};
  char ends[MAX_ROWS][TIME_LEN] = {{0}};
  size_t count = 0;

  char selection[8];
  int mode;
  printf("Välj läge, 0=standard, 1=Klister\n");

  if (fgets(selection, sizeof selection, stdin) == NULL)
    return 1;
  mode = atoi(&selection[0]);

  if (mode == 0) {
    iterateTimes(&count, starts, ends);

  } else if (mode == 1) {
    pasteInput(&count, starts, ends);
  }

  printf("Du valde läge %s", selection);

  Timespan work[count];
  parseToWrappers(count, starts, ends, work);

  printf("Beräkning slutförd\n");
  int totalHours = 0, totalMinutes = 0;
  for (size_t i = 0; i < count; i++) {
    Timespan t = work[i];

    totalHours += t.hours;
    totalMinutes += t.minutes;
  }

  int totmin = (totalHours * 60) + totalMinutes;
  int totalWorkHours = totmin / 60;
  int totWorkMins = totmin % 60;

  printf("Det innebär att du har arbetat: %d timmar och %d minuter.\n",
         totalWorkHours, totWorkMins);

  if (totmin < WORKDAY_IN_MINUTES) {
    int rest = WORKDAY_IN_MINUTES - totmin;

    int restH = rest / 60;
    int restM = rest % 60;

    printf("Det innebär att du har %d timmar och %d minuter frånvaro\n", restH,
           restM);
  }

  return 0;
}

int parseToMinutes(const char *input) {
  int hours, minutes, calc;
  char h[3], m[3];
  memcpy(h, input, 2);
  memcpy(m, input + 3, 2);
  h[2] = '\0';
  m[2] = '\0';

  hours = atoi(h);
  minutes = atoi(m);

  calc = (hours * 60) + minutes;

  return calc;
}

void parseToWrappers(size_t count, const char starts[][TIME_LEN],
                     const char ends[][TIME_LEN], Timespan workDone[count]) {
  for (size_t i = 0; i < count; i++) {
    int startMinutes = parseToMinutes(starts[i]);
    int endMinutes = parseToMinutes(ends[i]);

    int workedTime = endMinutes - startMinutes;

    if (workedTime < 0) {
      fprintf(stderr, "Angivet intervall är omöjligt, du kan ej arbeta bakåt");
      workedTime = 0;
    }

    workDone[i] = (Timespan){workedTime / 60, workedTime % 60};
  }
}

void pasteInput(size_t *count, char starts[][TIME_LEN], char ends[][TIME_LEN]) {
  if (starts && ends && *count) {
  }
  printf("Klistra in...\n");
  char line[32];
  while (*count < MAX_ROWS && fgets(line, sizeof(line), stdin)) {
    size_t delPos = strcspn(line, "->");
    if (delPos <= 0) {
      fprintf(stderr, "Ingen -> pil angiven");
    }

    size_t len = strcspn(line, "\n");
    if (len == 0)
      break;

    if (sscanf(line, "%6s -> %6s", starts[*count], ends[*count]) == 2) {
      ++(*count);
    } else {
      fprintf(stderr, "Ignorerar rad: %s\n", line);
    }
  }
}

void iterateTimes(size_t *count, char starts[][TIME_LEN],
                  char ends[][TIME_LEN]) {
  char line[LINE_LEN];
  printf("Ange klockslag vid start (hh:mm)\n");
  while (*count < MAX_ROWS) {
    int mode;
    printf("Varv %zu\n", *count);

    if (strlen(starts[*count]) == 0) {
      mode = START_MODE;
    } else if (strlen(ends[*count]) == 0) {
      mode = END_MODE;
    } else {
      mode = COMPLETE_MODE;
    }

    if (mode == START_MODE) {
      printf("Ange starttid (hh:mm)");
    } else if (mode == END_MODE) {
      printf("Ange sluttid (hh:mm)");
    } else {
      printf("Du har angivit '%s' -> '%s'\n", starts[*count], ends[*count]);
      *count += 1;
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

    if (line[0] == '\n' || line[0] == '\0') {
      printf("Loop avbruten\n");
      break;
    }

    if (validateValue(line) == 0) {
      printf("Ogiltig värde '%s' \n", line);
      continue;
    }

    if (mode == 1) {
      sscanf(line, "%6s", starts[*count]);
    } else if (mode == 2) {
      sscanf(line, "%6s", ends[*count]);
    }
  }
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
