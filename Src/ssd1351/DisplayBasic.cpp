#include "DisplayBasic.hpp"
#include "lvgl.h"

static lv_disp_drv_t disp_drv;
static lv_disp_buf_t disp_buf;
static lv_color_t buf1[LV_HOR_RES_MAX * 64];
static lv_color_t buf2[LV_HOR_RES_MAX * 64];
void flush_callback(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void lvgl_port_init()
{
    lv_init();
    lv_disp_buf_init(&disp_buf, buf1, buf2, LV_HOR_RES_MAX * 64);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 128;
    disp_drv.ver_res = 128;
    disp_drv.flush_cb = flush_callback;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

}

void flush_callback(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    int16_t w = (area->x2 - area->x1 + 1);
    int16_t h = (area->y2 - area->y1 + 1);
    uint16_t size = w * h * sizeof(lv_color_t);
    SPI_NSS_LOW(); //片选拉低，选通器件
    
    RAM_Address(area->y1,area->y2,area->x1,area->x2);
    DMA_Send_Pixel(size,color_p);
    
}
extern "C" void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance==SPI2)
    {
        SPI_NSS_HIGH();
        lv_disp_flush_ready(&disp_drv);
        // BaseType_t xHighTaskWoken=pdFALSE;
        // vTaskNotifyGiveFromISR(TransloopTaskHandle,&xHighTaskWoken);
        // portYIELD_FROM_ISR(xHighTaskWoken);
    }
} 


/*实例化页面调度器*/
PageManager page(PAGE_MAX);

/*页面注册器*/
#define PAGE_REG(name)\
do{\
    extern void PageRegister_##name(uint8_t pageID);\
    PageRegister_##name(PAGE_##name);\
}while(0)

/**
  * @brief  页面初始化
  * @param  无
  * @retval 无
  */
void Pages_Init()
{
    PAGE_REG(MainClock);    //表盘
    //extern void PageRegister_MainClock(uint8_t pageID);
    //PageRegister_MainClock(PAGE_MainClock);
    // PAGE_REG(MainMenu);     //主菜单
    // PAGE_REG(TimeCfg);      //时间设置
    // PAGE_REG(Backlight);    //背光设置
    // PAGE_REG(StopWatch);    //秒表
    // PAGE_REG(Altitude);     //海拔表
    // PAGE_REG(About);        //关于
    // PAGE_REG(Game);         //游戏
    
    page.PagePush(PAGE_MainClock);//打开表盘
}


