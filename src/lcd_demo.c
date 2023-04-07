#include "lcd_i2c.h"
#include "io_expander.h"

#include <avr/io.h>
#include <util/delay.h>

int main() {

    LiquidCrystalDevice_t device = lq_init(0x27, 16, 2, LCD_5x8DOTS); 

    lq_turnOnBacklight(&device);

    lq_print(&device, "Hello world!");
    lq_setCursor(&device, 1, 0); 
    lq_print(&device, "Sommer 2023");
}