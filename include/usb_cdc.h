#ifndef USB_CDC_H
#define USB_CDC_H

#include <libopencm3/usb/usbd.h>

#define USB_BUF_SIZE 64

void usb_init(void);
void usb_poll(void);
uint8_t usb_get(char *buf);

#endif
