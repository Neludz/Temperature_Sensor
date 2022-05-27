#ifndef IO_ADC_H_INCLUDED
#define IO_ADC_H_INCLUDED

#define IO_STACKSIZE                2048
#define IO_PRIORITY                 K_PRIO_COOP(0)
#define NUMBER_ADC_MEASUREMENTS     9
#define ADC_PAUSE_MS                1000
  
/* Common settings supported by most ADCs */
#define ADC_RESOLUTION			12
#define ADC_GAIN				ADC_GAIN_1_4
#define ADC_REFERENCE			ADC_REF_VDD_1_4
#define ADC_ACQUISITION_TIME	ADC_ACQ_TIME_DEFAULT
#define ADC_COUNTS  			(1<<ADC_RESOLUTION)


void io_adc_init (void);
void io_adc_task (void *arg1, void *arg2, void *arg3);
int16_t io_getADCval(int16_t nch);
#endif /* IO_ADC_H_INCLUDED */
