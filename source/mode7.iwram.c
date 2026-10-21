// Mode7 related instructions executing from IWRAM (32-bit ARM instruction set)

#include <tonc.h>
#include "mode7.h"
#include "gfx.h"

// HBlank interrupt handler
IWRAM_CODE void m7_hbl_gnd(void) {
    int vcount = REG_VCOUNT; // Which scanline are we on
    int horz = m7_level.horizon;

    // If we are above the horizon, no mode 7 calculations are performed
    if(!IN_RANGE(vcount, horz, SCREEN_HEIGHT)) {
        return;
    }

    if(vcount == horz) {
        // Switch over from mode0 to mode1 at the horizon
        BFN_SET(REG_DISPCNT, DCNT_MODE1, DCNT_MODE);
        REG_BG2CNT = m7_level.bgcnt_gnd;
    }

    // Apply the affine matrix for this scanline to BG2
    BG_AFFINE* bga = &m7_level.bgaff[vcount+1];
    REG_BG_AFFINE[2] = *bga;

    // Perform distance fogging using alpha blending
    u32 ey = bga->pb * 6 >> 12;
    if(ey > 16) ey = 16;
    REG_BLDALPHA = BLDA_BUILD(16-ey, ey);
}

IWRAM_CODE void m7_prep_affines(m7_level_t *level) {
    // We are looking up at the sky, mode7 bg not displayed
    if(level->horizon >= SCREEN_HEIGHT) return;

    const int ii0 = max(level->horizon, 0);
    BG_AFFINE* bga = &level->bgaff[ii0];

    m7_camera_t* cam = &level->camera;
    FIXED xc = cam->pos.x;
    FIXED yc = cam->pos.y;
    FIXED zc = cam->pos.z;

    FIXED yb, zb;           // b' = Rx(theta) * (L, ys, -D)
    FIXED cf, sf, ct, st;   // sines/cosines
    FIXED lam, lcf, lsf;    // scale and scaled (co)sine(phi)
    cf = cam->u.x;  sf = cam->u.z;
    ct = cam->v.y;  st = cam->w.y;

    for(int i = ii0; i < SCREEN_HEIGHT; ++i) {
        yb = (i - M7_TOP) * ct + M7_D * st;
        lam = DivSafe(yc << 12, yb);    // .12f

        lcf = lam * cf >> 8;            // .12f
        lsf = lam * sf >> 8;            // .12f

        bga->pa = lcf >> 4;             // .8f
        bga->pc = lsf >> 4;             // .8f

        // lambda * Rx * b
        zb = (i - M7_TOP) * st - M7_D * ct; // .8f
        // Set the displacement
        bga->dx = xc + (lcf >> 4) * M7_LEFT - (lsf * zb >> 12); // .8f
        bga->dy = zc + (lsf >> 4) * M7_LEFT + (lcf * zb >> 12); // .8f

        bga->pb = lam; // Note, fog parameter, used in m7_hbl_gnd
        bga++;
    }
    // Prep for next render, so the first line is set correctly
    level->bgaff[SCREEN_HEIGHT] = level->bgaff[0];
}
