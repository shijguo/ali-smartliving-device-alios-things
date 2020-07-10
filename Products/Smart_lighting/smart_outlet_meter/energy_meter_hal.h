/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _ENERGY_METER_HAL_H_
#define _ENERGY_METER_HAL_H_

#include <stdio.h>
#include <aos/aos.h>
#include "hal/soc/timer.h"
#include "hal/soc/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "energy_meter_types.h"

/* debug config */
#define METER_DEBUG     1

/* hardware config */

#define HLW8012			0
#define BL0937			1

#define METER_CHIP_TYPE		HLW8012

#if (METER_CHIP_TYPE == HLW8012)
#define SEL_PIN                         23
#define CF1_PIN                         24
#define CF_PIN                          25

#define USE_INTERRUPT

#define HIGH                1
#define LOW                 0

// Internal voltage reference value
#define V_REF               2.43

// Frequency of the HLW8012 internal clock
#define F_OSC               3579000.0

// Set SEL_PIN to HIGH to sample current
// This is the case for Itead's Sonoff POW, where a
// the SEL_PIN drives a transistor that pulls down
// the SEL pin in the HLW8012 when closed
#define CURRENT_MODE_SEL                HIGH

// These are the nominal values for the resistors in the circuit
#define CURRENT_RESISTOR                0.002
#define VOLTAGE_RESISTOR_UPSTREAM       ( 2 * 1000000.0 ) //2 x 1M
#define VOLTAGE_RESISTOR_DOWNSTREAM     ( 1000.0 ) //1k
// Calibrate using a 220V/40W bulb (pure resistive) on a 220V line
#define OUTLET_POWER        1.75
#define CAL_POWER           (38.28 + OUTLET_POWER)
#define CAL_VOLTAGE         220.0
#define CAL_CURRENT         CAL_POWER/CAL_VOLTAGE

#elif (METER_CHIP_TYPE == BL0937)
#define SEL_PIN                         6
#define CF1_PIN                         10
#define CF_PIN                          14

#define USE_INTERRUPT

#define HIGH                1
#define LOW                 0

// Internal voltage reference value
#define V_REF               1.218

// Frequency of the HLW8012 internal clock
#define F_OSC               3579000.0

// Set SEL_PIN to LOW to sample current
#define CURRENT_MODE_SEL                LOW

// These are the nominal values for the resistors in the circuit
#define CURRENT_RESISTOR                0.001
#define VOLTAGE_RESISTOR_UPSTREAM       ( 2 * 1000000.0 ) //2 x 1M
#define VOLTAGE_RESISTOR_DOWNSTREAM     ( 1000.0 ) //1k
// Calibrate using a 220V/40W bulb (pure resistive) on a 110V line
#define OUTLET_POWER        0
#define CAL_POWER           (13.67 + OUTLET_POWER)
#define CAL_VOLTAGE         110.0
#define CAL_CURRENT         CAL_POWER/CAL_VOLTAGE

#else
#error "meter chip not defined or not supported"
#endif

#define METER_TIMER_ACC                 10  /* timer accuracy in milliseconds */

// Check values every 10 seconds
#define MEASURE_DURATION_TIME           10000

// The factor of a 2mOhm resistor
// as per recomended circuit in datasheet
// A 2mOhm resistor allows a ~15A max measurement
#define R_CURRENT           CURRENT_RESISTOR

// This is the factor of a voltage divider of 2x 1M upstream and 1K downstream
// 6x 470K upstream and 1k downstream recomended circuit in datasheet
#define R_VOLTAGE           (VOLTAGE_RESISTOR_UPSTREAM + VOLTAGE_RESISTOR_DOWNSTREAM)/VOLTAGE_RESISTOR_DOWNSTREAM

// Minimum delay between selecting a mode and reading a sample
#define READING_INTERVAL    3000

// Maximum pulse with in microseconds
// If longer than this pulse width is reset to 0
// This value is purely experimental.
// Higher values allow for a better precission but reduce sampling rate
// and response speed to change
// Lower values increase sampling rate but reduce precission
// Values below 0.5s are not recommended since current and voltage output
// will have no time to stabilise
#define PULSE_TIMEOUT       2000000/1000

#if METER_DEBUG
#define METER_LOG(...)                               \
    do {                                                     \
        HAL_Printf("%s.%d: ", __func__, __LINE__);  \
        HAL_Printf(__VA_ARGS__);                                 \
        HAL_Printf("\r\n");                                   \
    } while (0)
#else
#define METER_LOG(...)
#endif

int energy_meter_hal_init(interrupt_handle_t cf_handle, interrupt_handle_t cf1_handle, interrupt_handle_t timer_handle);

void energy_meter_sel_set(int value);

void energy_meter_cal_pre(void);

int energy_meter_cal_done(meter_cal_t *meter_cal_data);

void energy_meter_cal_post(void);

uint32_t meter_get_pulse_timeout(void);

uint32_t meter_get_timer_acc(void);

uint32_t meter_get_current_mode_sel(void);

float meter_get_cal_voltage(void);

float meter_get_cal_current(void);

float meter_get_cal_power(void);

int meter_get_default_multipliers(meter_cal_t *meter_cal_data);

int energy_meter_cal_load(meter_cal_t *meter_cal_data);

void energy_meter_msleep(uint32_t mseconds);

int energy_meter_register_task(meter_task_t meter_task, void *args);

#ifdef __cplusplus
}
#endif

#endif /* end of _ENERGY_METER_HAL_H_ */
