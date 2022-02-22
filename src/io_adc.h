#ifndef IO_ADC_H_INCLUDED
#define IO_ADC_H_INCLUDED
#include <main.h>

#define STACKSIZE                     2048
#define NUMBER_ADC_MEASUREMENTS       9
#define ADC_PAUSE_MS                  ADC_PAUSE_MS_MAIN_H
  
void io_adc_init (void);
void io_adc_task (void);
int16_t io_getADCval(int16_t nch);
#endif /* IO_ADC_H_INCLUDED */
