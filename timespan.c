#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include "timespan.h"

void parseTimespans(size_t count, const char starts[][TIME_LEN],
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