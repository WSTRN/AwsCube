#include "DisplayBasic.hpp"
#include "lvgl.h"


static TaskHandle_t uiloopTaskHandle = NULL;
static TaskHandle_t TransloopTaskHandle = NULL;



void UILoopTask(void const * argument)
{
    lvgl_port_init();
    
    lv_obj_t * bar1 = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_size(bar1, 200, 20);
    lv_obj_align(bar1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_anim_time(bar1, 2000);
    lv_bar_set_value(bar1, 100, LV_ANIM_ON);
    //AppWindow_Create();
    

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

void lvgl_ui_init()
{
    oled_drv_init();
   
    // OLED_GRAM[25][50] = 0x07;
    // OLED_GRAM[25][51] = 0xe0;
     
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


extern "C" void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance==SPI2)
    {
        BaseType_t xHighTaskWoken=pdFALSE;
        vTaskNotifyGiveFromISR(TransloopTaskHandle,&xHighTaskWoken);
        portYIELD_FROM_ISR(xHighTaskWoken);
    }
} 