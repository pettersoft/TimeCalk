#include <stdio.h>
#include "timespan.h"

void timespan_to_string(const Timespan *timespan, char *buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0U) {
        return;
    }

    if (!timespan) {
        buffer[0] = '\0';
        return;
    }
    
    (void)snprintf(buffer, buffer_size, "%d:%02d", timespan->hours, timespan->minutes);
}