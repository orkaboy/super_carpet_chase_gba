#include "mode7.h"

#include <limits.h>

// Initialize mode 7 structure
void m7_init(m7_level_t* level, u16 skycnt, u16 gndcnt) {
    level->bgcnt_sky = skycnt;
    level->bgcnt_gnd = gndcnt;
    level->horizon = 80; // TODO: good default? this is calculated anyways.

    REG_BG2CNT = gndcnt;
    REG_BG_AFFINE[2] = bg_aff_default;
}

// Calculate the horizon scanline
void m7_prep_horizon(m7_level_t* level) {
    int horz;
    m7_camera_t* cam = &level->camera;

    if(cam->v.y != 0) {
        horz = M7_FAR_BG * cam->w.y - cam->pos.y;
        horz = M7_TOP - Div(horz * M7_D, M7_FAR_BG * cam->v.y);
    } else {
        // Looking straight up (w.y < 0) or down (w.y > 0)
        horz = cam->w.y > 0 ? INT_MIN : INT_MAX;
    }

    level->horizon = horz;
}

void m7_update_sky(const m7_level_t* level) {
    // If sky is visible, activate mode0 for next render pass
    if(level->horizon > 0) {
        BFN_SET(REG_DISPCNT, DCNT_MODE0, DCNT_MODE);
        REG_BG2CNT = level->bgcnt_sky;
        REG_BLDALPHA = 16; // TODO:
    }

    const m7_camera_t* cam = &level->camera;
    // Shift backdrop according to camera rotation around global y-axis (yaw)
    REG_BG2HOFS = (cam->phi >> 6) + M7_LEFT;
    // -1 due to hblank irq happening after the line. line[0] happens above the screen
    REG_BG2VOFS = -m7_horizon_line(level) - 1;
}

void m7_rotate(m7_camera_t* cam, int phi, int theta) {
    // Ensure we aren't looping around when looking up/down
    theta = clamp(theta, -0x3FFF, 0x4001);
    cam->phi = phi;
    cam->theta = theta;
    // (co)sines
    FIXED cf, sf, ct, st;
    cf = lu_cos(phi)    >> 4;   sf = lu_sin(phi)    >> 4;
    ct = lu_cos(theta)  >> 4;   st = lu_sin(theta)  >> 4;
    // Camera x-axis (right)
    cam->u.x = cf;
    cam->u.y = 0; // no roll allowed
    cam->u.z = sf;
    // Camera y-axis (up)
    cam->v.x = sf * st >> 8;
    cam->v.y = ct;
    cam->v.z = -cf * st >> 8;
    // Camera z-axis (back)
    cam->w.x = -sf * ct >> 8;
    cam->w.y = st;
    cam->w.z = cf * ct >> 8;
}

void m7_translate_level(m7_camera_t* cam, const VECTOR* dir) {
    cam->pos.x += (cam->u.x * dir->x - cam->u.z * dir->z) >> 8;
    cam->pos.y += dir->y; // Don't scale the up/down movement
    cam->pos.z += (cam->u.z * dir->x + cam->u.x * dir->z) >> 8;
}
