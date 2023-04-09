#include "adc32.h"
#include <avr/io.h>

void adc_init() {

    ADCSRA |= 0x00;	//Prescaler 1
    ADMUX |= 0x00; //Set AREF as Vref
    ADCSRA |= (1<<ADEN); //enable ADC
    ADCSRA |= (1<<ADSC);    //start dummy conversion
	while((ADCSRA & (1<<ADIF)) == 0); //Wait until conversion is done

    /* We do this once because the first conversion is inconsistent*/
    unsigned char discard;
    discard = ADCL;
    discard = ADCH;
}

void adc_channel_select(unsigned char channel)
{
	ADMUX &= 0xC0; // clear MUX4:MUX0 and ADLAR bit for initialization
	
	switch(channel)
	{
		case 0:
			ADMUX |= 0x00;
			break;
		case 1:
			ADMUX |= (1<<MUX0);
			break;
		case 2:
			ADMUX |= (1<<MUX1);
			break;
		case 3:
			ADMUX |= (1<<MUX0) | (1<<MUX1);
			break;
		case 4:
			ADMUX |= (1<<MUX2);
			break;
		case 5:
			ADMUX |= (1<<MUX0) | (1<<MUX2);
			break;
		case 6:
			ADMUX |= (1<<MUX1) | (1<<MUX2);
			break;
		case 7:
			ADMUX |= (1<<MUX0) | (1<<MUX1) | (1<<MUX2);
			break;
		default:
			ADMUX |= (0x1F & channel);
	}
	return;
}

double adc_readvoltage(unsigned char channel) 
{
	//get the raw input
	int analogReading = adc_read(channel);
	return analogReading/1024*AREF;
}

unsigned int adc_read(unsigned char channel) 
{
	int analogReading = 0;
	unsigned char LSB;
	unsigned char MSB;
	adc_channel_select(channel);
	ADCSRA |= (1<<ADSC);    //start conversion
	while((ADCSRA & (1<<ADIF)) == 0); //Wait until conversion is done
	LSB = ADCL;
	MSB = ADCH;
	analogReading |= MSB;
	analogReading <<= 8;
	analogReading |= LSB;
	return analogReading;	
}
