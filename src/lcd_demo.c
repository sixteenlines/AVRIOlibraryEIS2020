#include "lcd_i2c.h"
#include "adc32.h"
#include "mcp4725.h"
#include "pcf8574.h"
#include "util/delay.h"

int main()
{
    mcp4725_setvoltage_fastmode(0x66, 1.2);
    adc_init();
    LiquidCrystalDevice_t device = lcd_init(0x3A, 16, 2, LCD_5x8DOTS);

    while (1)
    {
        _delay_ms(100);
        lcd_returnHome(&device);
        pcf8574_set_outputs(0x21, (~(adc_read(0) / 4)));
        lcd_print(&device, "Test: ");
        lcd_printDouble(&device, adc_readvoltage(0), 100);
        lcd_printChar(&device, 'V');
    }

    return 0;
}