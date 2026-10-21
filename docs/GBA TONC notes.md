Notes while working with the [TONC tutorials](https://gbadev.net/tonc/).

# Graphic modes
240x160px screen.
68px hblank and vblank periods.
Rendering is at 59.73Hz.

4 background layers.
1 sprite layer with 128 sprites.
2+1 Window layers for masking (example, lamp in Zelda:LTTP).

Colors are stored in 16-bit memory slots, in the format (MSB to LSB):
```
x bbbbb ggggg rrrrr
```
Depending on graphics mode, this data can be either interpreted as raw pixels, or indices into a palette (there are two on the GBA, one with 256 colors for backgrounds and another with 256 colors for sprites). Palettes can also be subdivided into 16 palettes of 16 colors each, with color 0 being transparent.

Bitmap modes render individual pixels. Tiled graphics mode use 8x8 tiles like on the Game Boy and similar platforms. Prefer using tiled modes where possible.

Sprite graphics are up to 128 objects that move independently on the screen, each a square of 8x8 up to 64x64 pixels.

The modes:

| Mode | Type   | Width | Height | bpp | size       | page-flip | Notes                                                                                                                                       | BG layers |
| ---- | ------ | ----- | ------ | --- | ---------- | --------- | ------------------------------------------------------------------------------------------------------------------------------------------- | --------- |
| 0    | Tiled  |       |        |     |            |           | 4 regular backgrounds.                                                                                                                      | 4         |
| 1    | Tiled  |       |        |     |            |           | 2 regular + 1 affine bg.                                                                                                                    | 3         |
| 2    | Tiled  |       |        |     |            |           | 2 affine bgs.                                                                                                                               | 2         |
| 3    | Bitmap | 240   | 160    | 16  | 1x`12C00h` | No        | VRAM is filled with raw pixel data, 2bytes per pixel in 555 format. Supports all 32768 colors at once.                                      | 1         |
| 4    | Bitmap | 240   | 160    | 8   | 2x`9600h`  | Yes       | VRAM is filled with 8-bit indices into the bg palette. Supports 256 colors at once. Allows page flipping. Can be slow if implemented wrong. | 1         |
| 5    | Bitmap | 160   | 128    | 16  | 2x`A000h`  | Yes       |                                                                                                                                             | 1         |
Note that there is no real mode7 on the GBA like there is on the SNES, but the same effect can be accomplished with `HBlank` interrupts and DMA transfers.

Mode is set by the low bits in the `REG_DISPCNT` register (`0x0400_0000 [2:0]`). The layers are enabled by bits `[12:8]` in the same register (in the order `[Obj, BG3, .. BG0]`). Note bitmap mode only supports `BG2`, I think?

Note, bitmap/matrix layout in C is `px(x, y) = mem[w*y + x]` (note that `bpp` also plays into the memory layout). Note! It's not possible to write to the VRAM in byte-sized chunks, must access it as 16-bit or 32-bit! Can do read-modify-write cycles to replace single bytes.

Page flipping = double buffering (drawing to a VRAM buffer off-screen while the second buffer is rendering), but without any copying of data (just swapping a pointer deciding what is the work area and what is the display buffer). It can be used to reduce screen tear caused by changing data while it's rendered. Note that care has to be taken when doing partial redraws between frames (since what's on page 1 won't necessarily be on page 2). The second page is always located at address `0x0600_A000`. Which page is drawn is decided with `REG_DISPCNT[4]`.

In general, bitmap modes are not very useful, as they are not performant when compared to tiled modes. Might be useful for static images (but note that only one mode can be active at a time! Can't mix different modes between the layers). The bitmap modes only support one BG layer.

Additionally, Mode 3-5 overlaps the sprite tile memory, so only tiles 512-1023 will be available in the bitmap modes.

Note the bitmap modes might be more useful in 3D games than tiled modes (eg, Starfox or similar games). The main limitation is the CPU speed of the GBA, and how efficiently the VRAM is accessed.
# Getting data into the GBA
Image data is pretty big. A mode 3 image is `240*160*2 byte = 76800 byte`, which is more than double the size of the internal work memory (see [[GBA Hardware]] section on memory). If loading arrays like with `GBDK`, marking them as `const` will put them in the external work memory instead (which is 256KB).

It's also possible to mark any data (non-`const` too) to be put in the external work RAM, by marking it with:
```c
u32 some_array[256] __attribute__((section(".ewram")));

// Or, with some defines
#define EWRAM_DATA __attribute__((section(".ewram")))
#define IWRAM_DATA __attribute__((section(".iwram")))
#define  EWRAM_BSS __attribute__((section(".sbss")))

#define EWRAM_CODE __attribute__((section(".ewram"), long_call))
#define IWRAM_CODE __attribute__((section(".iwram"), long_call))

u32 some_array[256] EWRAM_DATA;
```

There's nicer ways of getting data in, including an actual FAT file system that's part of `libgba` (`libfat`, also, `gbfs`).

There are some binary tools for data conversions that are part of `devkitPro`: `raw2c.exe`, `bin2s.exe`, `bmp2bin.exe`, `grit.exe` (for converting images), `gbfs.exe` (and some other associated tools).

There is a full paint-like software that uses the same export code as `grit` called [Usenti](https://www.coranac.com/projects/#usenti). It works with palettes etc. and is made specifically for working with images for the GBA.

The appropriate way to go about things seems to be using `grit`, which is called by the `Makefile`, using a `.grit` file with information on how it should be handled. These are the arguments to give to `grit` when running it (for example, `-gb -gB8` below). This is also used by `butano`, but there it uses a `.json` format, and there's some extensive information [here](https://gvaliente.github.io/butano/import.html).

It can also be called directly, similar to `png2asset`:
```bash
# grit
# mode 3, 5 (C array; u32 fooBitmap[]; foo.c foo.h)
    grit foo.bmp -gb -gB16
# mode 4 (C array; u32 fooBitmap[], u16 fooPal[]; foo.c foo.h)
    grit foo.bmp -gb -gB8
```
Some more information on memory management and [compression](https://web.archive.org/web/20180820012154/http://members.iinet.net.au/~freeaxs/gbacomp/) is available.

Note, `memcpy()` is available and usable, but requires some care (proper 32-bit alignment for example). `libtonc` provides some substitutes: `memcpy16()` and `memcpy32()`, which are (claimed to be) more efficient.
# Input
Note, for details on the key layout, see [[GBA Hardware]].

The keypad can be read with `REG_KEYINPUT` (`0x0400_0130`). An important distinction to `GBDK` is that these bits are active low (0 when pressed, not 1). Something like this might be of use (`libtonc` provides better interfaces for it, but it essentially does this):
```c
#define KEY_DOWN_NOW(key)  (~(REG_KEYINPUT) & key)
```
Note that there's also another register `REG_KEYCNT` (`0x0400_0132`) which can be used to set up hardware interrupts on keypresses.
# Tiles
In the tiled modes (0-2 and 7), 8x8 tiles are loaded into `VRAM`. These are then interpreted by the four BG layers and the OAM (sprites), according to the Screenblock Entry (SE, corresponding to the tilemap in `GBDK`). This contains an index to the tile, a palette (0-15) and some bits from flipping x/y. Two of the backgrounds support affine transformations.

The sprites (128 of them) can be various sizes between 8x8 and 64x64, controlled by the `OAM`. Each sprite contains a pointer to the first tile, the size and type of mapping (1D/2D), sometimes a 2x2 affine matrix (can't have one for every single sprite) for rotation/scale/shear, and a palette. These work on the whole sprite, not on a specific tile.

Note on memory layout; in bitmap mode, `VRAM` is organized into scanlines. In tiled modes, `VRAM` contains 8x8 bitmaps. Here the entire 64 pixels are laid out in sequence before the next tile starts. Each "pixel" here represents an 8bpp index into a 256-bit palette, or a 4bpp index into a 16-bit palette. Which palette to use is described in the Screenblock Entry/tilemap. Essentially, the entire tile memory can be viewed as a 8px by 8 times N pixels bitmap.

The `VRAM` is split into 6 `charblock`s of 16KB (each block containing either 512 s-tiles (4bpp) or 256 d-tiles (8bpp)). These `charblock`s are dedicated to specific task, with the first four for `BG0-3` and block 4 and 5 dedicated to sprites (block 4 starting at `0x0601_0000`). There are entries in the control registers to define which character base block (`CBB`) is used for each layer. Sprites always use the first sprite `charblock` as index 0.

Each tile index (both BG and sprite) is 10-bit (0-1023). This means that you can access the `CBB` with index 0-511 and the `charblock` following it with index 512-1023.

Note that in the bitmap modes, the available `charblock`s extend into the sprite area, so only the higher 512-1023 indices can be used for sprites.

Note that palettes are in a different memory area, background starting at `0x0500_0000`, with sprites starting at `0x0500_0200`. Each palette contains 256 entries of 15-bit colors (either 256 colors, or 16x16 colors). Index 0 is transparent (note, for backgrounds too!).
# Sprites
Shorthand:
* Load the sprite tiles (8x8) into `VRAM` (starting at `0x0601_0000`). This is also called `OVRAM` (Object VRAM).
* Load the palette into PAL RAM (starting at `0x0500_0200`).
* Set attributes in the OAM (starting at `0x0700_0000`) for the sprite in question.
* Activate Object layer bit in `REG_DISPCNT` and set the mapping mode (1D/2D).
Like with backgrounds, sprites can be stored as s-tiles or d-tiles, allowing for both 256 color sprites and 16 color sprites using palettes. No matter the bit depth, sprite blocks are always done in 32 byte offsets.

Note: Again, in mode 3-5, the background uses the first sprite `charblock`. Only sprite tile index 512-1023 (the upper sprite block) is available in this mode.

Of the tile mapping modes for sprites, 1D is closest to what we did in GBDK (but not quite the same due to the 8x16 hardware sprites). The export tool (like `grit` or `Usenti`) need to be configured to use the right layout.
```sh
# grit
# 4x 16x16@4 objects (C array; u32 fooTiles[], u16 fooPal[]; foo.c, foo.h)
# -Mw 2 and -Mh 2 tells grit the meta-tile width (in number of tiles)
    grit foo.bmp -gB4 -Mw 2 -Mh 2
```

The OAM needs to be filled in with data to set up the sprite correctly. Each sprite entry consists of:
* A 48-bit attribute block (three 16-bit fields, `s16`).
* A 64-bit affine block (four 16-bit fields, `s16`). These are the 2x2 transform matrices.
These are laid out in memory in such a way that they overlap each other, giving 32 affine matrices and 128 attribute blocks (one for each sprite object). The data of the two are interwoven. Note the use of `s16`, not `u16`!

Sprite attributes contain:
* X (9-bits)/Y (8-bits) position (defining top-left corner of sprite). Note these are signed values.
* Various rendering attributes (affine, blend, mosaic).
* Color mode (4bpp or 8bpp).
* Sprite shape, from 8x8 to 64x64 (can also cover 16 and 32, and combinations thereof).
* Flip V/H (or affine matrix ID 0-31).
* Base tile index. Can update this to animate a sprite!
* Priority (lower prio are drawn on top). Combines with object attribute number if they share the same priority. Sprites are always drawn on top of BGs with the same priority.
* Palette bank (0-15) in 4bpp mode.

Note that OAM **must** only be updated during `VBlank`. `libtonc` suggests using a shadow OAM that can be updated at any point (but must be copied during `VBlank` to apply). This changes the regular game-loop somewhat:

```c
while(1) {
    // This logic can be done at any point during the frame
    check_inputs();
    update_logic();
    set_shadow_oam(); // in RAM, not OAM
    // Wait for the vsync (VBlank area, after the screen has been rendered)
    vsync();
    // Now is the safest time to write to OAM
    copy_shadow_oam_to_oam_mem();
}
```

Note, should use the `libtonc` function `oam_init()` or similar before turning on the Object layer. When the OAM buffer is zeroed out, this doesn't mean the sprites are hidden, so janky behavior may ensue. Calling `oam_init()` sets the sprite draw mode to hidden for all 128 objects.

For copying the shadow OAM, these functions are provided by `libtonc`:
```c
// Copy n (0-127) objects from obj_buffer to the OAM (doesn't care about affine)
oam_copy(oam_mem, obj_buffer, n);

// --- OR, use these if affine transforms are used ---

// Update n OBJ_ATTRs (0-127)
obj_copy(obj_mem, obj_buffer, n);
// Update m OBJ_AFFINEs (0-31)
obj_aff_copy(obj_aff_mem, obj_aff_buffer, m);
```
# Affine sprites
There's a lot of linear algebra math going on with the affine transformations. The GBA has 32 affine transformation matrices for sprites, residing in OAM memory (overlapping it, in fact). The transformation matrix is 2x2 (values called `PA`, `PB`, `PC`, `PD`). The [TONC tutorials](https://gbadev.net/tonc/affine.html) describe the math in great detail, though it's still somewhat confusing.

Essentially, using affine transformations allow for rotating, scaling and shearing sprites. The effect is quite pixelated due to the limited fixed point precision of the GBA hardware.

Notably, the sprite is still rendered inside the same block (so if you have a 16x16 sprite and want to scale it to 200%, it's likely going to be clipped, since the hardware only uses 16x16 pixels to draw it). This can be improved on by setting the affine double render area bitflag.

Shorthand:
* Set `ATTR0_HIDE` or `ATTR0_AFF_DBL` in the `OBJ_ATTR` block for the sprite in the OAM.
* Set `ATTR1_AFF_ID(n)` to the affine transform ID (0-31).
* Set the OAM affine parameters (`PA-PD`) in the shadow buffer.

Recall the OAM shadow buffer:
```c
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *const obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;
```
We can make a cast of the buffer using the `OBJ_AFFINE` type, which will allow us to index the buffer correctly with 0-31. Note that if you are using double sprite size, you need to adjust the render position on the screen accordingly as well using `obj_set_pos()`.

`libtonc` has some convenience functions, for example:
```c
obj_aff_rotate(&obj_aff_buffer[sprite_id], theta);
```
Theta here is the degrees to rotate (counter-clockwise around the center of the sprite). Note that the angle is a fixed point value, and not in degrees. Experiment to get the desired effect. There's also `obj_aff_rotscale(&obj_aff_buffer[sprite_id], sx, sy, theta)`, and if you understand the math, there's functions for arbitrarily multiplying matrices together. Note that use of division (such as when calculating inverse matrices) doesn't have hardware support, so it will be slow.
# Tiled backgrounds
Tiled backgrounds are used in mode 0, 1 and 2.

Just like with sprites, tiles are 8x8 pixels. Tile-maps can be between 128x128 and 1024x1024 per layer. How many BG layers are available (1-4) depends on the mode.

Note, `charblock`s (the memory area where the tile data is located) and `screenblock`s (the memory area where the tile-map data is located) overlaps. Each `charblock` takes up the same area as eight `screenblock`s. In other words, there are up to four `charblock`s dedicated to background tiles, and up to 32 `screenblock`s dedicated to tile-maps. A `charblock` is 16KB and a `screenblock` is 2KB. A `charblock` contains 256 tiles.

Unlike GBDK, the user has to manage which `charblock`s to dedicate to tiles and which to tile-maps (though of course, these can be swapped in memory by loading in new data from `EWRAM/ROM`).

Just like in GBDK, the background layers can be scrolled in hardware. Because each layer has individual offsets, this allows for easy parallax effects. The tiled graphics modes:

| mode | BG0 | BG1 | BG2 | BG3 |
| :--: | :-: | :-: | :-: | :-: |
|  0   | reg | reg | reg | reg |
|  1   | reg | reg | aff |  -  |
|  2   |  -  |  -  | aff | aff |
The background control registers (3 per layer) control how the layer behaves. Note that the size bits are different when the layer is affine! The scroll values (which are write-only!) are 16-bits for each dimension, and the map loops. The scroll values is the offset of the "camera", not the background itself (so the sign is inverted when compared to the sprites).

Screen entries (16-bit values in a `screenblock`, a single block takes up 1024 of these) contain:
* A 10-bit tile index (offset by the background layer's origin `charblock`). Note that pointing to `CBB[3]` doesn't allow using sprite `CBB[4]` (may work in emulation, doesn't work in HW).
* Horizontal and vertical flip flags.
* Palette bank to use in 16-color mode. Color mode is set in the control register for the background layer.
A single `screenblock` contains 1024 screen blocks, allowing for 32x32 tiles, which is a 256x256 pixel background. The bigger map sizes require more `screenblock`s.

Note that bigger maps are separated into 32x32 tile chunks (one for each `screenblock`), so to figure out the coordinate of a tile in a bigger block, you first need to figure out which `screenblock` it belongs to. Layout of the screen blocks are sequential in memory:

| 128 | x   | 64  | map |
| --- | --- | --- | --- |
| 0   | 1   | 2   | 3   |
| 4   | 5   | 6   | 7   |
Above are the indices of the `screenblock`s in memory of a 128x64 tile map. Each `screenblock` consists of 32x32 tiles.

There's some wonky differences in how sprite tiles and background tiles are indexed, but it only applies to 8bpp.

Note that while `charblock`s and `screenblock`s overlap in memory, there's nothing preventing the user from utilizing a single `charblock` for both tasks. For example, storing the tiles in the start of `CBB[0]`, filling up half of it, and pointing the tile-map to `SBB[4]`. This is valid and compact! 

Just like with sprites, `grit` is used to generate both the tiles and tile-maps (the `-mRtpf` compacts the tile, palette and flipped data the same way that `png2asset` does. This is the default. `-m` needs to be present to export map data):
```sh
# grit
# (C array; u32 fooTiles[], u16 fooMap[], u16 fooPal[]; foo.c, foo.h)
    grit foo.bmp -gB4 -mRtpf
```
Note, for maps larger than 32x32 (i.e., using more than one `screenblock`), the `-mLs` parameter must be used, otherwise the layout will be flat. Just like with GBDK, this should generate const arrays with tile data, palette data and tile-map data, which can then be loaded into the correct parts of the memory.

Shorthand:
* Load tiles into `charblock`s, starting at `0x0600_0000` (with the second block starting at `0x0600_4000`, etc.).
* Load the colors into the background palette at `0x0500_0000`.
* Load the map into one or more `screenblock` (overlaps with the `charblock`s in `VRAM`).
* Initialize the background layer's control registers to use the right `CBB`, `SBB` and `bpp`.
* Set the mode to 0-2 in `REG_DISPCNT` and activate the background layer(s).

Note, `memcpy` or `libtonc`'s `memcpy32` can be used to load the tiles/maps into memory.
# Affine backgrounds
(These are also tiled, so most of the above applies, with some differences).

Affine backgrounds are only available in mode 1 (`BG2`) and mode 2 (`BG2` and `BG3`). When using affine backgrounds, the size registers do different things. Another change is that the affine background data is laid out linearly in memory instead of in 32x32 chunks like regular backgrounds.

| mode | bg0 | bg1 | bg2 | bg3 |
| :--: | :-: | :-: | :-: | :-: |
|  0   | reg | reg | reg | reg |
|  1   | reg | reg | aff |  -  |
|  2   |  -  |  -  | aff | aff |
`REG_BGxCNT` (only on 2/3 for affine) still control the background parameters. This is where the priority, `CBB` and `SBB` are set. `Wr` (wrap/`BG_WRAP`) flag makes the affine background wrap on itself (otherwise only a single instance will be rendered). The `Sz` flag follows the table below.

| Sz  |     define     |  tiles  |  pixels   |
| :-: | :------------: | :-----: | :-------: |
| 00  |  BG_AFF_16x16  |  16x16  |  128x128  |
| 01  |  BG_AFF_32x32  |  32x32  |  256x256  |
| 10  |  BG_AFF_64x64  |  64x64  |  512x512  |
| 11  | BG_AFF_128x128 | 128x128 | 1024x1024 |
The transform itself is set with the `REG_BGxPA-PD` registers. The `REG_BGxHOFS/REG_BGxVOFS` registers doesn't work when using affine backgrounds, instead the `REG_BGxX/REG_BGxY` registers perform a similar function. Note these are `20.8` fixed-point numbers.

Note, in `libtonc` the `BG_AFFINE` struct is set up to map onto `REG_BG_AFFINE[2]`, rather than setting the registers individually. The identity matrix for backgrounds (`PA` and `PD` set to `1<<8` due to fixed-point. The last two parameters are `dx/dy`):
```c
const BG_AFFINE bg_aff_default = { 256, 0, 0, 256, 0, 0 };
```
`REG_BGxX/REG_BGxY` (in other words, `dx/dy`) set the offset from the top-left corner of the screen. It would seem the 2x2 matrix is really a 3x2 (similar to how 3D usually uses a 4x3 rather than 3x3, with the last column containing an offset).

Note that tile and tile-map data is laid out differently than for regular backgrounds. The screen entries are 1-byte long, only containing an index of the tile to use (0-255), so only the tiles in the `CBB` are usable (not the block after it like with regular backgrounds). 256-color tiles are used, so there's no smaller palettes to juggle (this means, don't use `tile_mem`, use `tile8_mem`). This also means there's no individual tile flipping. 

Because they are 256-color (taking up all the colors of the background palette), one has to be clever with managing the color budget, especially when using the mixed mode 1.

Shorthand:
* Load tile data into `charblock` 0-3 and tile-map data into `screenblock`.
* Set graphics mode to 1 or 2.
* In `REG_BG2CNT` (and 3 in mode 2), set up the background parameters, including the size, wrap and `CBB/SBB`.
* Set the affine transformation on `REG_BG_AFFINE`, for example using `bg_rotscale_ex()`. Note! Only do this during `VBlank`, or expect weirdness!
# Mode 7
An interesting hack is possible on the GBA to reproduce the SNES mode 7 (which is not available on GBA hardware). By using `HBlank` or `HDMA` to update the affine transformation matrices, it's possible to change the transform each scanline, achieving a perspective effect.

The transformation equation can be written in terms of linear algebra:
$$
\lambda C * x_p = x_w - a_{cw}
$$
$$
S(1, -1, 1) * x_s = (x_p - a_{sp})
$$
Where $\lambda$ is the scaling factor, `C` is the camera matrix expressed as a rotation of pitch and yaw, $x_p$ is the coordinates on the projection plane, $x_p=(x_p, y_p, -D)$, $x_s$ is the coordinates on the screen $x_s=(x_s,y_s,0)$, $a_{cw}$ is the location of the camera in world space, $a_{sp}$ is the location of the screen origin in camera space, $a_{sp} = (L, T, -D)$. $x_w$ is the coordinates in world space (all of this is way better explained in [this tutorial](https://gbadev.net/tonc/mode7ex.html), and a proper example is given in the `adv/m7_ex` demo. The demo also demonstrates rotating sprites and positioning them in the world, and a fog effect).

One restriction is that the x-axis of the camera frame must be parallel to the ground plane, meaning we can to pitch and yaw rotations, but not roll. All the coordinate systems are right-handed. $L$ and $T$ above relate to the GBA screen, so that $L=-120$ and $T=80$.

For additional complexity, we can introduce a horizon line. Using `HBlank`, we can switch between different graphics mode halfway into an image, using a regular background (mode 0 for example) to render the sky, and mode 7 to render the floor in perspective. Unfortunately, just switching on/off the background may not work as intended, since the hardware takes a few scanlines to do this, so using mode 1 with a combination of regular and affine backgrounds doesn't work.

The solution: change mode at the horizon scanline, and at the same time, change the CBB/SBB for the background in question (probably `BG2`).
# Sound
Sound is complicated. There are 6 sound channels; the four wave channels (DMG) from Game Boy (square, square, wave, noise) and two Direct Sound sample channels.

Note! Most sound registers are inaccessible until sound is activated in `REG_SNDSTAT[7]`! `REG_SNDDMGCNT` sets volume and activates the DMG sound channels. `REG_SNDDSCNT` also has some volume controls (including for the `DSound` channels).

DMG1 differs from DMG2 in that it has a frequency sweep register, allowing the sound to change over time. Fade in/attack, remain level/sustain, fade out/decay.

The `hUGEDriver` from Game Boy and Game Boy Color is not supported natively in GBA games. `devkitPro` typically uses `Maxmod`, which is part of the toolchain. There is an older driver called GBT Player, which uses the DMG sound channels.

The process is:
* Make a `.mod` format song (`.xm` supported, but seem to be issues?) and put it in the `music` folder.
* Make sure the `music` folder is pointed to in the `Makefile`.
* The build step will generate `build/soundbank.h` and `build/soundbank_bin.h` and link in the song data. The same two headers are used even if there are multiple songs.
* Put the following code to play the song:
```c
#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

int main(void) {
	irq_init(NULL);
	// Set up Maxmod IRQ on VBlank
	irq_add(II_VBLANK, mmVBlank);
	// Load Maxmod music data. The 8 is the number of SFX channels
	mmInitDefault((mm_addr)soundbank_bin, 8);
	...
	// Play the MOD_SONG_NAME song (defines in "soundbank.h")
	mmStart(MOD_SONG_NAME, MM_PLAY_LOOP);
	
	while(1) {
		...
		VBlankIntrWait();
        // Maxmod, update music
        mmFrame();
        ...
	}
}
```
