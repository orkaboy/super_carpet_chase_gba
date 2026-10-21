#include <tonc.h>
// TODO: Music
#include <maxmod.h>
//#include "soundbank.h"
//#include "soundbank_bin.h"

#include "fsm.h"

int main(void) {
	// Init interrupts and VBlank irq.
	irq_init(NULL);
	irq_add(II_VBLANK, mmVBlank);

    // TODO: Load Maxmod music
    //mmInitDefault((mm_addr)soundbank_bin, 8);
    // TODO: Start the Maxmod music on loop
    //mmStart(MOD_ARABIA_2, MM_PLAY_LOOP);

    // Set start rand seed
    sqran(0);

    init_fsm(STATE_MENU);

    while(1) {
        run_fsm();
    }
    return 1;
}
