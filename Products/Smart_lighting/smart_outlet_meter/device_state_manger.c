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
#include "vendor.h"
#include "app_entry.h"
#include "device_state_manger.h"
#include "factory.h"
#include "energy_meter.h"
#include "smart_outlet_meter.h"
#include "sdk_config_xxxx.h"
#include "def_sdk_config_xxxx.h"

#define AWSS_TIMEOUT_MS              (10 * 60 * 1000)
#define CLEAR_AWSS_FLAG_TIMEOUT_MS   (30 * 1000)
#define CONNECT_AP_FAILED_TIMEOUT_MS (2 * 60 * 1000)

static aos_timer_t awss_timeout_timer;
static aos_timer_t connect_ap_failed_timeout_timer;
static aos_timer_t clear_awss_flag_timeout_timer;

static void app_start_wifi_awss(void);

static int device_net_state = UNKNOW_STATE;
int get_net_state(void)
{
    return device_net_state;
}

void set_net_state(int state)
{
    device_net_state = state;
}

static void device_stop_awss(void)
{
    awss_dev_ap_stop();
    awss_stop();
}

static void timer_func_awss_timeout(void *arg1, void *arg2)
{
    LOG("awss timeout, stop awss");
    set_net_state(AWSS_NOT_START);
    device_stop_awss();
    aos_timer_stop(&awss_timeout_timer);
}

static void timer_func_connect_ap_failed_timeout(void *arg1, void *arg2)
{
    LOG("connect ap failed timeout");
    set_net_state(CONNECT_AP_FAILED_TIMEOUT);
    aos_timer_stop(&connect_ap_failed_timeout_timer);
}

static unsigned char awss_flag = 0;
int init_awss_flag(void)
{
    unsigned char value;
    int ret, len = sizeof(value);

    ret = aos_kv_get("awss_flag", &value, &len);
    if (ret == 0 && len > 0) {
        awss_flag = value;
    }
    return 0;
}

// #define AWSS_REBOOT_TIMEOUT     (4 * 1000)
#define AWSS_RESET_TIMEOUT      (FACTORY_RESTORE * 1000)
#define KEY_PRESSED_VALID_TIME  100
#define KEY_DETECT_INTERVAL     50
// #define AWSS_REBOOT_CNT         AWSS_REBOOT_TIMEOUT / KEY_DETECT_INTERVAL       //80
#define AWSS_RESET_CNT          AWSS_RESET_TIMEOUT / KEY_DETECT_INTERVAL        //120
#define KEY_PRESSED_CNT         KEY_PRESSED_VALID_TIME / KEY_DETECT_INTERVAL    //2


