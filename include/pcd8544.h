#ifndef PCD8544_H
#define PCD8544_H

#include <stdint.h>

//based on https://github.com/carlosefr/pcd8544 made to work with the libopencm3
void LcdInitialise(void);
void LcdString(char *characters);
void LcdCharacter(char character);
void LcdFlipBuffer(void);
void LcdSetPixel(uint8_t x, uint8_t y);

#endif
