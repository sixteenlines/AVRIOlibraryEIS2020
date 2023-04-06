#include "io_expander.h"

void pcf8574_init (void)
{
	/*set bus speed according to page 175 datasheet*/

	TWBR = 0x12;    //100kHz @ 16MHz F_CPU
}


unsigned char pcf8574_send_start (void)
{
	/*writing a one to TWINT clears it, TWSTA=Start, TWEN=TWI-enable*/
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	/*wait, until start condition has been sent --> ACK*/
	while (!(TWCR & (1<<TWINT)));
	return TWSR;
}


void pcf8574_send_stop (void)
{
	/*writing a one to TWINT clears it, TWSTO=Stop, TWEN=TWI-enable*/
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);	
}


unsigned char pcf8574_send_add_rw (unsigned char address, unsigned char rw)
{
	/*address can be 0 .. 8; rw=0 --> write, rw=1 --> read*/
	unsigned char addr_byte = 0;
	/*shift address one bit left*/
	addr_byte = address << 1;
	/*set RW-Bit, if necessary*/
	addr_byte |= rw;
	/*0b0100xxx0 --> address of Expander*/
	addr_byte |= 0b01000000;
	/*TWDR contains byte to send*/
	TWDR = addr_byte;
	/*send content of TWDR*/
	TWCR = (1<<TWINT) | (1<<TWEN);
	/*wait, until address has been sent --> ACK*/
	while (!(TWCR & (1<<TWINT)));
	return TWSR;
}


unsigned char pcf8574_send_byte (unsigned char byte)
{
	/*TWDR contains byte to send*/
	TWDR = byte;
	/*send content of TWDR*/
	TWCR = (1<<TWINT) | (1<<TWEN);
	/*wait, until byte has been sent --> ACK*/
	while (!(TWCR & (1<<TWINT)));
	return TWSR;
}


unsigned char pcf8574_read_byte (void)
{
	/*send content of TWDR; TWEA = enable ACK*/
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	/*wait, until byte has been received --> ACK*/
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}


unsigned char pcf8574_get_inputs (unsigned char address)
{
	pcf8574_init ();
	pcf8574_send_start ();
	pcf8574_send_add_rw (address, 1);
	unsigned char input = pcf8574_read_byte ();
	pcf8574_send_stop ();
	return input;
}


void pcf8574_set_outputs (unsigned char address, unsigned char byte)
{
	pcf8574_init ();
	pcf8574_send_start ();
	pcf8574_send_add_rw (address, 0);
	pcf8574_send_byte (byte);
	pcf8574_send_stop ();
}

