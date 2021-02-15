#include "ui.hpp"
#include "lvgl.h"


static TaskHandle_t uiloopTaskHandle = NULL;
static TaskHandle_t TransloopTaskHandle = NULL;

static lv_disp_buf_t disp_buf;
static lv_color_t buf1[LV_HOR_RES_MAX * 10];
static lv_color_t buf2[LV_HOR_RES_MAX * 10];
void flush_callback(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);


void UILoopTask(void const * argument)
{
    lv_init();
    lv_disp_buf_init(&disp_buf, buf1, buf2, LV_HOR_RES_MAX * 10);
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 128;
    disp_drv.ver_res = 128;
    disp_drv.flush_cb = flush_callback;
    disp_drv.buffer = &disp_buf;
    lv_disp_t * disp;
    disp = lv_disp_drv_register(&disp_drv);
    //lv_theme_set_current(lv_theme_night_init(200, NULL));

    lv_disp_set_bg_color(disp,lv_color_hex(0xff0000));
    //lv_disp_set_bg_opa(disp, 0);
    lv_obj_t * label;
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    //lv_obj_set_event_cb(btn1, NULL);
    lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -40);
    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button");
    

    for(;;)
    {
        lv_task_handler(); /* let the GUI do its work */
        vTaskDelay(5);
    }
}
void TransLoopTask(void const * argument)
{
    for(;;)
    {
        RAM_Address();
        Write_Command(0x5C);
        OLED_DC_Set();  //DC拉高，发送数据
        SPI_NSS_LOW(); //片选拉低，选通器件
        HAL_SPI_Transmit_DMA(&hspi2,(uint8_t*)OLED_GRAM,128*128*2);
        vTaskDelay(5);
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        SPI_NSS_HIGH(); //片选拉高，关闭器件
        OLED_DC_Set(); //DC拉高，空闲时为高电平
    }
}

void uitask_init()
{
    oled_drv_init();
    xTaskCreate((TaskFunction_t)UILoopTask,
                (const char*)"UILoopTask",
                (uint16_t)512,
                (void*)NULL,
                (UBaseType_t)2,
                (TaskHandle_t*)&uiloopTaskHandle);
    xTaskCreate((TaskFunction_t)TransLoopTask,
                (const char*)"TransloopTask",
                (uint16_t)512,
                (void*)NULL,
                (UBaseType_t)4,
                (TaskHandle_t*)&TransloopTaskHandle);
}


void flush_callback(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint16_t(* ram)[128]=(uint16_t(*)[128])OLED_GRAM;
    uint16_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            ram[y][x]=color_p->full;//put_px(x, y, *color_p)
            color_p++;
        }
    }
    lv_disp_flush_ready(disp_drv);
}

extern "C" void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance==SPI2)
    {
        BaseType_t xHighTaskWoken=pdFALSE;
        vTaskNotifyGiveFromISR(TransloopTaskHandle,&xHighTaskWoken);
        portYIELD_FROM_ISR(xHighTaskWoken);
    }
} 
