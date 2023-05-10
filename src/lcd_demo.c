#include "lcd_i2c.h"
#include "adc32.h"
#include "mcp4725.h"

int main()
{
    mcp4725_setvoltage_fastmode(0x66, 1.2);
    adc_init();
    LiquidCrystalDevice_t device = lcd_init(0x3A, 16, 2, LCD_5x8DOTS);

    lcd_turnOnBacklight(&device);
    lcd_print(&device, "Test: ");
    lcd_printDouble(&device, adc_readvoltage(3), 100);
    lcd_printChar(&device, 'V');

    while (1)
        ;

    return 0;
}