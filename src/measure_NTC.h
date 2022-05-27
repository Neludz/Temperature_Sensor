/*
 * Measure_NTC.h
 *
 *  Created on: 27 мая 2019 г.
 *      Author: a.litvin
 */

#ifndef MEASURE_NTC_H_INCLUDED
#define MEASURE_NTC_H_INCLUDED

#include <stdint.h>
#include <string.h>
#include "math.h"
#include "main.h"

//-----------------------------------------------------------------------
// define
//-----------------------------------------------------------------------
// Значение температуры, возвращаемое если сумма результатов АЦП больше первого значения таблицы
#define TEMPERATURE_UNDER 		        0x7001
// Значение температуры, возвращаемое если сумма результатов АЦП меньше последнего значения таблицы
#define TEMPERATURE_OVER 		        0x7002
#define TEMPERATURE_NO_MEASURE 	        0x7004
#define TEMPERATURE_ERROR_LABEL	        0x7000
#define TEMPERATURE_ERROR_MASK          0xF000

#define TEMPERATURE_TABLE_SIZE  		SIZE_NTC_TABLE
#define TEMPERATURE_TABLE_READ(i) 		(termo_table[i])


typedef uint16_t temperature_table_entry_type;
typedef uint8_t temperature_table_index_type;

typedef struct
{
    uint32_t            NTC_r2;						//sensor data
    int16_t             NTC_b;						//sensor data
    int16_t             NTC_t2;						//sensor data
    uint32_t            NTC_r_divider;				//R in device
    int16_t             NTC_start_temperature;
    int16_t             NTC_step_temperature;
    uint16_t            NTC_temper_number_step;
    uint16_t            NTC_adc_resolution;
    uint16_t            NTC_adc_multipler;
} NTC_Calculation_Data_t;

//-----------------------------------------------------------------------
// prototype
//-----------------------------------------------------------------------
int16_t calc_temperature(temperature_table_entry_type adcsum) ;
void calculate_table_NTC (NTC_Calculation_Data_t NTC_Data);

#endif /* MEASURE_NTC_H_INCLUDED */
