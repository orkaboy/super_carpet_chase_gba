#include "intro_state.h"

#include "cutscene.h"
#include "gfx.h"
#include "fsm.h"

// TODO: Story
#define NUM_INTRO_FRAMES 3
const cs_frame_t _intro_cs[NUM_INTRO_FRAMES] = {
    {.type = CS_TYPE_TEXT, .text = "The evil wizard stole the carpet!", .len = 33, .timer = 1, .color = CLR_WHITE},
    {.type = CS_TYPE_TEXT, .text = "It is time for wizard battle!", .len = 29, .timer = 1, .color = CLR_GREEN},
    {.type = CS_TYPE_TEXT, .text = "START!", .len = 6, .timer = 1, .color = CLR_RED}
};

void intro_state_on_entry(void) {
    // Clear sprites
    oam_init(obj_buffer, NUM_HW_SPRITES);

    init_cutscene(_intro_cs, NUM_INTRO_FRAMES, DCNT_MODE0 | CUTSCENE_BG_LAYER);
}

int intro_state_update(void) {

    if(update_cutscene()) {
        return STATE_GAME;
    }

    return STATE_INTRO;
}

void intro_state_on_exit(void) {
    REG_DISPCNT = 0;
}