void key_detect_event_task(void *arg)
{
    int nCount = 0, awss_mode = 0;
    int timeout = (AWSS_RESET_CNT < AWSS_RESET_TIMEOUT)? AWSS_RESET_CNT : AWSS_RESET_TIMEOUT;
    // MY_PRINTF("timeout = %d\n",timeout);
    // MY_PRINTF("KEY_PRESSED_CNT = %d\tAWSS_RESET_CNT = %d\t AWSS_REBOOT_CNT = %d\n",KEY_PRESSED_CNT,AWSS_RESET_CNT,AWSS_REBOOT_CNT);
    while (1) {
        #if(SWITCH_ENABLE_1 != 1)
        if (!product_get_key()) {   //获得key按键IO电平状态
        #else
        if(product_get_key()){
        #endif
            nCount++;
            if(nCount >= timeout){  //网络指示灯闪烁，提醒用户放开配网按键
                product_toggle_led();
            }
        } 
        else {
            if (nCount >= KEY_PRESSED_CNT && nCount < timeout) {
                if (product_get_switch() == ON) {
                    product_set_switch(OFF);
                    user_post_powerstate(OFF);
                } else {
                    product_set_switch(ON);
                    user_post_powerstate(ON);
                }
            }
            // MY_PRINTF("awss_flag = %d\t nCount = %d\n",awss_flag, nCount);
            // if ((awss_flag == 0) && (nCount >= AWSS_REBOOT_CNT)) {
            //     LOG("\r\n\r\n\r\n ###do awss reboot#### \r\n\r\n\r\n");
            //     do_awss_reboot();   //将awss_flag设置成1
            //     break;
            // } else if ((awss_flag == 1) && (nCount > AWSS_RESET_CNT)) {
            //     LOG("\r\n\r\n\r\n ###do awss reset#### \r\n\r\n\r\n");
            //     do_awss_reset();
            //     break;
            // }
            if(nCount > AWSS_RESET_CNT)
            {
                LOG("\r\n\r\n\r\n ###do awss reset#### \r\n\r\n\r\n");
                int ret;
                awss_flag = 1;
                int len = sizeof(awss_flag);
                ret = aos_kv_set("awss_flag", &awss_flag, len, 1);
                if (ret != 0)
                    LOG("KV Setting failed");
                do_awss_reset();
                break;
            }
            nCount = 0;
        }
        if ((awss_flag == 1) && (nCount > AWSS_RESET_CNT && awss_mode == 0)) {
            set_net_state(UNCONFIGED);
            awss_mode = 1;
        }
        aos_msleep(KEY_DETECT_INTERVAL);
    }
    aos_task_exit(0);
}

static void indicate_net_state_task(void *arg)
{
    uint32_t nCount = 0;
    uint32_t duration = 0;
    int pre_state = UNKNOW_STATE;
    int cur_state = UNKNOW_STATE;
    int switch_stat = 0;

    while (1) {
        pre_state = cur_state;
        cur_state = get_net_state();
        // MY_PRINTF("\r\n\r\n#### %d ###\r\n\r\n",cur_state);
        switch (cur_state) {
            case RECONFIGED:
                nCount++;
                #if(DEV_NET_TRIGGER_MODE_1 == 1)
                if (nCount >= 8) {
                    product_toggle_led();
                    nCount = 0;
                }
                #else
                {
                    unsigned char value;
                    int ret,len = sizeof(value);
                    ret = aos_kv_get("awss_flag", &value, &len);
                    if(awss_flag == 1){
                        product_toggle_led();
                        nCount = 0;
                    }
                    
                }
                #endif
                break;

            case UNCONFIGED:
                
                nCount++;
                #if(DEV_NET_TRIGGER_MODE_1 == 1)
                if (nCount >= 2) {
                    product_toggle_led();
                    nCount = 0;
                }
                #else
                {
                    unsigned char value;
                    int ret,len = sizeof(value);
                    ret = aos_kv_get("awss_flag", &value, &len);
                    if(awss_flag == 1){
                        product_toggle_led();
                        nCount = 0;
                    }
                    
                }
                #endif
                break;

            case AWSS_NOT_START:
                if (pre_state != cur_state) {
                    switch_stat = (int)product_get_switch();
                    LOG("[net_state]awss timeout, set led %d", switch_stat);
                    product_set_led(switch_stat);
                }
                break;

            case GOT_AP_SSID:
            case CONNECT_CLOUD_FAILED:
                nCount++;
                if (nCount >= 8) {
                    product_toggle_led();
                    nCount = 0;
                }
                break;

            case CONNECT_AP_FAILED_TIMEOUT:
                if (pre_state != cur_state) {
                    #if(WIFI_INDICATOR_DISCONNECTED_STATUS == 0)
                    switch_stat = (int)product_get_switch();
                    #elif(WIFI_INDICATOR_DISCONNECTED_STATUS == 1)
                    switch_stat = ON;
                    #elif
                    switch_stat = OFF;
                    #endif
                    LOG("[net_state]connect AP failed timeout, set led %d", switch_stat);
                    product_set_led(switch_stat);
                }
                break;

            case CONNECT_AP_FAILED:
                nCount++;
                if (nCount >= 5) {
                    product_toggle_led();
                    nCount = 0;
                }
                if (pre_state != cur_state) {
                    LOG("[net_state]connect AP failed");
                    aos_timer_new_ext(&connect_ap_failed_timeout_timer,
                    timer_func_connect_ap_failed_timeout, NULL, CONNECT_AP_FAILED_TIMEOUT_MS, 0, 1);
                    device_stop_awss();
                }
                break;

            case CONNECT_CLOUD_SUCCESS:     //连云成功
                if (pre_state != cur_state) {
                    aos_timer_stop(&awss_timeout_timer);
                    #if (WIFI_INDICATOR_CONNECTED_STATUS == 1)
                    switch_stat = ON;
                    #elif (WIFI_INDICATOR_CONNECTED_STATUS == 2)
                    switch_stat = OFF;
                    #else
                    switch_stat = (int)product_get_switch();    //指示继电器状态
                    #endif
                    LOG("[net_state]connect cloud success, set led %d", switch_stat);
                    product_set_led(switch_stat);
                }
                break;

            case APP_BIND_SUCCESS:
                if (pre_state != cur_state) {
                    set_net_state(CONNECT_CLOUD_SUCCESS);
                }
                break;

            case FACTORY_BEGIN:
                #if defined(ENERGY_METER_ENABLE)
                if (MODE_NORMAL == get_meter_mode()) {
                    LOG("[net_state]factory meter calibrate ok");
                    set_net_state(FACTORY_SUCCESS);
                }
                #endif
                break;

            case FACTORY_SUCCESS:
                if (pre_state != cur_state) {
                    LOG("[net_state]factory success, set led OFF");
                    product_set_led(OFF);
                }
                break;

            case FACTORY_FAILED_1:
                nCount++;
                if (nCount >= 5) {
                    product_toggle_led();
                    nCount = 0;
                }
                break;

            case FACTORY_FAILED_2:
                nCount++;
                if (nCount >= 2) {
                    product_toggle_led();
                    nCount = 0;
                }
                break;

            default:
                break;
        }
        aos_msleep(100);
    }

    LOG("exit quick_light mode");
    aos_task_exit(0);
}

static void timer_func_clear_awss_flag_timeout(void *arg1, void *arg2)
{
    LOG("clear awss flag");
    awss_flag = 0;
    aos_kv_del("awss_flag");
    aos_timer_stop(&clear_awss_flag_timeout_timer);
}

static void app_start_wifi_awss(void)
{
    aos_timer_new_ext(&awss_timeout_timer, timer_func_awss_timeout, NULL, AWSS_TIMEOUT_MS, 0, 1);
    aos_timer_new_ext(&clear_awss_flag_timeout_timer, timer_func_clear_awss_flag_timeout, NULL, CLEAR_AWSS_FLAG_TIMEOUT_MS, 0, 1);

#if defined (AWSS_ONESHOT_MODE)
    awss_config_press();
    do_awss();
#elif defined (AWSS_DEV_AP_MODE)
    do_awss_dev_ap();
#elif defined (AWSS_BT_MODE)
    do_ble_awss();
#else
#warning "Unsupported awss mode!!!"
#endif
}

extern void start_netmgr(void *p);
void check_factory_mode(void)
{
    int ret = 0;
    netmgr_ap_config_t ap_config;
    memset(&ap_config, 0, sizeof(netmgr_ap_config_t));

    aos_task_new("indicate net state", indicate_net_state_task, NULL, 1024);

    netmgr_get_ap_config(&ap_config);
    if (strlen(ap_config.ssid) <= 0) {
        LOG("scan factory ap, set led ON");
        product_set_led(ON);

        ret = scan_factory_ap();
        if (0 != ret) {
            set_net_state(UNCONFIGED);
            LOG("not enter factory mode, start awss");
            app_start_wifi_awss();
        }
    } else {
        if (awss_flag == 1) {
            LOG("start awss with netconfig exist");
            set_net_state(RECONFIGED);
            aos_kv_del("awss_flag");
            app_start_wifi_awss();
        } else {
            set_net_state(GOT_AP_SSID);
            LOG("start connect");
            aos_task_new("netmgr_start", start_netmgr, NULL, 5120);
        }
    }
}
