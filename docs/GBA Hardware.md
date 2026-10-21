Note, a lot of information can be found at [GBATEK](https://problemkaputt.de/gbatek.htm).

The GBA is, roughly, equivalent in power to a SNES.
# CPU
32-bit ARM RISC CPU @16.78MHz
Two different type of instructions:
* 32-bit ARM instructions (powerful, but slow to execute from ROM. If needed, move to `IWRAM` first).
* 16-bit Thumb instructions (subset of ARM, takes up less space, recommended).
# Memory

| Area       | Start       | End         | Size       | Width  | Notes                                                                                   |
| ---------- | ----------- | ----------- | ---------- | ------ | --------------------------------------------------------------------------------------- |
| System ROM | 0x0000_0000 | 0x0000_03FF | 16KB       | 32-bit | Bios memory. Execute only (can't read).                                                 |
| EWRAM      | 0x0200_0000 | 0x0203_FFFF | 256KB      | 16-bit | External Work RAM (on-board, so slower). Prefer Thumb instructions when used.           |
| IWRAM      | 0x0300_0000 | 0x0300_7FFF | 32KB       | 32-bit | Internal Work RAM (on-chip).                                                            |
| IO RAM     | 0x0400_0000 | 0x0400_03FF | 1KB        | 32-bit | Memory mapped IO registers for controlling graphics, sound, etc.                        |
| PAL RAM    | 0x0500_0000 | 0x0500_03FF | 1KB        | 16-bit | Palette memory. 2 palettes (1 for sprites, 1 for BGs) of 256 entries, 15-bits each.     |
| VRAM       | 0x0600_0000 | 0x0601_7FFF | 96KB       | 16-bit | Video RAM. Sprites and BGs are stored here. How they are interpreted depends on `mode`. |
| OAM        | 0x0700_0000 | 0x0700_03FF | 1KB        | 32-bit | Object Attribute Memory. Control registers for sprites.                                 |
| PAK ROM    | 0x0800_0000 |             | Up to 32MB | 16-bit | Cartridge data.                                                                         |
| Cart RAM   | 0x0E00_0000 |             | Up to 64KB | 8-bit  | SRAM/Flash/EEPROM for Cartridge save data.                                              |
# Graphics
240x160px LCD with 32768 colors (15-bits)
3 bitmap modes, 3 tilemap modes, and sprites.
4 tilemap layers (backgrounds).
128 sprite objects (8x8 to 64x64 in size).
Affine transformations (rotate/scale/shear) on 2 bgs and 32 sprites. Matrix math.
Special graphics effects (mosaic, additive blend, fade to white/black).
# Sound
6 sound channels:
* 4 tone generators (the Game Boy ones: 2 square, 1 wave, 1 noise).
* 2 `DirectSound` channels for playing samples and music. 
# Input
10 buttons:
* 4-way DPad
* Select/Start
* A/B
* Shoulder buttons L/R
Also has 4-player multiplayer with link cable.
Also some weird sensors like infrared, solar and gyroscopic (add-ins?).
# Restrictions
* No floating points.
* No hardware division.
* `i32/u32` is the preferred data type (the `int` type is 32-bit). Do not use `u8/u16`, especially for loop indices, it can actually slow things down! Note that `u8/u16` should be used when accessing certain memory areas due to the bus width (for example, the `VRAM` is 16-bit).
	* Even if this is the case, it's often more optimal to use `u32` when performing copies into `VRAM`! See [this](https://gbadev.net/tonc/bitmaps.html#ssec-data-memcpy).
* global variables are not optimal.
* Try to avoid branching (`if/loop/switch`) if possible. Arithmetic is faster.
* Some tips and tricks for [optimization](https://web.archive.org/web/20090719211007/http://www.abarnett.demon.co.uk:80/tutorial.html). But also, don't optimize too early.
