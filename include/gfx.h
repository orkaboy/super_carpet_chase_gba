#ifndef GFX_H
#define GFX_H

#include <tonc.h>
#include "mode7.h"

#define NUM_HW_SPRITES 128
// OAM shadow buffer
extern OBJ_ATTR obj_buffer[NUM_HW_SPRITES];
extern OBJ_AFFINE * const obj_aff_buffer;

void vblank_update_oam(void);

// Mode7 related
extern m7_level_t m7_level;

#endif
