#include "lcd_i2c.h"
#include "adc32.h"
#include "mcp4725.h"

int main()
{

    LiquidCrystalDevice_t device = lcd_init(0x3A, 16, 2, LCD_5x8DOTS);

    lcd_turnOnBacklight(&device);
    lcd_print(&device, "Test: ");
    adc_init();
    lcd_printInt(&device, adc_read(3));

    while (1)
        ;

    return 0;
}