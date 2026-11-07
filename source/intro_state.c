#include "intro_state.h"

#include "cutscene.h"
#include "gfx.h"
#include "fsm.h"

// TODO: Story
#define NUM_INTRO_FRAMES 3
const cs_frame_t _intro_cs[NUM_INTRO_FRAMES] = {
    {.type = CS_TYPE_TEXT, .text = "The evil wizard stole the carpet!", .len = 33, .timer = 1},
    {.type = CS_TYPE_TEXT, .text = "It is time for wizard battle!", .len = 29, .timer = 1},
    {.type = CS_TYPE_TEXT, .text = "START!", .len = 6, .timer = 1}
};

void intro_state_on_entry(void) {
    // Clear sprites
    oam_init(obj_buffer, NUM_HW_SPRITES);

    tte_init_chr4c(
        3, // BG3
        BG_CBB(0) | BG_SBB(31),
        0xF000, // Screen-entry base, sub-palette 15
        bytes2word(13, 15, 0, 0), // cattrs(INK, SHADOW)
        CLR_WHITE,
        NULL, // Font
        (fnDrawg)chr4c_drawg_b1cts_fast
    );

    pal_bg_mem[0] = CLR_BLACK;

    REG_BG3HOFS = -32;
    REG_BG3VOFS = -128;

    init_cutscene(_intro_cs, NUM_INTRO_FRAMES);

    // TODO: Set display control register
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG3;
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
