# AVRIOlibraryEIS2020
Repo for IHK Abschlussprüfung Teil 2 Sommer 23

## Instructions:

1. Copy header files and source files in project directory
2. Include io_expander.h and lcd_i2c.h

```C
#include "io_expander.h"
#include "lcd_i2c.h"
```

### 1. Adjust F_FCPU at the top of lcd_i2c.h

```C
/* set CPU speed*/
#ifndef  F_CPU
#define F_CPU 16000000UL
#endif
```

### 2. Adjust I2C bus speed in io_expander.h

```C
/* I2C Setup Options */
#define F_I2C 100000
#define PRESCALER_I2C 1
```

### 3. Code example

Use lcd_demo.c for testing purposes or adjust the snippet according to your setup.
Remember to check I2C address of your port expander as well as LCD size:

```C
LiquidCrystalDevice_t device = lcd_init(<address>, <columns>, <rows>, LCD_5x8DOTS); 

lcd_turnOnBacklight(&device);

lcd_print(&device, "Hello world!");
lcd_setCursor(&device, 1, 0); 
lcd_print(&device, "Sommer 2023");
```