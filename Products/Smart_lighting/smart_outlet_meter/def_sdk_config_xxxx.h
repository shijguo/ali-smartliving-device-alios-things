#ifndef DEF_SDK_CONFIG_XXXX_H_
#define DEF_SDK_CONFIG_XXXX_H_

#include "sdk_config_xxxx.h"

//     [0 ] = PA_14,
//     [1 ] = PA_15,
//     [2 ] = PA_22,
//     [3 ] = PA_19,
//     [4 ] = PA_23,
//     [5 ] = PA_18,
//     [6 ] = PA_0,
//     [7 ] = PA_12,
//     [8 ] = PA_5,
//     [10] = PA_30,
//     [11] = PA_29,

#define DEV_NET_TRIGGER_MODE_1 NET_TRIGGER_MODE_1

#if(SWITCH_1 == 14)
#define DEV_SWITCH_PORT 0
#elif(SWITCH_1 == 15)
#define DEV_SWITCH_PORT 1
#elif(SWITCH_1 == 22)
#define DEV_SWITCH_PORT 2
#elif(SWITCH_1 == 19)
#define DEV_SWITCH_PORT 3
#elif(SWITCH_1 == 23)
#define DEV_SWITCH_PORT 4
#elif(SWITCH_1 == 18)
#define DEV_SWITCH_PORT 5
#elif(SWITCH_1 == 0)
#define DEV_SWITCH_PORT 6
#elif(SWITCH_1 == 12)
#define DEV_SWITCH_PORT 7
#elif(SWITCH_1 == 5)
#define DEV_SWITCH_PORT 8
#elif(SWITCH_1 == 30)
#define DEV_SWITCH_PORT 10
#elif(SWITCH_1 == 29)
#define DEV_SWITCH_PORT 11
#endif


#if(RELAY_1 == 14)
#define DEV_RELAY_1_PORT 0
#elif(RELAY_1 == 15)
#define DEV_RELAY_1_PORT 1
#elif(RELAY_1 == 22)
#define DEV_RELAY_1_PORT 2
#elif(RELAY_1 == 19)
#define DEV_RELAY_1_PORT 3
#elif(RELAY_1 == 23)
#define DEV_RELAY_1_PORT 4
#elif(RELAY_1 == 18)
#define DEV_RELAY_1_PORT 5
#elif(RELAY_1 == 0)
#define DEV_RELAY_1_PORT 6
#elif(RELAY_1 == 12)
#define DEV_RELAY_1_PORT 7
#elif(RELAY_1 == 5)
#define DEV_RELAY_1_PORT 8
#elif(RELAY_1 == 30)
#define DEV_RELAY_1_PORT 10
#elif(RELAY_1 == 29)
#define DEV_RELAY_1_PORT 11
#endif

#if(WIFI_INDICATOR == 14)
#define DEV_WIFI_INDICATOR_PORT 0
#elif(WIFI_INDICATOR == 15)
#define DEV_WIFI_INDICATOR_PORT 1
#elif(WIFI_INDICATOR == 22)
#define DEV_WIFI_INDICATOR_PORT 2
#elif(WIFI_INDICATOR == 19)
#define DEV_WIFI_INDICATOR_PORT 3
#elif(WIFI_INDICATOR == 23)
#define DEV_WIFI_INDICATOR_PORT 4
#elif(WIFI_INDICATOR == 18)
#define DEV_WIFI_INDICATOR_PORT 5
#elif(WIFI_INDICATOR == 0)
#define DEV_WIFI_INDICATOR_PORT 6
#elif(WIFI_INDICATOR == 12)
#define DEV_WIFI_INDICATOR_PORT 7
#elif(WIFI_INDICATOR == 5)
#define DEV_WIFI_INDICATOR_PORT 8
#elif(WIFI_INDICATOR == 30)
#define DEV_WIFI_INDICATOR_PORT 10
#elif(WIFI_INDICATOR == 29)
#define DEV_WIFI_INDICATOR_PORT 11
#endif


#endif /* DEF_SDK_CONFIG_XXXX_H_ */
