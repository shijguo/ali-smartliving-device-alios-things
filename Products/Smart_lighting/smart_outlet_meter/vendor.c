/*
 *copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "netmgr.h"
#include "iot_export.h"
#include "iot_import.h"
#include "app_entry.h"
#include "aos/kv.h"
#include <hal/soc/gpio.h>
#include "vendor.h"
#include "device_state_manger.h"
#include "iot_import_awss.h"
#include "smart_outlet_meter.h"
#include "def_sdk_config_xxxx.h"

#if defined(HF_LPT230) || defined(HF_LPT130)
#include "hfilop/hfilop.h"
#endif

#if defined(HF_LPT130) /* config for PRODUCT: zuowei outlet */
#define LED_GPIO    22
#define RELAY_GPIO  5
#define KEY_GPIO    4
#elif defined(HF_LPT230) || defined(UNO_91H) /* on hf-lpt230 EVB */
#define LED_GPIO    8
#define RELAY_GPIO  24
#define KEY_GPIO    25
#elif defined(MK3080) /* on mk3080 EVB */
#define LED_GPIO    9
#define RELAY_GPIO  7
#define KEY_GPIO    12
#elif defined(MX1270) /* on mx1270 EVB  */
#define LED_GPIO    15
#define RELAY_GPIO  7
#define KEY_GPIO    1
#elif defined(BK7231UDEVKITC)
#define LED_GPIO    15
#define RELAY_GPIO  17
#define KEY_GPIO    28
// #elif defined(AMEBAZ_DEV) /* on amebaz_dev */
// #define LED_GPIO    9
// #define RELAY_GPIO  7
// #define KEY_GPIO    12
#else /* default config  */
#define LED_GPIO    DEV_WIFI_INDICATOR_PORT  //LED灯
#define RELAY_GPIO  DEV_RELAY_1_PORT    //继电器                   
#define KEY_GPIO    DEV_SWITCH_PORT     //配网按键
#endif

static gpio_dev_t io_led;
static gpio_dev_t io_key;
static gpio_dev_t io_relay;

static int led_state = -1;
void product_set_led(bool state)
{
    if (led_state == (int)state) {
        return;
    }

#if defined(HF_LPT130) || defined(UNO_91H)
    if (state) {
        hal_gpio_output_high(&io_led);
    } else {
        hal_gpio_output_low(&io_led);
    }
#else
    #if(RELAY_INDICATOR_ENABLE_1 != 1)  //低有效
    if (state) {
        hal_gpio_output_low(&io_led);
    } else {
        hal_gpio_output_high(&io_led);
    }
    #else
    if (state) {
        hal_gpio_output_high(&io_led);
    } else {
        hal_gpio_output_low(&io_led);
    }
    #endif
#endif
    led_state = (int)state;
}

static bool product_get_led()
{
    return (bool)led_state;
}

void product_toggle_led()
{
    if (product_get_led() == ON) {
        product_set_led(OFF);
    } else {
        product_set_led(ON);
    }
}

static int switch_state = -1;
void product_init_switch(void)
{
    int state = ON;
    int len = sizeof(int);
    int ret = 0;

#if (REBOOT_STATE == LAST_STATE)
    ret = aos_kv_get(KV_KEY_SWITCH_STATE, (void *)&state, &len);
    LOG("get kv save switch state:%d, ret:%d", state, ret);
    if (ret != 0)
        state = ON;
#elif (REBOOT_STATE == POWER_OFF)
    state == OFF;
#else
    state == ON;
#endif
    if (state == OFF) {
        product_set_switch(OFF);
    } else {
        product_set_switch(ON);
    }
}

#if (REBOOT_STATE == LAST_STATE)
aos_timer_t g_timer_period_save_device_status;
#define PERIOD_SAVE_DEVICE_STATUS_INTERVAL (1000 * 30)
static void timer_period_save_device_status_cb(void *arg1, void *arg2)
{
    int state, len = sizeof(int);
    int ret;

    ret = aos_kv_get(KV_KEY_SWITCH_STATE, (void *)&state, &len);

   if(ret != 0 || state != switch_state) {
        LOG("KV DEVICE_STATUS Update!!!,ret=%d.\n",ret);
        ret = aos_kv_set(KV_KEY_SWITCH_STATE, &switch_state, len, 0);
        if (ret < 0) LOG("KV set Error: %d\r\n", __LINE__);
    }
}
#endif

void vendor_product_init(void)
{
    sys_jtag_off(); //禁用jtag接口
    io_led.port = LED_GPIO;
    io_relay.port = RELAY_GPIO;
    io_key.port = KEY_GPIO;

    io_led.config = OUTPUT_PUSH_PULL;
    io_relay.config = OUTPUT_PUSH_PULL;
    #if (SWITCH_ENABLE_1 != 1)
    io_key.config = INPUT_PULL_UP;
    #else
    io_key.config = INPUT_PULL_DOWN;
    #endif

    hal_gpio_init(&io_led);
    hal_gpio_init(&io_relay);
    hal_gpio_init(&io_key);

    product_init_switch();
#if (REBOOT_STATE == LAST_STATE)
    aos_timer_new(&g_timer_period_save_device_status, timer_period_save_device_status_cb, NULL, PERIOD_SAVE_DEVICE_STATUS_INTERVAL, 1);
#endif
}

