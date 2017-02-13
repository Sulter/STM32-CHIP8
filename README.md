# STM32-CHIP8
CHIP8 implementation on STM32F04

requires:
https://github.com/texane/stlink
arm-none-eabi

1)
git submodules init

2)
git submodules update

3)
cd libopencm3
make

4)
make V=1
make flash
make clean
etc.


Debugging

1)
st-util

2)
arm-none-eabi-gdb main.elf
