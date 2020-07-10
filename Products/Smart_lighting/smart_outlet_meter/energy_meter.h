/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _ENERGY_METER_H_
#define _ENERGY_METER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "energy_meter_types.h"

void energy_meter_start(void);
void set_meter_mode(meter_mode_t mode);
meter_mode_t get_meter_mode(void);
int get_meter_data(meter_data_t* p_data);
int energy_meter_get_version(char* str_ver, int length);

#ifdef __cplusplus
}
#endif

#endif /* end of _ENERGY_METER_H_ */
