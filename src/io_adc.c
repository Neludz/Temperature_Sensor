
//********************************** ADC ***********************************



#include <drivers/adc.h>
#include <device.h>
#include <zephyr.h>
#include <drivers/uart.h>
#include <logging/log.h>
#include "io_adc.h"
#include "modbus_reg.h"
#include "measure_ntc.h"
LOG_MODULE_REGISTER(io_adc, LOG_LEVEL_INF);
//**************************************************************************
extern uint16_t MBbuf_main[];
const struct device *dev_adc;

#if !DT_NODE_EXISTS(DT_ALIAS(msdz_adcs_t))
#error "No suitable devicetree overlay specified"
#endif

#define ADC_NUM_CHANNELS		DT_PROP_LEN(DT_ALIAS(msdz_adcs_t), msdz_adc_t_channels)
#define ADC_NODE_T				DT_PHANDLE(DT_ALIAS(msdz_adcs_t), msdz_adc_t_id)


/* Common settings supported by most ADCs */
#define ADC_RESOLUTION			12
#define ADC_GAIN				ADC_GAIN_1_4
#define ADC_REFERENCE			ADC_REF_VDD_1_4
#define ADC_ACQUISITION_TIME	ADC_ACQ_TIME_DEFAULT
#define ADC_COUNTS  			(1<<ADC_RESOLUTION)

/* Get the numbers of up to two channels */

#define PHANDLE_TO_DEVICE(node_id, prop, idx) \
     (DT_PROP_BY_IDX(node_id, prop, idx)),

static const uint8_t channel_ids[ADC_NUM_CHANNELS] = {
	DT_FOREACH_PROP_ELEM(DT_ALIAS(msdz_adcs_t), msdz_adc_t_channels, PHANDLE_TO_DEVICE)
};

static const uint8_t channel_seq[ADC_NUM_CHANNELS] = {
	DT_FOREACH_PROP_ELEM(DT_ALIAS(msdz_adcs_t), msdz_adc_t_sequence, PHANDLE_TO_DEVICE)
};
static int16_t sample_buffer[ADC_NUM_CHANNELS*NUMBER_ADC_MEASUREMENTS];

struct adc_channel_cfg channel_cfg = {
	.gain = ADC_GAIN,
	.reference = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	/* channel ID will be overwritten below */
	.channel_id = 0,
	.differential = 0
};

struct adc_sequence_options sequence_options = {
.extra_samplings = NUMBER_ADC_MEASUREMENTS-1,
};

struct adc_sequence sequence = {
	/* individual channels will be added below */
	.channels    = 0,
	.buffer      = sample_buffer,
	/* buffer size in bytes, not number of samples */
	.buffer_size = sizeof(sample_buffer)*NUMBER_ADC_MEASUREMENTS,
	.resolution  = ADC_RESOLUTION,
	.options = &sequence_options,
};

uint16_t ADC_Value_T[ADC_NUM_CHANNELS];
//**************************************************************************

   K_THREAD_DEFINE(io_adc_read_id, STACKSIZE, io_adc_task, NULL, NULL, NULL,
		 K_PRIO_COOP(0) , 0,  10); 

//**************************************************************************
void io_adc_init (void)
{
NTC_Calculation_Data_t NTC;

NTC.NTC_r2=50000;//(MBbuf_main[Reg_NTC_R2_Value_W1]&0xFFFF)|((MBbuf_main[Reg_NTC_R2_Value_W2]&0xFFFF)<<16);
NTC.NTC_r_divider=20000;//(MBbuf_main[Reg_NTC_R_Divider_W1]&0xFFFF)|((MBbuf_main[Reg_NTC_R_Divider_W2]&0xFFFF)<<16);
NTC.NTC_adc_multipler=1;
NTC.NTC_adc_resolution=ADC_COUNTS;
NTC.NTC_b=3950;//(int16_t)MBbuf_main[Reg_NTC_B_Value];
NTC.NTC_t2=25;//(int16_t)MBbuf_main[Reg_NTC_T2_Value];
NTC.NTC_start_temperature=-10;//(int16_t)MBbuf_main[Reg_NTC_Start_Temperature];
NTC.NTC_step_temperature=1;//(int16_t)MBbuf_main[Reg_NTC_Step_Temperature];
NTC.NTC_temper_number_step=136;//MBbuf_main[Reg_NTC_Temper_Number_Step];
calculate_table_NTC(NTC);


	dev_adc = DEVICE_DT_GET(ADC_NODE_T);

	if (!device_is_ready(dev_adc)) {
		LOG_INF("ADC device not found\n");
		return;
	}

	/*
	 * Configure channels individually prior to sampling
	 */
	for (uint8_t i = 0; i < ADC_NUM_CHANNELS; i++) {
		channel_cfg.channel_id = channel_ids[i];
#ifdef CONFIG_ADC_NRFX_SAADC
		channel_cfg.input_positive = SAADC_CH_PSELP_PSELP_AnalogInput0
					     + channel_ids[i];
#endif

		adc_channel_setup(dev_adc, &channel_cfg);

		sequence.channels |= BIT(channel_ids[i]);
	}

}
//**************************************************************************
/**
 * @brief io_getADCval - calculate median value for `nch` channel
 * @param nch - number of channel
 * @return
 */