void product_set_switch(bool state)
{
    int ret = 0;
    //LOG("product_set_switch, state:%d, switch_state:%d", state, switch_state);
    if (switch_state == (int)state) {
        return;
    }

#if defined(HF_LPT130) || defined(UNO_91H) || defined(MX1270)
    if (state) {
        hal_gpio_output_high(&io_relay);
    } else {
        hal_gpio_output_low(&io_relay);
    }
#else
    #if(RELAY_ENABLE_1 != 1)    //低有效
    if (state) {
        hal_gpio_output_low(&io_relay);
    } else {
        hal_gpio_output_high(&io_relay);
    }
    #else                       //高有效
    if (state) {
        hal_gpio_output_high(&io_relay);
    } else {
        hal_gpio_output_low(&io_relay);
    }
    #endif
#endif
    switch_state = (int)state;
    product_set_led(state);

    update_power_state(state);
}

bool product_get_switch(void)
{
    return (bool)switch_state;
}

bool product_get_key(void)
{
    uint32_t level;
    hal_gpio_input_get(&io_key, &level);
#if defined(HF_LPT130)
    return !level;
#else
    return level;
#endif
}

int vendor_get_product_key(char *product_key, int *len)
{
    char *pk = NULL;
    int ret = -1;
    int pk_len = *len;

    ret = aos_kv_get("linkkit_product_key", product_key, &pk_len);
#if defined(HF_LPT230) || defined(HF_LPT130)
    if ((ret != 0)&&((pk = hfilop_layer_get_product_key()) != NULL)) {
        pk_len = strlen(pk);
        memcpy(product_key, pk, pk_len);
        ret = 0;
    }
#else
    /*
        todo: get pk...
    */
#endif
    if (ret == 0) {
        *len = pk_len;
    }
    return ret;
}

int vendor_get_product_secret(char *product_secret, int *len)
{
    char *ps = NULL;
    int ret = -1;
    int ps_len = *len;

    ret = aos_kv_get("linkkit_product_secret", product_secret, &ps_len);
#if defined(HF_LPT230) || defined(HF_LPT130)
    if ((ret != 0)&&((ps = hfilop_layer_get_product_secret()) != NULL)) {
        ps_len = strlen(ps);
        memcpy(product_secret, ps, ps_len);
        ret = 0;
    }
#else
    /*
        todo: get ps...
    */
#endif
    if (ret == 0) {
        *len = ps_len;
    }
    return ret;
}

int vendor_get_device_name(char *device_name, int *len)
{
    char *dn = NULL;
    int ret = -1;
    int dn_len = *len;

    ret = aos_kv_get("linkkit_device_name", device_name, &dn_len);
#if defined(HF_LPT230) || defined(HF_LPT130)
    if ((ret != 0)&&((dn = hfilop_layer_get_device_name()) != NULL)) {
        dn_len = strlen(dn);
        memcpy(device_name, dn, dn_len);
        ret = 0;
    }
#else
    /*
        todo: get dn...
    */
#endif
    if (ret == 0) {
        *len = dn_len;
    }
    return ret;
}

int vendor_get_device_secret(char *device_secret, int *len)
{
    char *ds = NULL;
    int ret = -1;
    int ds_len = *len;

    ret = aos_kv_get("linkkit_device_secret", device_secret, &ds_len);
#if defined(HF_LPT230) || defined(HF_LPT130)
    if ((ret != 0)&&((ds = hfilop_layer_get_device_secret()) != NULL)) {
        ds_len = strlen(ds);
        memcpy(device_secret, ds, ds_len);
        ret = 0;
    }
#else
    /*
        todo: get ds...
    */
#endif
    if (ret == 0) {
        *len = ds_len;
    }
    return ret;
}

int vendor_get_product_id(uint32_t *pid)
{
    int ret = -1;
    char pidStr[9] = { 0 };
    int len = sizeof(pidStr);

    ret = aos_kv_get("linkkit_product_id", pidStr, &len);
    if (ret == 0 && len < sizeof(pidStr)) {
        *pid = atoi(pidStr);
    } else {
        ret = -1;
    }
    return ret;
}

int set_device_meta_info(void)
{
    int len = 0;
    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char product_secret[PRODUCT_SECRET_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char device_secret[DEVICE_SECRET_LEN + 1] = {0};

    memset(product_key, 0, sizeof(product_key));
    memset(product_secret, 0, sizeof(product_secret));
    memset(device_name, 0, sizeof(device_name));
    memset(device_secret, 0, sizeof(device_secret));

    len = PRODUCT_KEY_LEN + 1;
    vendor_get_product_key(product_key, &len);

    len = PRODUCT_SECRET_LEN + 1;
    vendor_get_product_secret(product_secret, &len);

    len = DEVICE_NAME_LEN + 1;
    vendor_get_device_name(device_name, &len);

    len = DEVICE_SECRET_LEN + 1;
    vendor_get_device_secret(device_secret, &len);

    if ((strlen(product_key) > 0) && (strlen(product_secret) > 0) \
            && (strlen(device_name) > 0) && (strlen(device_secret) > 0)) {
        HAL_SetProductKey(product_key);
        HAL_SetProductSecret(product_secret);
        HAL_SetDeviceName(device_name);
        HAL_SetDeviceSecret(device_secret);
        LOG("pk[%s]", product_key);
        LOG("dn[%s]", device_name);
        return 0;
    } else {
        LOG("no valid device meta data");
        return -1;
    }
}

void linkkit_reset(void *p);
void vendor_device_bind(void)
{
    set_net_state(APP_BIND_SUCCESS);
}

void vendor_device_unbind(void)
{
    linkkit_reset(NULL);
}
void vendor_device_reset(void)
{
    /* do factory reset */
    // clean kv ...
    // clean buffer ...
    /* end */
    do_awss_reset();
}
