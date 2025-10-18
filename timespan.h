#ifndef TIMESPAN_H
#define TIMESPAN_H

#include <stddef.h>

typedef struct {
    int hours;
    int minutes;
} Timespan;

void timespan_to_string(const Timespan *span, char *buffer, size_t buffer_size);

#endif
