#include "gfx.h"

// OAM shadow buffer
OBJ_ATTR obj_buffer[NUM_HW_SPRITES];
OBJ_AFFINE * const obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

void obj_set_tid(int hwsprite, int tid) {
    obj_buffer[hwsprite].attr2 &= ~ATTR2_ID_MASK;
    obj_buffer[hwsprite].attr2 |= tid;
}

void vblank_update_oam(void) {
    oam_copy(oam_mem, obj_buffer, NUM_HW_SPRITES);
}

// Mode7 related
m7_level_t m7_level;
