Can try your luck with the [installer](https://github.com/devkitPro/installer/releases), but I didn't test it.
# Links
**Community, some incomplete documentation:**
[gbadev](https://gbadev.net/getting-started.html)
[gbadev FAQ](https://gbadev.net/forum-archive/thread/14/418.html)
**Low level:**
[TONC tutorials](https://gbadev.net/tonc/) (Description of coding for the GBA using low-level C/Assembly. Very thorough).
[GBATEK](https://problemkaputt.de/gbatek.htm) (Description of all the registers and functions of the hardware).
**High level:**
[butano](https://github.com/GValiente/butano) (C++ framework, abstracting away much of the low-level hardware interactions, while still not being a full game engine).
# My own notes
[[GBA Hardware]]
[[GBA TONC notes]]
# MSYS2 installation guide
Ideally, you already have [MSYS2](https://www.msys2.org/) installed from the Game Boy Color gamejam. If not, install it and make sure that `make` is installed:
```bash
pacman -S make
```
More or less follow the instructions from [here](https://devkitpro.org/wiki/devkitPro_pacman), but without using `sudo`.

Specifically, follow these steps in `MSYS2`:
```bash
nano .profile
```
Add these lines to the end of the file and save it:
```bash
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=/opt/devkitpro/devkitARM
export DEVKITPPC=/opt/devkitpro/devkitPPC
```
Restart the `MSYS2` terminal to get these new environment variables loaded.

Then, run these commands:
```bash
pacman-key --recv BC26F752D25B92CE272E0F44F7FD5492264BB9D0 --keyserver keyserver.ubuntu.com
pacman-key --lsign BC26F752D25B92CE272E0F44F7FD5492264BB9D0
pacman -U https://pkg.devkitpro.org/devkitpro-keyring.pkg.tar.zst
```

Next, add the libraries to the `pacman` config file:
```bash
nano /etc/pacman.conf
```
Add these fields to the end of the file and save it:
```
[dkp-libs]
Server = https://pkg.devkitpro.org/packages

[dkp-windows]
Server = https://pkg.devkitpro.org/packages/windows/$arch/
```
Next, resync `pacman` and install the `gba-dev` package:
```bash
pacman -Syu

pacman -S gba-dev
```
Say yes to all. At the end, edit `.profile` again:
```bash
nano .profile
```
And add this line:
```bash
source /etc/profile.d/devkit-env.sh
```

Now, next time `MSYS2` is opened, the `gba` toolchain should be available!
# Setting up the first project from scratch
Examples for `gba` are installed in `/opt/devkitpro/examples/gba`. Copy the template `Makefile` to your project:
```bash
mkdir /c/path_to_projects/gba_test
cd /c/path_to_projects/gba_test
cp /opt/devkitpro/examples/gba/template/Makefile .
mkdir source
mkdir include
```
Then, create `main.cpp` in the `source` folder:
```cpp
int main() {
	while(1);
	return 0;
}
```
Note that the `Makefile` is set up to have all `.cpp` files under source and all headers (`.h/.hpp`) under `include`.

Compile this in MSYS2 with the command:
```bash
make
```
This should generate the `gba_test.gba` file in the project directory. Note that the `-j` option is supported for faster compilation using multiple cores:
```bash
make -j16
```

Need the `gba_bios.bin` file from [here](https://www.mediafire.com/download/uijj3i3349h8j2j/gba_bios.zip) to run it in `MesenCE` (it will ask once).

Just drag the `gba_test.gba` file into `MesenCE` and provide the bios file, and the "game" will boot!
# Engines and Frameworks
## Working with the `libgba` examples
**NOTE! Probably don't use this API? Not super well documented.**

For ease of use, put the `gba` examples somewhere that `VSCode` can see:
```bash
mkdir gba_examples
cp -r /opt/devkitpro/examples/gba/* gba_examples
```
The examples use a library called [libgba](https://github.com/devkitPro/libgba), which is part of the framework. This is a very low level framework.
## More examples from `libtonc`
**NOTE! Probably don't use this API? Not unless we want to work with low-level hardware, which could be fun. Very close to the metal. Very well documented in the [TONC tutorials](https://gbadev.net/tonc/); it's pretty much a book.**

The examples are a bit sparse, but there are more available from `libtonc`:
```bash
git clone https://github.com/gbadev-org/libtonc-examples
```
These are a lot more relevant to learning about the features of the platform. `libtonc` itself is part of the `devkitPro` distribution, you already have the library itself. This is another very low level framework.
## High level engine, `Butano`
**NOTE! Recommended.**

A more high level framework/engine for the `gba` is available: [butano](https://github.com/GValiente/butano). It also comes with examples.

Note that `butano` requires python for the build environment as well, install it with `MSYS2`:
```bash
pacman -S python
```

The `butano` documentation is available [here](https://gvaliente.github.io/butano/).,
Information about [importing assets](https://gvaliente.github.io/butano/import.html) like images and music.
The [API](https://gvaliente.github.io/butano/modules.html) documentation.
The [FAQ](https://gvaliente.github.io/butano/faq.html).
The code [examples](https://gvaliente.github.io/butano/examples.html).

There is a `template` project in the root of the `butano` repo. Use this when setting up a new project.

While this is a "high level" framework compared to `libtonc` and `libgba`, it's by no means a full engine on the scale of `Godot`. For example, while there's functions for loading and displaying sprites, playing music and sound, interacting with the keypad and other peripherals, there's no physics engine or scene graph.
