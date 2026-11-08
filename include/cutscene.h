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

typedef struct cs_frame_t {
    int type;
    int timer;
    int len;
    char* text;
    int bg;
    int color;
} cs_frame_t;

typedef struct cutscene_t {
    int len;
    int frametime;
    int charframe;
    int frame;
    const cs_frame_t* frames;
} cutscene_t;

void init_cutscene(const cs_frame_t* frames, int len);
int update_cutscene();

#endif
