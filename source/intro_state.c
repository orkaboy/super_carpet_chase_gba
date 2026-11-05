#include "intro_state.h"

#include "gfx.h"
#include "fsm.h"

void intro_state_on_entry(void) {
    // Clear sprites
    oam_init(obj_buffer, NUM_HW_SPRITES);

    // Initialize text engine
    tte_init_se(
        0, // BG0
        BG_CBB(0) | BG_SBB(31),
        0, // ???
        CLR_BLUE,
        0, // BUPOFS
        NULL, // Font
        NULL // drawg
    );

    // Write text
    tte_set_pos(10, 10);
    tte_write("Wizard battle!");

    // TODO: Set display control register
    REG_DISPCNT = DCNT_BG0;
}

int intro_state_update(void) {
    // Handle input
    if(key_hit(KEY_START)) {
        // TODO: Add fade to black
        return STATE_GAME;
    }

    return STATE_INTRO;
}

void intro_state_on_exit(void) {
    REG_DISPCNT = 0;
}
