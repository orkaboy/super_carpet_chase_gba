#ifndef GFX_H
#define GFX_H

#include <tonc.h>
#include "mode7.h"

#define Y_OOB_TOP int2fx(-32)
#define Y_OOB_BOT int2fx(SCREEN_HEIGHT+32)
#define X_OOB_LEFT int2fx(-32)
#define X_OOB_RIGHT int2fx(SCREEN_WIDTH+32)

#define NUM_HW_SPRITES 128
// OAM shadow buffer
extern OBJ_ATTR obj_buffer[NUM_HW_SPRITES];
extern OBJ_AFFINE * const obj_aff_buffer;

void obj_set_tid(int hwsprite, int tid);

void vblank_update_oam(void);

// Mode7 related
extern m7_level_t m7_level;

#endif
