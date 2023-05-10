/* set CPU speed*/
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef _ADC_32_H
#define _ADC_32_H

#define AREF 5.0

void adc_init();
void dummy_conversion();
void adc_channel_select(unsigned char channel);
double adc_readvoltage(unsigned char channel);
int adc_read(unsigned char channel);

#endif
