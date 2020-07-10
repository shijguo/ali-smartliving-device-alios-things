/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _ENERGY_METER_TYPES_H_
#define _ENERGY_METER_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*interrupt_handle_t)(int);
typedef void (*meter_task_t)(int);

// meter measure mode
typedef enum _METER_MODE_T {
    MODE_NORMAL = 0,
    MODE_CALIBRATION,
    MODE_MAX
} meter_mode_t;

/* calibration data */
typedef struct _METER_CAL_T {
    float current_multiplier;
    float voltage_multiplier;
    float power_multiplier;
}meter_cal_t;

// meter measure data
typedef struct _METER_DATA_T {
    float current;
    float voltage;
    float power;
} meter_data_t;

#ifdef __cplusplus
}
#endif

#endif /* end of _ENERGY_METER_TYPES_H_ */