int16_t io_getADCval(int16_t nch)
{
    int16_t i, addr = nch;
#define PIX_SORT(a,b) { if ((a)>(b)) PIX_SWAP((a),(b)); }
#define PIX_SWAP(a,b) {register int16_t temp=(a);(a)=(b);(b)=temp; }
    int16_t p[9];
    for(i = 0; i < 9; ++i, addr += ADC_NUM_CHANNELS) // first we should prepare array for optmed
        p[i] = sample_buffer[addr];
    PIX_SORT(p[1], p[2]) ; PIX_SORT(p[4], p[5]) ; PIX_SORT(p[7], p[8]) ;
    PIX_SORT(p[0], p[1]) ; PIX_SORT(p[3], p[4]) ; PIX_SORT(p[6], p[7]) ;
    PIX_SORT(p[1], p[2]) ; PIX_SORT(p[4], p[5]) ; PIX_SORT(p[7], p[8]) ;
    PIX_SORT(p[0], p[3]) ; PIX_SORT(p[5], p[8]) ; PIX_SORT(p[4], p[7]) ;
    PIX_SORT(p[3], p[6]) ; PIX_SORT(p[1], p[4]) ; PIX_SORT(p[2], p[5]) ;
    PIX_SORT(p[4], p[7]) ; PIX_SORT(p[4], p[2]) ; PIX_SORT(p[6], p[4]) ;
    PIX_SORT(p[4], p[2]) ;
    return p[4];
#undef PIX_SORT
#undef PIX_SWAP
}

//**************************************************************************
void io_adc_task (void)
{
int16_t Adc_Filter_Value[ADC_NUM_CHANNELS];
int16_t ADC_Val;	
int err, i;
io_adc_init();

err = adc_read(dev_adc, &sequence);
	if (err != 0) 
	{
	LOG_INF("ADC reading failed with error %d.\n", err);
	return;
	}
    for(i = 0; i<ADC_NUM_CHANNELS;i++)
    {
    ADC_Val=(int16_t)io_getADCval(i);
    Adc_Filter_Value[i]=ADC_Val;
	ADC_Value_T[i] = calc_temperature(Adc_Filter_Value[i]);
	MBbuf_main[(channel_seq[i]+Reg_T_0_Channel)] = ADC_Value_T[i];
	}
	k_sleep(K_MSEC(ADC_PAUSE_MS));

    while (1) 
    {	
	err = adc_read(dev_adc, &sequence);
		if (err != 0) 
		{
		LOG_INF("ADC reading failed with error %d.\n", err);
		return;
		}
    	for(i = 0; i<ADC_NUM_CHANNELS;i++)
    	{
    	ADC_Val=(int16_t)io_getADCval(i);
		//LOG_INF("ADC reading %d: %d", (i+1), ADC_Val);
    	Adc_Filter_Value[i]=(Adc_Filter_Value[i]*3+ADC_Val)>>2;
		ADC_Value_T[i] = calc_temperature(Adc_Filter_Value[i]);
    	MBbuf_main[(channel_seq[i]+Reg_T_0_Channel)] = ADC_Value_T[i];
		}
	k_sleep(K_MSEC(ADC_PAUSE_MS));
	}
}

////*********************************** ADC END *******************************