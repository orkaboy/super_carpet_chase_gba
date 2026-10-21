#include "gfx.h"

// OAM shadow buffer
OBJ_ATTR obj_buffer[NUM_HW_SPRITES];
OBJ_AFFINE * const obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

void vblank_update_oam(void) {
    oam_copy(oam_mem, obj_buffer, NUM_HW_SPRITES);
}

// Mode7 related
m7_level_t m7_level;
