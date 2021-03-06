/*
 * Change Logs:
 * Date             Author                Notes
 * 2021-09-28       WangHao WangPeng      first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "drv_common.h"

#include "ft6236.h"
#include "touch.h"

#define DBG_TAG "ft6236_example"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include "littlevgl2rtt.h"
#include "lvgl.h"
#include "../lvgl/lv_examples.h"

#include <rtthread.h>
#include <rtdevice.h>

#include "lv_test_theme.h"

rt_thread_t ft6236_thread;
rt_device_t touch;
LV_FONT_DECLARE(test4)
extern int sample(void);
extern void littlevgl2rtt_send_input_event(rt_int16_t x, rt_int16_t y, rt_uint8_t state);

void ft6236_thread_entry(void *parameter)
{
    struct rt_touch_data *read_data;
    read_data = (struct rt_touch_data *)rt_calloc(1, sizeof(struct rt_touch_data));
    while(1)
    {
        rt_device_read(touch, 0, read_data, 1);
        littlevgl2rtt_send_input_event(read_data->y_coordinate, read_data->x_coordinate?(320 - read_data->x_coordinate):0, read_data->event);
        rt_thread_delay(10);
    }
}

#define REST_PIN GET_PIN(D, 3)

int ft6236_example(void)
{
    struct rt_touch_config cfg;

    cfg.dev_name = "i2c2";
    rt_hw_ft6236_init("touch", &cfg, REST_PIN);
    touch = rt_device_find("touch");

    rt_device_open(touch, RT_DEVICE_FLAG_RDONLY);

    struct rt_touch_info info;
    rt_device_control(touch, RT_TOUCH_CTRL_GET_INFO, &info);
    LOG_I("type       :%d", info.type);
    LOG_I("vendor     :%d", info.vendor);
    LOG_I("point_num  :%d", info.point_num);
    LOG_I("range_x    :%d", info.range_x);
    LOG_I("range_y    :%d", info.range_y);

    ft6236_thread = rt_thread_create("touch", ft6236_thread_entry, RT_NULL, 800, 10, 20);

    if (ft6236_thread == RT_NULL)
    {
        LOG_D("create ft6236 thread err");
        return -RT_ENOMEM;
    }
    rt_thread_startup(ft6236_thread);

    return RT_EOK;
}
INIT_APP_EXPORT(ft6236_example);

#define LED_PIN GET_PIN(I, 8)


#include "stm32h7xx.h"
static int vtor_config(void)
{
    /* Vector Table Relocation in Internal QSPI_FLASH */
    SCB->VTOR = QSPI_BASE;
    return 0;
}
INIT_BOARD_EXPORT(vtor_config);
