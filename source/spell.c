#include "spell.h"
#include "sprites.h"
#include "gfx.h"

spell_t spell_pool[NUM_SPELLS];

void init_spells(void) {
    spell_t* s = spell_pool;
    for(int idx = 0; idx < NUM_SPELLS; ++idx, s++) {
        s->alive = 0;
        int hwsprite = PSPELL_HWSPRITE + idx;
        // TODO: Boss spells
        obj_set_attr(&obj_buffer[hwsprite], ATTR0_SQUARE | ATTR0_HIDE, ATTR1_SIZE_16, ATTR2_PRIO(1) | ATTR2_PALBANK(4) | PSPELL_TID);
    }
}

void update_spells(void) {
    spell_t* s = spell_pool;
    for(int idx = 0; idx < NUM_SPELLS; ++idx, s++) {
        if(!s->alive) continue;
        s->pos.x += s->vel.x;
        s->pos.y += s->vel.y;

        // TODO: Check Out of Bounds. Keep here? X OOB?
        if(s->pos.y < Y_OOB_TOP || s->pos.y >= Y_OOB_BOT) {
            end_spell(idx);
        } else {
            int hwsprite = PSPELL_HWSPRITE + idx;
            // TODO: animation + boss spells
            int tid = PSPELL_TID; // (i < NUM_E_SPELLS) ? BOSS_SPELL_TID : PLAYER_SPELL_TID;
            obj_set_tid(hwsprite, tid); //TODO: tid_offset + anim_frame * PCARPET_TILE_PER_FRAME);
            obj_set_pos(&obj_buffer[hwsprite], fx2int(s->pos.x) - PSPELL_W/2, fx2int(s->pos.y) - PSPELL_H/2);
        }
    }
}

int spawn_spell(int player_pool, FIXED x, FIXED y, FIXED velx, FIXED vely) {
    int idx = (player_pool) ? 0 : NUM_PLAYER_SPELLS;
    const int top = (player_pool) ? NUM_PLAYER_SPELLS : NUM_SPELLS;
    spell_t* s = &spell_pool[idx];
    for(; idx < top; ++idx) {
        if(s->alive == 0) {
            // This spell is dead, reuse it
            s->alive = 1;
            s->pos.x = x;
            s->pos.y = y;
            s->vel.x = velx;
            s->vel.y = vely;
            s->w = SPELL_COLL_W;
            s->h = SPELL_COLL_H;

            int hwsprite = PSPELL_HWSPRITE + idx;
            int tid = PSPELL_TID; // (i < NUM_E_SPELLS) ? BOSS_SPELL_TID : PLAYER_SPELL_TID;
            obj_unhide(&obj_buffer[hwsprite], ATTR0_REG); // TODO: Note, ATTR0_AFF
            obj_set_tid(hwsprite, tid);
            obj_set_pos(&obj_buffer[hwsprite], fx2int(s->pos.x) - PSPELL_W/2, fx2int(s->pos.y) - PSPELL_H/2);
            return 1;
        }
        s++;
    }
    return 0;
}

void end_spell(int index) {
    spell_pool[index].alive = 0;
    // Hide sprite
    obj_hide(&obj_buffer[PSPELL_HWSPRITE + index]);
}
