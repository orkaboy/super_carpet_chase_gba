#ifndef FSM_H
#define FSM_H

#include <tonc.h>

#define STATE_MENU 0
#define STATE_INTRO 1
#define STATE_GAME 2
#define STATE_OUTRO 3
#define STATE_GAME_OVER 4

typedef int (*fptr_ret_t)(void);
typedef void (*fptr_t)(void);

typedef struct fsm_t {
    fptr_t on_entry;
    fptr_ret_t update;
    fptr_t vblank;
    fptr_t on_exit;
} fsm_t;

void init_fsm(int state);
void run_fsm(void);

#endif
