# CHIP8-UEFI
A [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) emulator that works in the UEFI environment without any OS.
Works both on real hardware and QEMU.

The repo also contains some popular CHIP-8 roms, located in `roms`.
You can add your own games there.
The roms are sourced from [here](https://github.com/kripod/chip8-roms).

## Compiling
You need `clang`, `lld` and `make`. Then just run `make all`.

## Running in QEMU
You need to have `qemu-system-x86_64` and `ovmf` installed.
If your distribution does not come with `ovmf` you need to find a `OVMF.fd` file online and put it to the repo root.

When you have all the prerequisites ready just run `make run`.

## Running on real hardware
Run `make partition` to create the required folder structure.
Then just put the contents of the `./partition` folder to your FAT32 formatted pendrive and boot from it.