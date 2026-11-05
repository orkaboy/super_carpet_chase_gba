#ifndef WIZARD_H
#define WIZARD_H

#include <tonc.h>

typedef struct wizard_t {
    POINT pos;
    POINT vel;
    int jumping;
    int hp;
} wizard_t;

void init_wizard(wizard_t* wizard);
void update_wizard(wizard_t* wizard);

#endif
