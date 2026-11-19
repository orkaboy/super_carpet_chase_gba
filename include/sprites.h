#ifndef SPRITES_H
#define SPRITES_H

// TODO: Assign correct sprite ranges

#define PWIZARD_HWSPRITE        0
#define PWIZARD_TID             128
#define PWIZARD_FRAMES          1 // TODO
#define PWIZARD_FRAME_TIME      8
#define PWIZARD_W               32
#define PWIZARD_H               32
#define PWIZARD_TILE_PER_FRAME  (PWIZARD_W / 8) * (PWIZARD_H / 8)

#define PCARPET_HWSPRITE        1
#define PCARPET_TID             0
#define PCARPET_FRAMES          8
#define PCARPET_FRAME_TIME      8
#define PCARPET_W               32
#define PCARPET_H               32
#define PCARPET_TILE_PER_FRAME  (PCARPET_W / 8) * (PCARPET_H / 8)

#define PSPELL_HWSPRITE         2
#define PSPELL_TID              256 // TODO
#define PSPELL_FRAMES           8   // TODO
#define PSPELL_FRAME_TIME       8   // TODO
#define PSPELL_W                16
#define PSPELL_H                16
#define PSPELL_TILE_PER_FRAME   (PSPELL_W / 8) * (PSPELL_H / 8)

#endif
