#include <tonc.h>

#include "gfx.h"

int main(void) {
	// Init interrupts and VBlank irq.
	irq_init(NULL);
    // TODO: Add Maxmod IRQ handler
	irq_add(II_VBLANK, NULL);
    // TODO: Add HBlank IRQ

    // Set start rand seed
    sqran(0);

    init_gfx();

    while(1) {
        // Update game logic
        // Update graphics
        update_gfx();
        // Wait for VBlank
        VBlankIntrWait();
        // TODO: Maxmod update
        // Copy OAM shadow buffer etc.
        update_vblank();
    }
    return 1;
}
