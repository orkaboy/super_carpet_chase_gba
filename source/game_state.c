#include "game_state.h"

#include "gfx.h"
#include "fsm.h"
#include "mode7.h"

#include "wizard.h"
#include "spell.h"

#include "skybox.h"
#include "desert.h"
#include "bgpal.h"

#include <tonc.h>

FIXED game_scroll_speed = 0;
#define GAME_SCROLL_ACC 1
#define GAME_SCROLL_MAX float2fx(1.0)

wizard_t player;
wizard_t boss;

void game_state_on_entry(void) {
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

    // Set up mode7
    m7_camera_t cam = {
        {int2fx(0), int2fx(32), int2fx(0)}, // pos.x,y,z
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

    // Initialize state
    game_scroll_speed = 0;
    init_spells();
    init_wizard(&player);
    init_wizard(&boss);
    player.pos.y = PLAYER_Y_PLANE;
    boss.pos.y = BOSS_Y_PLANE;

    // TODO: Set display control register
    REG_DISPCNT = DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
    // For mode7
	irq_add(II_HBLANK, (fnptr)m7_hbl_gnd);
}

int game_state_update(void) {
    // Handle input
    if(key_hit(KEY_START)) {
        // TODO: Add fade to black
        return STATE_OUTRO;
    }
    if(key_hit(KEY_UP) && !player.jumping) {
        player.jumping = 1;
        player.vel.y = PLAYER_JUMP_VEL;
    }
    player.vel.x = key_tri_horz() * PLAYER_X_SPEED;
    if(player.jumping) {
        player.vel.y += PLAYER_JUMP_GRAVITY;
    }

    // Speed up game scroll
    game_scroll_speed += GAME_SCROLL_ACC;
    game_scroll_speed = clamp(game_scroll_speed, 0, GAME_SCROLL_MAX);

    // Set camera rotation
    m7_rotate(&m7_level.camera, int2fx(7), 0);
    // Move camera slowly forward
    VECTOR dir = {0, 0, -game_scroll_speed};
    m7_translate_level(&m7_level.camera, &dir);

    // Handle mode7 bg
    m7_prep_horizon(&m7_level);
    m7_update_sky(&m7_level);
    m7_prep_affines(&m7_level);

    update_spells();
    update_wizard(&player);
    update_wizard(&boss);

    // Bind player position
    player.pos.x = clamp(player.pos.x, PLAYER_X_MIN, PLAYER_X_MAX);
    player.pos.y = min(player.pos.y, PLAYER_Y_PLANE);
    if(player.pos.y >= PLAYER_Y_PLANE) {
        player.vel.y = 0;
        player.jumping = 0;
    }

    return STATE_GAME;
}

void game_state_on_exit(void) {
	irq_delete(II_HBLANK);
    REG_DISPCNT = 0;
}
