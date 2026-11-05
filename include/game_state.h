#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <tonc.h>

#define PLAYER_X_MIN        int2fx(16)
#define PLAYER_X_MAX        int2fx(SCREEN_WIDTH - 16)
#define PLAYER_X_SPEED      float2fx(4.0)
#define PLAYER_Y_PLANE      int2fx(SCREEN_HEIGHT - 24)
#define PLAYER_JUMP_VEL     float2fx(-6.0)
#define PLAYER_JUMP_GRAVITY float2fx(0.1)

#define BOSS_Y_PLANE        int2fx(64)

void game_state_on_entry(void);
int game_state_update(void);
void game_state_on_exit(void);

#endif
