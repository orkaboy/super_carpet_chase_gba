#include "fsm.h"

#include "menu_state.h"
#include "gfx.h"

#include <maxmod.h>

int current_state;

fsm_t states[] = {
    {.update = menu_state_update, .vblank = vblank_update_oam, .on_entry = menu_state_on_entry, .on_exit = menu_state_on_exit},
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
