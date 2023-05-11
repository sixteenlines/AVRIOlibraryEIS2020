#include "lcd_i2c.h"
#include "pcf8574.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LiquidCrystalDevice_t lcd_init(uint8_t address, uint8_t columns, uint8_t rows, uint8_t dotSize)
{
	LiquidCrystalDevice_t device;

	device.Address = address;
	device.Columns = columns;
	device.Rows = rows;
	device.Backlight = LCD_NOBACKLIGHT;
	device.DisplayFunction = LCD_4BITMODE | LCD_1LINE | dotSize;
	device.DisplayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;

	// Initialize to default text direction (for roman languages)
	device.DisplayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	if (rows > 1)
	{
		device.DisplayFunction |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotSize != 0) && (rows == 1))
	{
		device.DisplayFunction |= LCD_5x10DOTS;
	}

	// according to datasheet, we need at least 40ms after power rises above 2.7V
	_delay_ms(100);

	// Now we pull both RS and R/W low to begin commands
	lcd_transmitI2C(&device, LCD_NOBACKLIGHT); // reset expander and turn backlight off (Bit 8 =1)
	_delay_ms(1000);

	// put the LCD into 4 bit mode
	//  this is according to the hitachi HD44780 datasheet
	//  figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	lcd_transmitI2C(&device, 0x03 << 4);
	_delay_us(4500); // wait min 4.1ms

	// second try
	lcd_writeDevice4Bits(&device, 0x03 << 4);
	_delay_us(4500); // wait min 4.1ms

	// third try
	lcd_writeDevice4Bits(&device, 0x03 << 4);
	_delay_us(150); // wait min 150 mics

	// set to 4-bit interface
	lcd_writeDevice4Bits(&device, 0x02 << 4);

	// set # lines, font size, etc.
	lcd_sendCommand(&device, LCD_FUNCTIONSET | device.DisplayFunction);

	// turn the display on with no cursor or blinking default
	lcd_turnOnDisplay(&device);

	// clear it off
	lcd_clear(&device);

	// set the entry mode
	lcd_sendCommand(&device, LCD_ENTRYMODESET | device.DisplayMode);

	lcd_returnHome(&device);

	return device;
};

void lcd_print(struct LiquidCrystalDevice_t *device, char *value)
{
	char letter = *value;

	while (letter != 0x00)
	{
		lcd_writeDeviceByte(device, letter, LCD_REGISTER_SELECT_BIT);
		letter = *(++value);
	}
};

void lcd_printChar(struct LiquidCrystalDevice_t *device, char value)
{
	lcd_writeDeviceByte(device, value, LCD_REGISTER_SELECT_BIT);
};

void lcd_printInt(struct LiquidCrystalDevice_t *device, int value)
{
	int length = snprintf(NULL, 0, "%d", value);

	// Allocate memory for the string
	char *strValue = malloc(length + 1);
	if (strValue == NULL)
		return;
	snprintf(strValue, length + 1, "%d", value);

	// Print
	lcd_print(device, strValue);
	free(strValue);
}

void lcd_printDouble(struct LiquidCrystalDevice_t *device, double value, int precision)
{
	if (value == 0)
	{
		lcd_printChar(device, '0');
		lcd_printChar(device, '.');
		lcd_printChar(device, '0');
		lcd_printChar(device, '0');
	}
	else if ((value >= (-2147483647)) && (value < 2147483648))
	{
		// Print sign
		if (value < 0)
		{
			value = -value;
			lcd_printChar(device, '-');
		}
		// Print integer part
		lcd_printInt(device, value);
		lcd_printChar(device, '.');
		// Print decimal part
		lcd_printInt(device, (value - (int)(value)) * precision);
	}
}

void lcd_turnOnBacklight(struct LiquidCrystalDevice_t *device)
{
	device->Backlight = LCD_BACKLIGHT;
	lcd_transmitI2C(device, 0);
}

void lcd_turnOffBacklight(struct LiquidCrystalDevice_t *device)
{
	device->Backlight = LCD_NOBACKLIGHT;
	lcd_transmitI2C(device, 0);
}

void lcd_clear(LiquidCrystalDevice_t *device)
{
	lcd_sendCommand(device, LCD_CLEARDISPLAY); // clear display, set cursor position to zero
	_delay_us(2000);						   // this command takes a long time!

	lcd_setCursor(device, 0, 0);
}

void lcd_setCursor(LiquidCrystalDevice_t *device, uint8_t row, uint8_t column)
{
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

	lcd_sendCommand(device, LCD_SETDDRAMADDR | (column + row_offsets[row]));
}

