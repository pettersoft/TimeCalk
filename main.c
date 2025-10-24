#include "group-work.h"
#include "string-helpers.h"
#include "timespan.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 8
#define TIME_LEN 7
#define LINE_LEN 7

#define MODE_ITERATE '0'
#define MODE_PASTE '1'

#define ITERATE_START_MODE 1
#define ITERATE_END_MODE 2
#define ITERATE_COMPLETE_MODE 3
#define WORKDAY_IN_MINUTES 480

int validateValue(const char *str);
void removeWhitespaces(char *str);
void pasteInput(size_t *count, char starts[][TIME_LEN], char ends[][TIME_LEN]);
void iterateTimes(size_t *count, char starts[][TIME_LEN],
                  char ends[][TIME_LEN]);

void parseGroups(size_t *count, GroupWork tot[MAX_ROWS]);
void printGroups(GroupWork *groups, size_t len);

int main() {
  char starts[MAX_ROWS][TIME_LEN] = {{0}};
  char ends[MAX_ROWS][TIME_LEN] = {{0}};
  GroupWork tot[MAX_ROWS];
  size_t count = 0;

  char selection[8];
  printf("Välj läge, 0=iterativt, 1=Klister\n");
  char *mode;

  if (fgets(selection, sizeof selection, stdin) == NULL)
    return 1;
  mode = &selection[0];

  if (*mode == MODE_ITERATE) {
    iterateTimes(&count, starts, ends);

    Timespan work[count];
    parseTimespans(count, starts, ends, work);

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

      printf("Det innebär att du har %d timmar och %d minuter frånvaro\n",
             restH, restM);
    }
  } else if (*mode == MODE_PASTE) {
    // pasteInput(&count, starts, ends);
    parseGroups(&count, tot);
    printGroups(tot, count);
  } else {
    printf("Felakigt menyval, programmet avbryts...");
    return 0;
  }

  return 0;
}

void parseGroups(size_t *count, GroupWork tot[MAX_ROWS]) {
  printf("Klistra in:\n");

  char line[64];
  GroupWork curr_grp;
  int has_group = 0;

  gw_init(&curr_grp);

  while (fgets(line, sizeof line, stdin)) {
    size_t len = strcspn(line, "\r\n");
    line[len] = '\0';

    if (line[0] == '\0') {
      continue;
    }

    if (line[0] == '#') {
      if (has_group) {
        if (*count >= MAX_ROWS) {
          fprintf(stderr, "Maximalt antal grupper (%d) uppnått.\n", MAX_ROWS);
          gw_free(&curr_grp);
          has_group = 0;
          break;
        }
        tot[*count] = curr_grp;
        ++(*count);
        gw_init(&curr_grp);
      } else {
        has_group = 1;
      }

      char *group_name = line + 1;
      while (*group_name == ' ') {
        ++group_name;
      }
      if (*group_name == '\0') {
        group_name = "Namnlös grupp";
      }
      str_alloc_copy(&curr_grp.groupName, group_name);
      continue;
    }

    if (!has_group) {
      fprintf(stderr, "Rad ignorerad tills en grupp angivits: %s\n", line);
      continue;
    }

    char *arrow = strstr(line, "->");
    if (!arrow) {
      fprintf(stderr, "Ingen -> pil angiven: %s\n", line);
      continue;
    }

    *arrow = '\0';
    char *start = line;
    char *end = arrow + 2;

    while (isspace((unsigned char)*start)) {
      ++start;
    }
    char *start_end = start + strlen(start);
    while (start_end > start && isspace((unsigned char)start_end[-1])) {
      --start_end;
    }
    *start_end = '\0';

    while (isspace((unsigned char)*end)) {
      ++end;
    }
    char *end_end = end + strlen(end);
    while (end_end > end && isspace((unsigned char)end_end[-1])) {
      --end_end;
    }
    *end_end = '\0';

    if (gw_ensure_kappa(&curr_grp, curr_grp.size + 1) != 0) {
      fprintf(stderr, "Kunde inte allokera plats för nya tider.\n");
      gw_free(&curr_grp);
      has_group = 0;
      break;
    }

    strncpy(curr_grp.starts[curr_grp.size], start, TIME_LEN - 1);
    curr_grp.starts[curr_grp.size][TIME_LEN - 1] = '\0';

    strncpy(curr_grp.ends[curr_grp.size], end, TIME_LEN - 1);
    curr_grp.ends[curr_grp.size][TIME_LEN - 1] = '\0';

    curr_grp.size++;
  }

  if (has_group && *count < MAX_ROWS) {
    tot[*count] = curr_grp;
    ++(*count);
  } else {
    gw_free(&curr_grp);
  }
}

void printGroups(GroupWork *groups, size_t len) {
  for (size_t i = 0; i < len; i++){
    printf("Grupp: %s\n", groups[i].groupName);

    printf("Allokerat %lu\n", groups[i].allocated);
    printf("Storlek %lu\n", groups[i].size);

    printf("\n\n");
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
      mode = ITERATE_START_MODE;
    } else if (strlen(ends[*count]) == 0) {
      mode = ITERATE_END_MODE;
    } else {
      mode = ITERATE_COMPLETE_MODE;
    }

    if (mode == ITERATE_START_MODE) {
      printf("Ange starttid (hh:mm)");
    } else if (mode == ITERATE_END_MODE) {
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
