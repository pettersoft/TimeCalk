#include "group-work.h"
#include <stdio.h>
#include <stdlib.h>

void gw_init(GroupWork *gw) {
    if (!gw) return;

    gw->groupName=NULL;
    gw->ends=NULL;
    gw->starts=NULL;
    gw->allocated=0;
    gw->size=0;
}

int gw_ensure_kappa(GroupWork *gw, size_t req) {
    if (!gw) return EXIT_FAILURE;

    if (req <= gw->allocated) return EXIT_FAILURE;

    size_t newKappa = gw->allocated ? gw->allocated * 2 : 8;
    while (newKappa < req) {
        newKappa*=2;
    }

    void *newStarts = realloc(gw->starts,newKappa);
    void *newEnds = realloc(gw->ends, newKappa);

    if (!newStarts || !newEnds) {
        free(newStarts);
        free(newEnds);
        fprintf(stderr, "Kunde inte allokera minne");
        exit(EXIT_FAILURE);
    }

    gw->starts = newStarts;
    gw->ends = newEnds;
    gw->allocated = newKappa;

    return 0;
}

void gw_free(GroupWork *gw) {
    if(!gw)return;

    free(gw->starts);
    free(gw->ends);
    free(gw->groupName);

    gw_init(gw);
}

// void calc_tot_minutes_worked(GroupWork *gw) {

// }