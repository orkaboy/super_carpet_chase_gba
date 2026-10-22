#include "outro_state.h"

#include "gfx.h"
#include "fsm.h"

void outro_state_on_entry(void) {
    // Clear sprites
    oam_init(obj_buffer, NUM_HW_SPRITES);
    // TODO: Set display control register
    REG_DISPCNT = 0;
}

int outro_state_update(void) {
    // Handle input
    if(key_hit(KEY_START)) {
        // TODO: Add fate to black
        return STATE_GAME_OVER;
    }

    return STATE_OUTRO;
}

void outro_state_on_exit(void) {
    REG_DISPCNT = 0;
}
