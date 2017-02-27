#ifndef HEX_KEY_H
#define HEX_KEY_H

#include <stdint.h>
#include <stdbool.h>

void hex_key_init(void);
void hex_key_cycle(void);
bool hex_check_key(uint8_t key);

#endif
