#include "menu_state.h"

#include "fsm.h"
#include "gfx.h"

#include <tonc.h>

void menu_state_on_entry(void) {
    oam_init(obj_buffer, NUM_HW_SPRITES);

}

int menu_state_update(void) {
    // Handle input


    return STATE_MENU;
}
