#include "lcd_i2c.h"
#include "pcf8574.h"

#include <avr/io.h>
#include <util/delay.h>

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

	if (rows > 1) {
		device.DisplayFunction |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotSize != 0) && (rows == 1)) {
		device.DisplayFunction |= LCD_5x10DOTS;
	}

	// according to datasheet, we need at least 40ms after power rises above 2.7V
	_delay_ms(100);
	
	// Now we pull both RS and R/W low to begin commands
	lcd_transmitI2C(&device, LCD_NOBACKLIGHT);	// reset expander and turn backlight off (Bit 8 =1)
	_delay_ms(1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
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

void lcd_print(struct LiquidCrystalDevice_t* device, char * value)
{
	char letter = *value;

	while(letter != 0x00)
	{
		lcd_writeDeviceByte(device, letter, LCD_REGISTER_SELECT_BIT);
		letter = *(++value);
	}
};

void lcd_print_int(struct LiquidCrystalDevice_t* device, int32_t value)
{
	if (value == 0 )
	{
		lcd_print(device, "0");
	}
	else if ((value > INT32_MIN ) && (value <= INT32_MAX))
	{
		//int32_max + sign + null = 12 bytes
		char arr[12] = { '\0' };
		
		//Convert integer to array (returns in reversed order)
		int2bcd(value, arr);
		
		//Print
		lcd_print(device, arr);
	}
}

void lcd_turnOnBacklight(struct LiquidCrystalDevice_t* device)
{
	device->Backlight = LCD_BACKLIGHT;
	lcd_transmitI2C(device, 0);
}

void lcd_turnOffBacklight(struct LiquidCrystalDevice_t* device)
{
	device->Backlight = LCD_NOBACKLIGHT;
	lcd_transmitI2C(device, 0);
}

void lcd_clear(LiquidCrystalDevice_t* device)
{
	lcd_sendCommand(device, LCD_CLEARDISPLAY); // clear display, set cursor position to zero
	_delay_us(2000);  // this command takes a long time!

	lcd_setCursor(device, 0, 0);
}

void lcd_setCursor(LiquidCrystalDevice_t* device, uint8_t row, uint8_t column)
{
	uint8_t row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };

	lcd_sendCommand(device, LCD_SETDDRAMADDR | (column + row_offsets[row]));
}

void lcd_returnHome(LiquidCrystalDevice_t* device)
{
	lcd_sendCommand(device, LCD_RETURNHOME);  // set cursor position to zero
	_delay_us(2000);  // this command takes a long time!
};

void lcd_turnOnDisplay(LiquidCrystalDevice_t* device)
{
	device->DisplayControl |= LCD_DISPLAYON;
	lcd_sendCommand(device, LCD_DISPLAYCONTROL | device->DisplayControl);
};

void lcd_turnOffDisplay(LiquidCrystalDevice_t* device)
{
	device->DisplayControl &= ~LCD_DISPLAYON;
	lcd_sendCommand(device, LCD_DISPLAYCONTROL | device->DisplayControl);
};

void lcd_turnOnCursor(struct LiquidCrystalDevice_t* device)
{
	device->DisplayControl |= LCD_CURSORON;
	lcd_sendCommand(device, LCD_DISPLAYCONTROL | device->DisplayControl);
}

void lcd_turnOffCursor(struct LiquidCrystalDevice_t* device)
{
	device->DisplayControl &= ~LCD_CURSORON;
	lcd_sendCommand(device, LCD_DISPLAYCONTROL | device->DisplayControl);
}

void lcd_turnOnBlink(struct LiquidCrystalDevice_t* device)
{
	device->DisplayControl |= LCD_BLINKON;
	lcd_sendCommand(device, LCD_DISPLAYCONTROL | device->DisplayControl);
}

void lcd_turnOffBlink(struct LiquidCrystalDevice_t* device)
{
	device->DisplayControl &= ~LCD_BLINKON;
	lcd_sendCommand(device, LCD_DISPLAYCONTROL | device->DisplayControl);
}

