#include "spell.h"

spell_t spell_pool[NUM_SPELLS];

void init_spells(void) {
    spell_t* s = spell_pool;
    for(int i = 0; i < NUM_SPELLS; ++i, s++) {
        s->alive = 0;
    }
}

void update_spells(void) {
    spell_t* s = spell_pool;
    for(int i = 0; i < NUM_SPELLS; ++i, s++) {
        if(!s->alive) continue;
        s->pos.x += s->vel.x;
        s->pos.y += s->vel.y;
    }
}
