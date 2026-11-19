#include "game_state.h"

#include "gfx.h"
#include "fsm.h"
#include "mode7.h"

#include "wizard.h"
#include "spell.h"

#include "sprites.h"

#include "skybox.h"
#include "desert.h"
#include "bgpal.h"

#include "carpet.h"
#include "pwizard.h"
#include "pspell.h"
#include "objpal.h"

#include <tonc.h>

FIXED game_scroll_speed = 0;
#define GAME_SCROLL_ACC 1
#define GAME_SCROLL_MAX float2fx(2.0)

FIXED game_height = 0;
#define GAME_HEIGHT_VEL float2fx(0.1)
#define GAME_HEIGHT_START int2fx(32)
#define GAME_HEIGHT_MAX int2fx(64)

wizard_t player;
wizard_t boss;

// TODO: Move elsewhere
void update_carpet(int hwsprite, int tid_offset) {
    static int anim_frame = 0;
    static int anim_timer = 0;

    if(anim_timer < PCARPET_FRAME_TIME) {
        anim_timer++;
    } else {
        anim_timer = 0;
        anim_frame = (anim_frame + 1) % PCARPET_FRAMES;
        // Update to the new frame
        obj_set_tid(hwsprite, tid_offset + anim_frame * PCARPET_TILE_PER_FRAME);
    }
}

void game_state_on_entry(void) {
    // Clear sprites
    oam_init(obj_buffer, NUM_HW_SPRITES);

    // Load palettes
    LZ77UnCompVram(bgPal, pal_bg_mem);
    LZ77UnCompVram(objPal, pal_obj_mem);

    // Load tiles
    LZ77UnCompVram(desertTiles, tile_mem[0]);
    LZ77UnCompVram(skyboxTiles, &tile_mem[0][128]); // Halfway into the page (note, the sky tile IDs are offset too)

    LZ77UnCompVram(carpetTiles, &tile_mem[4][0]); // 256x32px = 32x4 tiles = 128 tiles in
    LZ77UnCompVram(pwizardTiles, &tile_mem[4][128]); // 256x32px = 32x4 tiles = 256 tiles in
    LZ77UnCompVram(pspellTiles, &tile_mem[4][256]); // 16x16 = 2x2 tiles = 260 tiles in //TODO: How long animation?

    // Load map
    LZ77UnCompVram(desertMap, se_mem[24]);
    LZ77UnCompVram(skyboxMap, se_mem[22]);

    // Set up mode7
    m7_camera_t cam = {
        {int2fx(0), GAME_HEIGHT_START, int2fx(0)}, // pos.x,y,z
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

    // TODO: Move elsewhere?
    // TODO: Defines for hwsprite index, palette, tile
    obj_set_attr(&obj_buffer[PWIZARD_HWSPRITE], ATTR0_SQUARE, ATTR1_SIZE_32, ATTR2_PRIO(2) | ATTR2_PALBANK(1) | PWIZARD_TID);
    obj_set_pos(&obj_buffer[PWIZARD_HWSPRITE], SCREEN_WIDTH/2 - PWIZARD_W/2, SCREEN_HEIGHT - 32);

    obj_set_attr(&obj_buffer[PCARPET_HWSPRITE], ATTR0_SQUARE, ATTR1_SIZE_32, ATTR2_PRIO(2) | ATTR2_PALBANK(0) | PCARPET_TID);
    obj_set_pos(&obj_buffer[PCARPET_HWSPRITE], SCREEN_WIDTH/2 - PCARPET_W, SCREEN_HEIGHT - 32);

    // TODO: TEMP, move to spell
    obj_set_attr(&obj_buffer[PSPELL_HWSPRITE], ATTR0_SQUARE | ATTR0_HIDE, ATTR1_SIZE_16, ATTR2_PRIO(1) | ATTR2_PALBANK(4) | PSPELL_TID);
    obj_set_pos(&obj_buffer[PSPELL_HWSPRITE], SCREEN_WIDTH/2 - 8, SCREEN_HEIGHT/2 - 8);


    // Initialize state
    game_scroll_speed = 0;
    game_height = GAME_HEIGHT_START;
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
    // Jump with A
    if(key_hit(KEY_A) && !player.jumping) {
        player.jumping = 1;
        player.vel.y = PLAYER_JUMP_VEL;
    }
    player.vel.x = key_tri_horz() * PLAYER_X_SPEED;
    if(player.jumping) {
        player.vel.y += PLAYER_JUMP_GRAVITY;
    }
    // TODO: Fire with B? // TODO: TEMP
    if(key_hit(KEY_B)) {
        //TODO: Fire delay
        spawn_player_spell();
    }
    // TODO: Shield/Special with L/B?

    // Speed up game scroll
    game_scroll_speed += GAME_SCROLL_ACC;
    game_scroll_speed = min(game_scroll_speed, GAME_SCROLL_MAX);

    FIXED rise_speed = (game_height < GAME_HEIGHT_MAX) ? GAME_HEIGHT_VEL : 0;
    game_height += GAME_HEIGHT_VEL;
    game_height = min(game_height, GAME_HEIGHT_MAX);

    // Set camera rotation
    m7_rotate(&m7_level.camera, int2fx(7), 0);
    // Move camera slowly forward
    VECTOR dir = {0, rise_speed, -game_scroll_speed};
    m7_translate_level(&m7_level.camera, &dir);

    // Handle mode7 bg
    m7_prep_horizon(&m7_level);
    m7_update_sky(&m7_level);
    m7_prep_affines(&m7_level);

    update_spells();
    // TODO: Animate wizards
    update_wizard(&player);
    update_wizard(&boss);

    update_carpet(PCARPET_HWSPRITE, PCARPET_TID);

    // Bind player position
    player.pos.x = clamp(player.pos.x, PLAYER_X_MIN, PLAYER_X_MAX);
    player.pos.y = min(player.pos.y, PLAYER_Y_PLANE);
    if(player.pos.y >= PLAYER_Y_PLANE) {
        player.vel.y = 0;
        player.jumping = 0;
    }

    // Carpet should stay at same height when jumping
    obj_set_pos(&obj_buffer[PWIZARD_HWSPRITE], fx2int(player.pos.x) - PWIZARD_W/2, fx2int(player.pos.y)-PWIZARD_H/2-18);
    obj_set_pos(&obj_buffer[PCARPET_HWSPRITE], fx2int(player.pos.x) - PCARPET_W/2, fx2int(PLAYER_Y_PLANE)-PCARPET_H/2);

    return STATE_GAME;
}

void game_state_on_exit(void) {
	irq_delete(II_HBLANK);
    REG_DISPCNT = 0;
}
