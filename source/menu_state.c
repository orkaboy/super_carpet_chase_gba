#include "menu_state.h"

#include "fsm.h"
#include "gfx.h"
#include "mode7.h"

#include <tonc.h>

#define MENU_SCROLL_SPEED float2fx(0.05)

void menu_state_on_entry(void) {
    // Clear sprites
    oam_init(obj_buffer, NUM_HW_SPRITES);

    // TODO: Load tiles

    // TODO: Load palettes

    // TODO: Setup sprites

    // Set up mode7
    m7_camera_t cam = {
        {int2fx(16), int2fx(16), int2fx(16)},   // pos.x,y,z
        int2fx(10), int2fx(0),                  // theta, phi
        // u, v, w, unit vectors
        { int2fx(1), 0, 0 }, {0, int2fx(1), 0}, {0, 0, int2fx(1)}
    };
    m7_level.camera = cam;
    m7_init(&m7_level,
        // BG2CNT for sky (reg)     // TODO: CBB/SBB
        BG_CBB(0) | BG_SBB(22) | BG_REG_32x32 | BG_PRIO(3),
        // BG2CNT for ground (aff)  // TODO: CBB/SBB
        BG_CBB(0) | BG_SBB(24) | BG_AFF_32x32 | BG_WRAP | BG_PRIO(3)
    );
    // Initialize fog (alpha blend to BG transparency color)
    REG_BLDCNT = BLD_BUILD(BLD_BG2, BLD_BACKDROP, 1);
    pal_bg_mem[0] = CLR_SKYBLUE;

    // For mode7
	irq_add(II_HBLANK, (fnptr)m7_hbl_gnd);
}

int menu_state_update(void) {
    // Handle input

    // Move camera slowly forward
    VECTOR dir = {0, 0, MENU_SCROLL_SPEED};
    m7_translate_level(&m7_level.camera, &dir);

    // Handle mode7 bg
    m7_prep_horizon(&m7_level);
    m7_update_sky(&m7_level);

    return STATE_MENU;
}

void menu_state_on_exit(void) {
	irq_delete(II_HBLANK);
}
