#ifndef SDK_CONFIG_XXXX_H_
#define SDK_CONFIG_XXXX_H_

#define PRODUCT_MODEL		"{#$PRODUCT_MODEL#}"


/**********************
 * @ 开关按键1
 *********************/
#define SWITCH_1				{#$SWITCH_1#}
/**********************
 * @ 开关按键1使能电平
 *********************/
#define SWITCH_ENABLE_1		{#$SWITCH_ENABLE_1#}

/**********************
 * @ 默认通电状态1
 * 0：断电记忆
 * 1：通电
 * 2：断电
 *********************/
#define DEF_POWER_STATUS_1	{#$DEF_POWER_STATUS_1#}

/**********************
 * @ 继电器1
 *********************/
#define RELAY_1				{#$RELAY_1#}
/**********************
 * @ 继电器1使能电平
 *********************/
#define RELAY_ENABLE_1		{#$RELAY_ENABLE_1#}

/**********************
 * @ 电源指示灯1
 *********************/
#define RELAY_INDICATOR_1				{#$RELAY_INDICATOR_1#}
/**********************
 * @ 电源指示灯1使能电平
 *********************/
#define RELAY_INDICATOR_ENABLE_1		{#$RELAY_INDICATOR_ENABLE_1#}

/**********************
 * @ WiFi指示灯
 *********************/
#define WIFI_INDICATOR				{#$WIFI_INDICATOR#}
/**********************
 * @ WiFi指示灯使能电平
 *********************/
#define WIFI_INDICATOR_ENABLE		{#$WIFI_INDICATOR_ENABLE#}

/**********************
 * @ 复位长按秒数（恢复出厂设置）
 *********************/
#define FACTORY_RESTORE				{#$FACTORY_RESTORE#}

/**********************
 * @ 已联网时WiFi灯状态
 * 0：指示继电器状态
 * 1：亮度
 * 2：灭灯
 *********************/
#define WIFI_INDICATOR_CONNECTED_STATUS		{#$WIFI_INDICATOR_CONNECTED_STATUS#}
/**********************
 * @ 未联网时WiFi灯状态
 * 0：指示继电器状态
 * 1：亮灯
 * 2：灭灯
 *********************/
#define WIFI_INDICATOR_DISCONNECTED_STATUS	{#$WIFI_INDICATOR_DISCONNECTED_STATUS#}

/**********************
 * @ 首次配网触发方式
 * 0：长按至网络灯闪烁
 * 1：上电网络灯即闪烁
 *********************/
#define NET_TRIGGER_MODE					{#$NET_TRIGGER_MODE#}


#endif /* SDK_CONFIG_XXXX_H_ */