void lcd_scrollDisplayLeft(struct LiquidCrystalDevice_t* device)
{
	lcd_sendCommand(device, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void lcd_scrollDisplayRight(struct LiquidCrystalDevice_t* device)
{
	lcd_sendCommand(device, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void lcd_leftToRight(struct LiquidCrystalDevice_t* device)
{
	device->DisplayMode |= LCD_ENTRYLEFT;
	lcd_sendCommand(device, LCD_ENTRYMODESET | device->DisplayMode);
}

void lcd_rightToLeft(struct LiquidCrystalDevice_t* device)
{
	device->DisplayMode &= ~LCD_ENTRYLEFT;
	lcd_sendCommand(device, LCD_ENTRYMODESET | device->DisplayMode);
}

void lcd_turnOnAutoscroll(struct LiquidCrystalDevice_t* device)
{
	device->DisplayMode |= LCD_ENTRYSHIFTINCREMENT;
	lcd_sendCommand(device, LCD_ENTRYMODESET | device->DisplayMode);
}

void lcd_turnOffAutoscroll(struct LiquidCrystalDevice_t* device)
{
	device->DisplayMode &= ~LCD_ENTRYSHIFTINCREMENT;
	lcd_sendCommand(device, LCD_ENTRYMODESET | device->DisplayMode);
}

void lcd_createChar(struct LiquidCrystalDevice_t* device, uint8_t slot, uint8_t charmap[8])
{
	uint8_t i = 0;
	slot &= 0x7; // we only have 8 locations 0-7
	lcd_sendCommand(device, LCD_SETCGRAMADDR | (slot << 3));

	for (i = 0; i < 8; i++) 
	{
		lcd_writeDeviceByte(device, charmap[i], LCD_REGISTER_SELECT_BIT);
	}
}


void lcd_sendCommand(LiquidCrystalDevice_t* device, uint8_t command)
{
	lcd_writeDeviceByte(device, command, 0);
}

void lcd_writeDeviceByte(LiquidCrystalDevice_t* device, uint8_t value, uint8_t mode)
{
	uint8_t highnib= value & 0xf0;
	uint8_t lownib= (value<<4) & 0xf0;

	lcd_writeDevice4Bits(device, highnib | mode);
	lcd_writeDevice4Bits(device, lownib | mode);
};

void lcd_writeDevice4Bits(LiquidCrystalDevice_t* device, uint8_t value)
{
	lcd_transmitI2C(device, value);
	lcd_writeDevicePulse(device, value);
};

void lcd_writeDevicePulse(LiquidCrystalDevice_t* device, uint8_t value)
{
	lcd_transmitI2C(device, value | LCD_ENABLE_BIT);
	_delay_us(2);

	lcd_transmitI2C(device, value & ~LCD_ENABLE_BIT);
	_delay_us(50);
};

void lcd_transmitI2C(LiquidCrystalDevice_t* device, uint8_t value)
{
	pcf8574_set_outputs(device->Address, value | device->Backlight);
};

void int2bcd(int32_t value, char bcd[])
{
	uint8_t isNegative = 0;
	
	bcd[0] = bcd[1] = bcd[2] =
	bcd[3] = bcd[4] = bcd[5] =
	bcd[6] = bcd[7] = bcd[8] =
	bcd[9] = bcd[10] = '0';
	
	if (value < 0)
	{
		isNegative = 1;
		value = -value;
	}
	
	while (value > 1000000000)
	{
		value -= 1000000000;
		bcd[1]++;
	}
	
	while (value >= 100000000)
	{
		value -= 100000000;
		bcd[2]++;
	}
		
	while (value >= 10000000)
	{
		value -= 10000000;
		bcd[3]++;
	}
	
	while (value >= 1000000)
	{
		value -= 1000000;
		bcd[4]++;
	}
	
	while (value >= 100000)
	{
		value -= 100000;
		bcd[5]++;
	}

	while (value >= 10000)
	{
		value -= 10000;
		bcd[6]++;
	}

	while (value >= 1000)
	{
		value -= 1000;
		bcd[7]++;
	}
	
	while (value >= 100)
	{
		value -= 100;
		bcd[8]++;
	}
	
	while (value >= 10)
	{
		value -= 10;
		bcd[9]++;
	}

	while (value >= 1)
	{
		value -= 1;
		bcd[10]++;
	}

	uint8_t i = 0;
	//Find first non zero digit
	while (bcd[i] == '0')
		i++;

	//Add sign 
	if (isNegative)
	{
		i--;
		bcd[i] = '-';
	}

	//Shift array
	uint8_t end = 10 - i;
	uint8_t offset = i;
	i = 0;
	while (i <= end)
	{
		bcd[i] = bcd[i + offset];
		i++;
	}
	bcd[i] = '\0';
}