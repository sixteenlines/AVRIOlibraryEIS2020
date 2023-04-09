#ifndef _ADC_32_H
#define _ADC_32_H

#define AREF 5.0

void adc_init();
void adc_channel_select(unsigned char channel);
double adc_readvoltage(unsigned char channel);
unsigned int adc_read(unsigned char channel);

#endif
