#ifndef _IO_EXPANDER_H
#define _IO_EXPANDER_H

#include <avr/io.h>

/* I2C Setup Options */
#define F_I2C 100000
#define PRESCALER_I2C 1

void pcf8574_init (void);

unsigned char pcf8574_send_start (void);

void pcf8574_send_stop (void);

unsigned char pcf8574_send_add_rw (unsigned char address, unsigned char rw);

unsigned char pcf8574_send_byte (unsigned char byte);

unsigned char pcf8574_read_byte (void);

unsigned char pcf8574_get_inputs (unsigned char address);

void pcf8574_set_outputs (unsigned char address, unsigned char byte);

#endif
