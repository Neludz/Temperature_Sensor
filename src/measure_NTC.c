/*
 * Measure_NTC.c
 *
 *  Created on: 27 мая 2019 г.
 *      Author: a.litvin
 */

//======================Include========================================
#include <measure_NTC.h>

/*
  NTCCALUG02A502G; ADC 12bit:
     R1(T1): 5кОм(25°С)
     R2(T2): 0.5331кОм(85°С)
     Ra: 5.1кОм
     Напряжения U0/Uref: 3.3В/3.3В
    диапазон -10..125, шаг 1
*/

//======================Variable========================================
static temperature_table_entry_type termo_table[TEMPERATURE_TABLE_SIZE];
NTC_Calculation_Data_t NTC_Parametr;
//test=20000*exp(3977/263.15-3977/298.15);
//test_2=(8*test*1024)/((20000+test));

//======================Function========================================



void calculate_table_NTC (NTC_Calculation_Data_t NTC_Data)
{
uint32_t i;
float  r_Value, Temp_t1, Temp_t2;
memcpy(&NTC_Parametr, &NTC_Data, sizeof(NTC_Calculation_Data_t));
Temp_t2=(float) NTC_Parametr.NTC_t2+273.15;

    if (NTC_Parametr.NTC_temper_number_step>TEMPERATURE_TABLE_SIZE)
    {
    NTC_Parametr.NTC_temper_number_step = TEMPERATURE_TABLE_SIZE;
    }

    for (i=0; i <  NTC_Parametr.NTC_temper_number_step; i++)
    {
    Temp_t1=(((float)NTC_Parametr.NTC_start_temperature+(float)i*(float)NTC_Parametr.NTC_step_temperature))+273.15;
    r_Value=((float)NTC_Parametr.NTC_r2*exp(((float)NTC_Parametr.NTC_b/Temp_t1)-((float)NTC_Parametr.NTC_b/Temp_t2)));
    termo_table[i] = (temperature_table_entry_type)((r_Value*(float)NTC_Parametr.NTC_adc_multipler*(float)NTC_Parametr.NTC_adc_resolution*2+r_Value+(float)NTC_Parametr.NTC_r_divider)/
                     (2*(r_Value + (float)NTC_Parametr.NTC_r_divider)));
    }
}


int16_t calc_temperature(temperature_table_entry_type adcsum) {
  temperature_table_index_type l = 0;
  temperature_table_index_type r = ( NTC_Parametr.NTC_temper_number_step) - 1;
  temperature_table_entry_type thigh = TEMPERATURE_TABLE_READ(r);
  int32_t TEMPERATURE_TABLE_START_RATE;
  int32_t TEMPERATURE_TABLE_STEP_RATE;

  TEMPERATURE_TABLE_START_RATE = NTC_Parametr.NTC_start_temperature;
  TEMPERATURE_TABLE_STEP_RATE = NTC_Parametr.NTC_step_temperature;

  if (adcsum <= thigh) {
        if (adcsum < thigh)
        {
        return TEMPERATURE_OVER;
        }
    return (TEMPERATURE_TABLE_STEP_RATE * r + TEMPERATURE_TABLE_START_RATE);
  }
  temperature_table_entry_type tlow = TEMPERATURE_TABLE_READ(0);
  if (adcsum >= tlow) {

      if (adcsum > tlow)
        return TEMPERATURE_UNDER;

    return TEMPERATURE_TABLE_START_RATE;
  }

  while ((r - l) > 1) {
    temperature_table_index_type m = (l + r) >> 1;
    temperature_table_entry_type mid = TEMPERATURE_TABLE_READ(m);
    if (adcsum > mid) {
      r = m;
    } else {
      l = m;
    }
  }
  temperature_table_entry_type vl = TEMPERATURE_TABLE_READ(l);
  if (adcsum >= vl) {
    return (l * TEMPERATURE_TABLE_STEP_RATE + TEMPERATURE_TABLE_START_RATE);
  }
  temperature_table_entry_type vr = TEMPERATURE_TABLE_READ(r);
  temperature_table_entry_type vd = vl - vr;
  int16_t res = TEMPERATURE_TABLE_START_RATE + r * TEMPERATURE_TABLE_STEP_RATE;
  if (vd) {

    res -= ((TEMPERATURE_TABLE_STEP_RATE * (int32_t)(adcsum - vr) + (vd >> 1)) / vd);
  }
  return res;
}


