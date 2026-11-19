#ifndef SPELL_H
#define SPELL_H

#include <tonc.h>
#include "game_state.h"

#define NUM_PLAYER_SPELLS       10
#define NUM_ENEMY_SPELLS        54
#define NUM_AFFINE_SPELLS       28 // (of 32 affine sprites)

#define NUM_SPELLS (NUM_PLAYER_SPELLS+NUM_ENEMY_SPELLS)

// TODO: FIX VALUES
#define PLAYER_SPELL_SPEED float2fx(-4.0)

#define SPELL_COLL_W 8
#define SPELL_COLL_H 8
#define SPELL_SPAWN_OFF int2fx(32)

typedef struct spell_t {
    POINT pos;
    POINT vel;
    // Collision
    int w;
    int h;
    int alive;
} spell_t;

extern spell_t spell_pool[NUM_SPELLS];

void init_spells(void);
void update_spells(void);

int spawn_spell(int player_pool, FIXED x, FIXED y, FIXED velx, FIXED vely);

INLINE int spawn_player_spell(void) {
    // Spawn a spell above the player, moving upwards
    return spawn_spell(1, player.pos.x, player.pos.y - SPELL_SPAWN_OFF, 0, PLAYER_SPELL_SPEED);
}

INLINE int spawn_enemy_spell(FIXED x, FIXED y, FIXED velx, FIXED vely) {
    return spawn_spell(0, x, y, velx, vely);
}

void end_spell(int index);

#endif
