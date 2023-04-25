#include "lcd_i2c.h"

int main()
{

    LiquidCrystalDevice_t device = lcd_init(0x3A, 16, 2, LCD_5x8DOTS);

    lcd_turnOnBacklight(&device);
    lcd_print(&device, "Temp:");
    lcd_setCursor(&device, 0, 9);
    lcd_printDouble(&device, 20.5, 10);
    lcd_printChar(&device, DEGREE);
    lcd_print(&device, "C");
    lcd_setCursor(&device, 1, 0);
    lcd_print(&device, "Humid:");
    lcd_setCursor(&device, 1, 9);
    lcd_printInt(&device, 45);
    lcd_printChar(&device, PERCENT);

    while (1)
        ;

    return 0;
}