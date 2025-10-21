#ifndef TIMESPAN_H
#define TIMESPAN_H

#define TIME_LEN 7
#include <stddef.h>

typedef struct {
    int hours;
    int minutes;
} Timespan;

void parseTimespans(size_t count, const char starts[][TIME_LEN], const char ends[][TIME_LEN], Timespan worked[count]);
int parseToMinutes(const char *input);

#endif
