#include <tonc.h>

#include "fsm.h"

int main(void) {
	// Init interrupts and VBlank irq.
	irq_init(NULL);
    // TODO: Add Maxmod IRQ handler
	irq_add(II_VBLANK, NULL);
    // TODO: Add HBlank IRQ

    // Set start rand seed
    sqran(0);

    init_fsm(STATE_MENU);

    while(1) {
        run_fsm();
    }
    return 1;
}
