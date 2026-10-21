#ifndef MODE7_H
#define MODE7_H

#include <tonc.h>

#define M7_D            256                 // Focal length
#define M7_D_SHIFT      8                   // Focal shift
#define M7O_NORM        2                   // Object renormalization shift (by /4)

// View frustrum limits
#define M7_LEFT         (-SCREEN_WIDTH/2)   // Viewport left
#define M7_RIGHT        (SCREEN_WIDTH/2)    // Viewport right
#define M7_TOP          (SCREEN_HEIGHT/2)   // Viewport top (y-axis up)
#define M7_BOTTOM       (-SCREEN_HEIGHT/2)  // Viewport bottom (y-axis up!)
#define M7_NEAR         24                  // Near plane (objects)
#define M7_FAR          512                 // Far plane (objects)

#define M7_FAR_BG       768                 // Far plane (floor)

// Right-handed coordinate system camera for mode 7
typedef struct m7_camera_t {
    VECTOR pos; // World position
    int theta;  // Polar angle (pitch)
    int phi;    // Azimuth angle (yaw)
    VECTOR u;   // Local x-axis (right)
    VECTOR v;   // Local y-axis (up)
    VECTOR w;   // Local z-axis (back)
} m7_camera_t;

// Mode 7 coordinator
typedef struct m7_level_t {
    m7_camera_t     camera;                 // Camera
    BG_AFFINE       bgaff[SCREEN_HEIGHT+1]; // Affine parameter array
    // TODO: Sprites
    int             horizon;    // Horizon scanline
    u16             bgcnt_sky;  // BG2CNT for sky (mode0)
    u16             bgcnt_gnd;  // BG2CNT for ground (mode1)
} m7_level_t;

void m7_init(m7_level_t* level, u16 skycnt, u16 gndcnt);
void m7_prep_horizon(m7_level_t* level);
void m7_update_sky(const m7_level_t* level);

// m7 math functions
void m7_rotate(m7_camera_t* cam, int theta, int phi);
void m7_translate_level(m7_camera_t* cam, const VECTOR* dir);

// IWRAM functions (ARM instructions)
IWRAM_CODE void m7_prep_affines(m7_level_t *level);
IWRAM_CODE void m7_hbl_gnd(void);

// Get the scanline on which the horizon lies
INLINE int m7_horizon_line(const m7_level_t *level) {
    return clamp(level->horizon, 0, 228);
}

#endif
