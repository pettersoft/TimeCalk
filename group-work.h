#ifndef GROUP_WORK_H
#define GROUP_WORK_H

#include <stddef.h>
#define TIME_LEN 7

typedef struct {
    char* groupName;
    char (*starts)[TIME_LEN];
    char (*ends)[TIME_LEN];
    int tot_minutes_worked;
    size_t size; // actual count of items added 
    size_t allocated; // allocated, calc: (n x sizeof *ends)
} GroupWork;

void gw_init(GroupWork *gw);
int gw_ensure_kappa(GroupWork *gw, size_t req);
void gw_free(GroupWork *gw);

// void calc_tot_minutes_worked(GroupWork *gw);

#endif