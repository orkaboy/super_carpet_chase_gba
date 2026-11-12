#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <tonc.h>

enum cutscene_frame_type {
    CS_TYPE_IMG = 0,
    CS_TYPE_TEXT,
    CS_TYPE_WAIT,
};

#define CUTSCENE_BG_NR 3
#define CUTSCENE_BG_LAYER DCNT_BG3

enum cutscene_img_frame_type {
    CSI_TYPE_HIDE = 0,
    CSI_TYPE_SHOW,
    CSI_TYPE_MOVEX,
    CSI_TYPE_MOVEY,
    CSI_TYPE_SET_SE
};

#define CUTSCENE_IMAGE_LAYERS 8

typedef struct cs_img_frame_t {
    // Used by CS_TYPE_IMG
    int type;
    int bg;
    int start_frame;    // Used by many
    int end_frame;
    union {
        int se;         // CSI_TYPE_SET_SE
        FIXED start;    // CSI_TYPE_MOVEX/Y
    };
    FIXED speed;        // Rate of change/frame
} cs_img_frame_t;

typedef struct cs_frame_t {
    // Used by all states
    int type;
    int timer;
    // Used by CS_TYPE_TEXT
    int color;
    // Used by CS_TYPE_TEXT/CS_TYPE_IMG
    int len;
    union {
        // Used by CS_TYPE_TEXT
        char* text;
        // Used by CS_TYPE_IMG
        cs_img_frame_t* img;
    };
} cs_frame_t;

typedef struct cutscene_t {
    int len;
    int first_frame;
    int frametime;
    int charframe;
    int frame;
    int dispcnt;
    const cs_frame_t* frames;
} cutscene_t;

void init_cutscene(const cs_frame_t* frames, int len, int dispcnt);
int update_cutscene();

#endif
