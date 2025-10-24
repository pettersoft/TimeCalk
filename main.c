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

  char line[32];
  GroupWork curr_grp;

  gw_init(&curr_grp);
  curr_grp.groupName = "Standard";
  printf("Grupp med namn %s initialiserad\n", curr_grp.groupName);

  int lap = 0;
  
  while (*count < MAX_ROWS && fgets(line, sizeof line, stdin)) {
    printf("\n\nTolkar data...\n\n”");
    removeWhitespaces(line);
    if (line[0] == '#' && *count == 0) {
      str_alloc_copy(&curr_grp.groupName, line);
      printf("Kopierade gruppnamn %s\n", curr_grp.groupName);
      
      ++(*count);
      continue;
    } else if (line[0] == '#' && *count > 0) {
      // This means we should add the group to the array of groups
      // and init a new group
      tot[lap] = curr_grp;
      ++lap;

      gw_init(&curr_grp);
      
      str_alloc_copy(&curr_grp.groupName, line);
      printf("Initierar ny grupp, kopierar '%s' som gruppnamn\n", curr_grp.groupName);
      ++(*count);
      continue;
    } else {
      size_t delPos = strcspn(line, "->");
      if (delPos <= 0) {
        fprintf(stderr, "Ingen -> pil angiven");
      }

      size_t len = strcspn(line, "\n");
      if (len == 0)
        break;

      gw_ensure_kappa(&curr_grp, curr_grp.size + 1);

      if (sscanf(line, "%5s->%5s", curr_grp.starts[*count],
                 curr_grp.ends[*count]) == 2) {
                  printf("Scan lyckades, tolkade %s till %s och %s\n", line, curr_grp.starts[*count], curr_grp.ends[*count]);
        ++(*count);
        curr_grp.size++;
      } else {
        fprintf(stderr, "Ignorerar rad: %s\n", line);
      }
    }
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
