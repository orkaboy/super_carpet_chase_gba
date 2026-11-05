#include "wizard.h"

#include "gfx.h"

void init_wizard(wizard_t* wizard) {
    wizard->pos.x = int2fx(SCREEN_WIDTH/2);
    wizard->pos.y = 0;
    wizard->vel.x = 0;
    wizard->vel.y = 0;
    wizard->jumping = 0;
    wizard->hp = 0; // TODO:
}

void update_wizard(wizard_t* wizard) {
    wizard->pos.x += wizard->vel.x;
    wizard->pos.y += wizard->vel.y;
}