void lcd_returnHome(LiquidCrystalDevice_t *device)
{
	lcd_sendCommand(device, LCD_RETURNHOME); // set cursor position to zero
	_delay_us(2000);						 // this command takes a long time!
};

void lcd_turnOnDisplay(LiquidCrystalDevice_t *device)
{
	device->DisplayControl |= LCD_DISPLAYON;
	lcd_sendCommand(device, LCD_DISPLAYCONTROL | device->DisplayControl);
};

void lcd_turnOffDisplay(LiquidCrystalDevice_t *device)
{
	device->DisplayControl &= ~LCD_DISPLAYON;
	lcd_sendCommand(device, LCD_DISPLAYCONTROL | device->DisplayControl);
};

void lcd_turnOnCursor(struct LiquidCrystalDevice_t *device)
{
	device->DisplayControl |= LCD_CURSORON;
	lcd_sendCommand(device, LCD_DISPLAYCONTROL | device->DisplayControl);
}

void lcd_turnOffCursor(struct LiquidCrystalDevice_t *device)
{
	device->DisplayControl &= ~LCD_CURSORON;
	lcd_sendCommand(device, LCD_DISPLAYCONTROL | device->DisplayControl);
}

void lcd_turnOnBlink(struct LiquidCrystalDevice_t *device)
{
	device->DisplayControl |= LCD_BLINKON;
	lcd_sendCommand(device, LCD_DISPLAYCONTROL | device->DisplayControl);
}

void lcd_turnOffBlink(struct LiquidCrystalDevice_t *device)
{
	device->DisplayControl &= ~LCD_BLINKON;
	lcd_sendCommand(device, LCD_DISPLAYCONTROL | device->DisplayControl);
}

void lcd_scrollDisplayLeft(struct LiquidCrystalDevice_t *device)
{
	lcd_sendCommand(device, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void lcd_scrollDisplayRight(struct LiquidCrystalDevice_t *device)
{
	lcd_sendCommand(device, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void lcd_leftToRight(struct LiquidCrystalDevice_t *device)
{
	device->DisplayMode |= LCD_ENTRYLEFT;
	lcd_sendCommand(device, LCD_ENTRYMODESET | device->DisplayMode);
}

void lcd_rightToLeft(struct LiquidCrystalDevice_t *device)
{
	device->DisplayMode &= ~LCD_ENTRYLEFT;
	lcd_sendCommand(device, LCD_ENTRYMODESET | device->DisplayMode);
}

void lcd_turnOnAutoscroll(struct LiquidCrystalDevice_t *device)
{
	device->DisplayMode |= LCD_ENTRYSHIFTINCREMENT;
	lcd_sendCommand(device, LCD_ENTRYMODESET | device->DisplayMode);
}

void lcd_turnOffAutoscroll(struct LiquidCrystalDevice_t *device)
{
	device->DisplayMode &= ~LCD_ENTRYSHIFTINCREMENT;
	lcd_sendCommand(device, LCD_ENTRYMODESET | device->DisplayMode);
}

void lcd_createChar(struct LiquidCrystalDevice_t *device, uint8_t slot, uint8_t charmap[8])
{
	uint8_t i = 0;
	slot &= 0x7; // we only have 8 locations 0-7
	lcd_sendCommand(device, LCD_SETCGRAMADDR | (slot << 3));

	for (i = 0; i < 8; i++)
	{
		lcd_writeDeviceByte(device, charmap[i], LCD_REGISTER_SELECT_BIT);
	}
}

void lcd_sendCommand(LiquidCrystalDevice_t *device, uint8_t command)
{
	lcd_writeDeviceByte(device, command, 0);
}

void lcd_writeDeviceByte(LiquidCrystalDevice_t *device, uint8_t value, uint8_t mode)
{
	uint8_t highnib = value & 0xf0;
	uint8_t lownib = (value << 4) & 0xf0;

	lcd_writeDevice4Bits(device, highnib | mode);
	lcd_writeDevice4Bits(device, lownib | mode);
};

void lcd_writeDevice4Bits(LiquidCrystalDevice_t *device, uint8_t value)
{
	lcd_transmitI2C(device, value);
	lcd_writeDevicePulse(device, value);
};

void lcd_writeDevicePulse(LiquidCrystalDevice_t *device, uint8_t value)
{
	lcd_transmitI2C(device, value | LCD_ENABLE_BIT);
	_delay_us(2);

	lcd_transmitI2C(device, value & ~LCD_ENABLE_BIT);
	_delay_us(50);
};

void lcd_transmitI2C(LiquidCrystalDevice_t *device, uint8_t value)
{
	pcf8574_set_outputs(device->Address, value | device->Backlight);
};