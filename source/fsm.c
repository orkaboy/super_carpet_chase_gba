#include "fsm.h"

// States
#include "menu_state.h"
#include "intro_state.h"
#include "game_state.h"
#include "outro_state.h"
#include "game_over_state.h"

#include "gfx.h"

#include <maxmod.h>

int current_state;

fsm_t states[] = {
    /* STATE_MENU       */ {.update = menu_state_update, .vblank = vblank_update_oam, .on_entry = menu_state_on_entry, .on_exit = menu_state_on_exit},
    /* STATE_INTRO      */ {.update = intro_state_update, .vblank = vblank_update_oam, .on_entry = intro_state_on_entry, .on_exit = intro_state_on_exit},
    /* STATE_GAME       */ {.update = game_state_update, .vblank = vblank_update_oam, .on_entry = game_state_on_entry, .on_exit = game_state_on_exit},
    /* STATE_OUTRO      */ {.update = outro_state_update, .vblank = vblank_update_oam, .on_entry = outro_state_on_entry, .on_exit = outro_state_on_exit},
    /* STATE_GAME_OVER  */ {.update = game_over_state_update, .vblank = vblank_update_oam, .on_entry = game_over_state_on_entry, .on_exit = game_over_state_on_exit},
};

void init_fsm(int state) {
    current_state = state;
    if(states[state].on_entry) states[state].on_entry();
}

void run_fsm(void) {
    // Get new key input
    key_poll();

    // Check for reset (Se+St+L+R)
    if(KEY_EQ(key_held, KEY_RESET)) {
        SoftReset();
    }

    // Handle logic for the current state
    int next_state = states[current_state].update();
    // Do we need to change state?
    if(next_state != current_state) {
        // If so, handle tear-down/setup of states
        if(states[current_state].on_exit) states[current_state].on_exit();
        if(states[next_state].on_entry) states[next_state].on_entry();
        current_state = next_state;
        return;
    }

    // Wait for VBlank
    VBlankIntrWait();

    // Maxmod, update music
    mmFrame();

    // Post-VBlank update (OAM etc.)
    if(states[current_state].vblank) {
        states[current_state].vblank();
    }
}
