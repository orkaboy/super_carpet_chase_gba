#include "menu_state.h"

#include "fsm.h"
#include "gfx.h"
#include "mode7.h"

#include "desert.h"
#include "skybox.h"
#include "title.h"
#include "bgpal.h"

#include <tonc.h>

#define MENU_SCROLL_SPEED float2fx(0.05)
#define TITLE_WAVE_ACCY float2fx(0.01)
#define TITLE_WAVE_VEL_PEAK float2fx(0.3)

#define CENTER_X (SCREEN_WIDTH/2)
#define CENTER_Y (SCREEN_HEIGHT/2)
#define SPRITE_HSIZE (64/2)

// Title wave motion
FIXED title_y = 0;
FIXED title_vely = 0;
FIXED title_accy = 0;

void _set_title_pos(void) {
    obj_set_pos(&obj_buffer[0], CENTER_X - 3*SPRITE_HSIZE, fx2int(title_y) + CENTER_Y - 2*SPRITE_HSIZE);
    obj_set_pos(&obj_buffer[1], CENTER_X - 1*SPRITE_HSIZE, fx2int(title_y) + CENTER_Y - 2*SPRITE_HSIZE);
    obj_set_pos(&obj_buffer[2], CENTER_X + 1*SPRITE_HSIZE, fx2int(title_y) + CENTER_Y - 2*SPRITE_HSIZE);
    obj_set_pos(&obj_buffer[3], CENTER_X - 3*SPRITE_HSIZE, fx2int(title_y) + CENTER_Y);
    obj_set_pos(&obj_buffer[4], CENTER_X - 1*SPRITE_HSIZE, fx2int(title_y) + CENTER_Y);
    obj_set_pos(&obj_buffer[5], CENTER_X + 1*SPRITE_HSIZE, fx2int(title_y) + CENTER_Y);
}

void _update_title_wave(void) {
    title_vely += title_accy;
    title_y += title_vely;
    if(title_vely > TITLE_WAVE_VEL_PEAK) {
        title_accy = -TITLE_WAVE_ACCY;
    } else if(title_vely < -TITLE_WAVE_VEL_PEAK) {
        title_accy = TITLE_WAVE_ACCY;
    }
    // Update the sprite positions
    _set_title_pos();
}

void menu_init_sprites(void) {
    // Setup sprites
    for(int i = 0; i < 6; ++i) {
        obj_set_attr(&obj_buffer[i], ATTR0_SQUARE, ATTR1_SIZE_64, ATTR2_PALBANK(0) | (64 * i));
    }
    // Temp position
    title_accy = -TITLE_WAVE_ACCY;
    _set_title_pos();
}

void menu_state_on_entry(void) {
    // Title wave motion
    title_y = 0;
    title_vely = 0;
    title_accy = 0;

    // Clear sprites
    oam_init(obj_buffer, NUM_HW_SPRITES);

    // Load palettes
    LZ77UnCompVram(bgPal, pal_bg_mem);

    // Load tiles
    LZ77UnCompVram(desertTiles, tile_mem[0]);
    LZ77UnCompVram(skyboxTiles, &tile_mem[0][128]); // Halfway into the page (note, the sky tile IDs are offset too)

    // Load map
    LZ77UnCompVram(desertMap, se_mem[24]);
    LZ77UnCompVram(skyboxMap, se_mem[22]);

    // Setup sprite tiles
    memcpy16(&pal_obj_mem[0], titlePal, titlePalLen / sizeof(u16));
    memcpy32(tile_mem[4], titleTiles, titleTilesLen / sizeof(u32));

    menu_init_sprites();

    // Set up mode7
    m7_camera_t cam = {
        {int2fx(0), int2fx(16), int2fx(0)}, // pos.x,y,z
        0, 0,                               // pitch, yaw
        // u, v, w, unit vectors
        { int2fx(1), 0, 0 }, {0, int2fx(1), 0}, {0, 0, int2fx(1)}
    };
    m7_level.camera = cam;
    m7_init(&m7_level,
        // BG2CNT for sky (reg)
        BG_CBB(0) | BG_SBB(22) | BG_REG_32x32 | BG_PRIO(3),
        // BG2CNT for ground (aff)
        BG_CBB(0) | BG_SBB(24) | BG_AFF_32x32 | BG_WRAP | BG_PRIO(3)
    );
    // Initialize fog (alpha blend to BG transparency color)
    REG_BLDCNT = BLD_BUILD(BLD_BG2, BLD_BACKDROP, 1);
    pal_bg_mem[0] = CLR_SKYBLUE;

    REG_DISPCNT = DCNT_MODE0 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
    // For mode7
	irq_add(II_HBLANK, (fnptr)m7_hbl_gnd);
}

int menu_state_update(void) {
    // Handle input
    if(key_hit(KEY_START)) {
        // TODO: Add fate to black
        return STATE_INTRO;
    }

    // Set camera rotation
    m7_rotate(&m7_level.camera, int2fx(7), 0);
    // Move camera slowly forward
    VECTOR dir = {0, 0, -MENU_SCROLL_SPEED};
    m7_translate_level(&m7_level.camera, &dir);

    // Handle mode7 bg
    m7_prep_horizon(&m7_level);
    m7_update_sky(&m7_level);
    m7_prep_affines(&m7_level);

    // Move the title up and down
    _update_title_wave();

    return STATE_MENU;
}

void menu_state_on_exit(void) {
	irq_delete(II_HBLANK);
    REG_DISPCNT = 0;
}
