#include "cutscene.h"

static cutscene_t _cs;

void init_cutscene(const cs_frame_t* frames, int len) {
    _cs.charframe = 0;
    _cs.frametime = 0;
    _cs.frame = 0;
    _cs.len = len;
    _cs.frames = frames;
    
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

    REG_BG3HOFS = -32;
    REG_BG3VOFS = -128;

    // Clear the text layer
    tte_set_pos(0,0);
    tte_erase_screen();
}

int update_cutscene() {
    const cs_frame_t* cur_frame = &_cs.frames[_cs.frame];

    switch(cur_frame->type) {
        case CS_TYPE_IMG: {
            _cs.frametime++;
            if(_cs.frametime >= cur_frame->timer) {
                _cs.frame++;
                _cs.frametime = 0;
            }
            // TODO: Move/show image layer
            break;
        }
        case CS_TYPE_TEXT: {
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
                    _cs.frame++;
                    _cs.frametime = 0;
                    _cs.charframe = 0;
                    // Clear the text layer
                    tte_set_pos(0,0);
                    tte_erase_screen();
                }
            }
            break;
        }
        case CS_TYPE_WAIT: {
            _cs.frametime++;
            if(_cs.frametime >= cur_frame->timer) {
                _cs.frame++;
                _cs.frametime = 0;
            }
            break;
        }
        // Fallback
        default:
            _cs.frame++;
            _cs.frametime = 0;
            _cs.charframe = 0;
            break;
    }

    // Check if we are done
    return (_cs.frame >= _cs.len);
}
