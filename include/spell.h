#ifndef SPELL_H
#define SPELL_H

#include <tonc.h>

#define NUM_PLAYER_SPELLS       10
#define NUM_ENEMY_SPELLS        54
#define NUM_AFFINE_SPELLS       28 // (of 32 affine sprites)

#define NUM_SPELLS (NUM_PLAYER_SPELLS+NUM_ENEMY_SPELLS)

typedef struct spell_t {
    POINT pos;
    POINT vel;
    int alive;
} spell_t;

extern spell_t spell_pool[NUM_SPELLS];

void init_spells(void);
void update_spells(void);

#endif
