#include "cutscene.h"

static cutscene_t _cs;

static FIXED _img_val[CUTSCENE_IMAGE_LAYERS];        // Scratch value

static inline void _reset_cursor() {
    // Clear the text layer
    tte_set_pos(32, 128);
    tte_erase_screen();
}

static inline void _next_frame() {
    _cs.frame++;
    _cs.frametime = 0;
    _cs.charframe = 0;
    _cs.first_frame = 1;
}

static inline int _first_frame() {
    return _cs.first_frame;
}

void init_cutscene(const cs_frame_t* frames, int len, int dispcnt) {
    _cs.charframe = 0;
    _cs.frametime = 0;
    _cs.first_frame = 1;
    _cs.frame = 0;
    _cs.len = len;
    _cs.frames = frames;
    _cs.dispcnt = dispcnt;

    // Set display control register
    REG_DISPCNT = dispcnt;

    tte_init_chr4c(
        CUTSCENE_BG_NR,
        BG_CBB(0) | BG_SBB(31),
        0xF000, // Screen-entry base, sub-palette 15
        bytes2word(15, 0, 0, 0), // cattrs(INK, SHADOW)
        CLR_WHITE,
        NULL, // Font
        (fnDrawg)chr4c_drawg_b1cts_fast
    );

    pal_bg_mem[0] = CLR_BLACK;

    REG_BG_OFS[CUTSCENE_BG_NR].x = 0;
    REG_BG_OFS[CUTSCENE_BG_NR].y = 0;

    _reset_cursor();
}

int update_cutscene() {
    const cs_frame_t* cur_frame = &_cs.frames[_cs.frame];

    switch(cur_frame->type) {
        case CS_TYPE_IMG: {
            if(_cs.frametime >= cur_frame->timer) {
                _next_frame();
            } else {
                const cs_img_frame_t* cur_img = cur_frame->img;
                for(int i = 0; i < cur_frame->len; ++i, cur_img++) {
                    if(_cs.frametime < cur_img->start_frame) {
                        continue;
                    } else if(_cs.frametime > cur_img->end_frame) {
                        continue;
                    }
                    // First frame
                    if(_cs.frametime == cur_img->start_frame) {
                        _img_val[i] = cur_img->start;
                    } else {
                        _img_val[i] += cur_img->speed;
                    }
                    // Update
                    switch(cur_img->type) {
                        // TODO: Fade in/out with blend
                        case CSI_TYPE_HIDE:
                            if(_cs.frametime == cur_img->end_frame) {
                                _cs.dispcnt &= ~DCNT_LAYER(cur_img->bg);
                            }
                            break;
                        case CSI_TYPE_SHOW:
                            if(_cs.frametime == cur_img->end_frame) {
                                _cs.dispcnt |= DCNT_LAYER(cur_img->bg);
                            }
                            break;
                        case CSI_TYPE_MOVEX:
                            REG_BG_OFS[cur_img->bg].x = fx2int(_img_val[i]);
                            break;
                        case CSI_TYPE_MOVEY:
                            REG_BG_OFS[cur_img->bg].y = fx2int(_img_val[i]);
                            break;
                        case CSI_TYPE_SET_SE:
                            // Change base image on BG (equivalent to swapping sprite frame)
                            REG_BGCNT[cur_img->bg] &= ~BG_SBB_MASK;
                            REG_BGCNT[cur_img->bg] |= BG_SBB(cur_img->se);
                            break;
                        default: break;
                    }
                }
            }
            _cs.frametime++;
            break;
        }
        case CS_TYPE_TEXT: {
            if(_first_frame()) {
                _cs.first_frame = 0;
                // Clear the text layer
                _reset_cursor();
            }
            tte_set_color(TTE_INK, cur_frame->color);
            // Check for last char
            if(_cs.charframe < cur_frame->len) {
                _cs.frametime++;
                // Can speed up text by holding A
                if(key_held(KEY_A)) {
                    _cs.frametime++;
                }
                // Draw next char
                if(_cs.frametime >= cur_frame->timer) {
                    _cs.frametime = 0;
                    // Draw next char
                    tte_putc(cur_frame->text[_cs.charframe]);
                    _cs.charframe++;
                }
            } else {
                // Advance text
                if(key_hit(KEY_A)) {
                    _next_frame();
                }
            }
            break;
        }
        case CS_TYPE_WAIT: {
            _cs.frametime++;
            if(_cs.frametime >= cur_frame->timer) {
                _next_frame();
            }
            break;
        }
        // Fallback
        default:
            _next_frame();
            break;
    }

    // Check if we are done
    return (_cs.frame >= _cs.len);
}
