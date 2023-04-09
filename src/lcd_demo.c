#include "hd44780.h"

int main() {
    
    lcd_init();
    lcd_print("Temp:");
    lcd_setCursor(0, 9);
    lcd_printDouble(20.5, 10);
    lcd_printChar(DEGREE);
    lcd_print("C");
    lcd_setCursor(1, 0);
    lcd_print("Humid:");
    lcd_setCursor(1, 9);
    lcd_printInt(45);
    lcd_printChar(PERCENT);

    while(1);
    
    return 0;
}