/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include "energy_meter_hal.h"

#define METER_CAL_KEY                   "METER_CAL"

/* meter cf1 select */
static gpio_dev_t meter_sel_gpio;

static aos_timer_t  meter_timer;

extern void product_set_switch(bool state);

void energy_meter_sel_set(int value)
{
    /* SEL */
    if(value) {
        hal_gpio_output_high(&meter_sel_gpio);
    } else {
        hal_gpio_output_low(&meter_sel_gpio);
    }
}

void energy_meter_cal_pre(void)
{
    product_set_switch(true);
}

uint32_t meter_get_pulse_timeout(void)
{
    return PULSE_TIMEOUT;
}

uint32_t meter_get_timer_acc(void)
{
    return METER_TIMER_ACC;
}

uint32_t meter_get_current_mode_sel(void)
{
    return CURRENT_MODE_SEL;
}

float meter_get_cal_voltage(void)
{
    return CAL_VOLTAGE;
}

float meter_get_cal_current(void)
{
    return CAL_CURRENT;
}

float meter_get_cal_power(void)
{
    return CAL_POWER;
}

int meter_get_default_multipliers(meter_cal_t *meter_cal_data)
{
    if (NULL == meter_cal_data) {
        return -1;
    }

#if (METER_CHIP_TYPE == HLW8012)
    meter_cal_data->current_multiplier = ( 1000.0 * 512 * V_REF / R_CURRENT / 24.0 / F_OSC );
    meter_cal_data->voltage_multiplier = ( 1000.0 * 512 * V_REF * R_VOLTAGE / 2.0 / F_OSC );
    meter_cal_data->power_multiplier   = ( 1000.0 * 128 * V_REF * V_REF * R_VOLTAGE / R_CURRENT / 48.0 / F_OSC );
#elif (METER_CHIP_TYPE == BL0937)
    meter_cal_data->current_multiplier =  ( 531500000.0 * V_REF / R_CURRENT / 24.0 / F_OSC ) / 1.166666f; // 
    meter_cal_data->voltage_multiplier =  ( 221380000.0 * V_REF * R_VOLTAGE /  2.0 / F_OSC ) / 1.0474137931f; //221384120,171674
    meter_cal_data->power_multiplier   =  (  50850000.0 * V_REF * V_REF * R_VOLTAGE / R_CURRENT / 48.0 / F_OSC) / 1.1371681416f;  //15102450
#else
#error "meter chip undefined or not supported!"
#endif
    return 0;
}

int energy_meter_cal_done(meter_cal_t *meter_cal_data)
{
    int len = sizeof(meter_cal_t);
    if (NULL == meter_cal_data) {
        return -1;
    }
    int res = aos_kv_set(METER_CAL_KEY, meter_cal_data, len, 1);
    return res;
}

void energy_meter_cal_post(void)
{
    product_set_switch(false);
}

int energy_meter_cal_load(meter_cal_t *meter_cal_data)
{
    int len = sizeof(meter_cal_t);
    if (NULL == meter_cal_data) {
        return -1;
    }
    /* try to read calibration data from flash */
    if (0 != aos_kv_get(METER_CAL_KEY, meter_cal_data, &len)) {
        return -2;
    }
    return 0;
}

void energy_meter_msleep(uint32_t mseconds)
{
    aos_msleep(mseconds);
}

int energy_meter_hal_init(interrupt_handle_t cf_handle, interrupt_handle_t cf1_handle, interrupt_handle_t timer_handle)
{
    gpio_dev_t meter_cfx_gpio;
    /* VI_CF */
    meter_cfx_gpio.port = CF_PIN;
    meter_cfx_gpio.config = INPUT_PULL_UP;
    hal_gpio_init(&meter_cfx_gpio);
#if defined (USE_INTERRUPT)
    hal_gpio_enable_irq(&meter_cfx_gpio, IRQ_TRIGGER_RISING_EDGE, cf_handle, NULL);
#endif
    /* P_CF */
    meter_cfx_gpio.port = CF1_PIN;
    meter_cfx_gpio.config = INPUT_PULL_UP;
    hal_gpio_init(&meter_cfx_gpio);
#if defined (USE_INTERRUPT)
    hal_gpio_enable_irq(&meter_cfx_gpio, IRQ_TRIGGER_RISING_EDGE, cf1_handle, NULL);
#endif
    /* SEL */
    meter_sel_gpio.port = SEL_PIN;
    meter_sel_gpio.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&meter_sel_gpio);
    energy_meter_sel_set(CURRENT_MODE_SEL);
    aos_timer_new(&meter_timer, timer_handle, NULL, METER_TIMER_ACC, 1);
    return 0;
}

int energy_meter_register_task(meter_task_t meter_task, void *args)
{
    aos_task_new("meter task", meter_task, args, 4096);
    return 0;
}
